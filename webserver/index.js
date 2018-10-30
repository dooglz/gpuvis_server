const express = require('express');
const fileUpload = require('express-fileupload');
const uuidv4 = require('uuid/v4');
const fs = require('fs');
const https = require('https');
const commandLineArgs = require('command-line-args');

const optionDefinitions = [
  { name: 'verbose', alias: 'v', type: Boolean },
  { name: 'gpuvis', alias: 'g', type: String },
  { name: 'rga', alias: 'r', type: String },
  { name: 'port', alias: 'p', type: Number, defaultValue: 80 },
  { name: 'https', type: Boolean, defaultValue: false },
  { name: 'key', type: String },
  { name: 'cert', type: String },
  { name: 'webdir', type: String, defaultValue: __dirname + '/static/' },
  { name: 'filesavedir', type: String, defaultValue: process.cwd() + '/incomming/' },
  { name: 'gpuvisintDir', type: String, defaultValue: process.cwd() + '/gpuvis/' },
  { name: 'rgaintDir', type: String, defaultValue: process.cwd() + '/rga/' },
]
const options = commandLineArgs(optionDefinitions);

if (!fs.existsSync(options.rga)) {
  console.error("Bad rga path", options.rga);
  process.exit(1);
}
if (!fs.existsSync(options.gpuvis)) {
  console.error("Bad gpuvis path", options.gpuvis);
  process.exit(1);
}

if (options.https === true) {
  if (options.port == 80) { options.port = 443; }
  var http = require('http');
  if (!options.key || !options.cert ||
    options.key == "" || options.cert == "") {
    console.error("Bad SSL key/cert");
    process.exit(1);
  }
  try {
    let key = fs.readFileSync(options.key);
    let crt = fs.readFileSync(options.cert);
    var https_options = { key: key, cert: crt };
  } catch (e) {
    console.error("Bad SSL key/cert", e);
    process.exit(1);
  }
}

const child_process = require('child_process');
const spawn = child_process.spawn;

let app = express();

if (!fs.existsSync(options.filesavedir)) {
  fs.mkdirSync(options.filesavedir);
}
if (!fs.existsSync(options.gpuvisintDir)) {
  fs.mkdirSync(options.gpuvisintDir);
}
if (!fs.existsSync(options.rgaintDir)) {
  fs.mkdirSync(options.rgaintDir);
}
app.use(fileUpload());

app.use(function (req, res, next) {
  res.setHeader('Access-Control-Allow-Headers', 'accept, authorization, content-type, x-requested-with');
  res.setHeader('Access-Control-Allow-Methods', 'GET,HEAD,PUT,PATCH,POST,DELETE');
  res.setHeader('Access-Control-Allow-Origin', "*");
  next();
});

app.post('/upload', function (req, res) {
  console.info("/upload");
  const uuid = uuidv4();
  const uuids = uuid.substr(0, 4);
  if (!req.files || !req.files.inputfile) {
    console.info(uuids, "/upload req, but no files!");
    return res.status(400).send('No files were uploaded.');
  }
  let inputfile = req.files.inputfile;
  console.log(uuids, "file uploaded", inputfile.name);

  let outputfilename = options.filesavedir + uuid + '.txt';
  inputfile.mv(outputfilename, function (err) {
    if (err) {
      console.error(uuids, err);
      return res.status(500).send(uuids + " Internal Server Error");
    }
    console.log(uuids, 'File saved', outputfilename);
    //res.send(200, 'File uploaded! ' + uuids);
    processUpload(outputfilename, uuid, inputfile, req, res);
  });
});

//app.use(express.static('static'));

app.use('/', express.static(options.webdir));
app.post('/run', function (req, res) {
  res.send('Got a post request at /run');
  console.log(req, res);
})

if (options.https) {
  let server = https.createServer(https_options, app);
  server.on('error', (e) => console.error("https server error, ", e));
  server.listen(options.port, () => console.log("SSL, Listening on port " + server.address().port));
  let insecureserver = http.createServer((req, res) => {
    res.writeHead(301, { "Location": "https://" + req.headers['host'] + req.url });
    res.end();
  });
  insecureserver.on('error', (e) => console.error("http server error, ", e));
  insecureserver.listen(80);
} else {
  app.listen(options.port, () => console.log(`Example app listening on port ${options.port}!`));
}


function processUpload(fileondisk, uuid, fileinMemory, req, res) {
  let filetype = "NA";
  const filetypeHint = req.body ? req.body.filetype : "NA";
  const headder = fileinMemory.data.toString();
  if (filetypeHint == "RGA_ASM_COMPUTE" && (headder.startsWith("ShaderType = IL_SHADER_COMPUTE") || headder.startsWith("AMD Kernel Code for"))) {
    filetype = "RGA_ASM_COMPUTE";
  } else if (filetypeHint == "OCL_SOURCE" && headder.includes("__kernel")) {
    filetype = "OCL_SOURCE"
  }

  console.log(uuid, 'processUpload, filetype:', filetype, " hint: ", filetypeHint);

  if (filetype === "NA") {
    res.status(400).send(uuid + " Can't read filetype:" + filetype);
    return;
  }

  if (filetype === "RGA_ASM_COMPUTE") {
    runGpuVis(fileondisk, uuid).then(
      outfile => {
        console.log(uuid, "gpuvis done ", outfile);
        res.type('application/octet-stream');
        try {
          res.sendFile(outfile);
        } catch (e) {
          console.error("Can't sendfile ", outfile, e);
          res.status(500).send(uuid + " Internal Server Error");
        }
      },
      error => {
        console.error("runGpuVis error", error);
        res.status(500).send(uuid + " Internal Server Error");
      });
  } else if (filetype === "OCL_SOURCE") {
    //res.status(200).send(uuid + " Can't do that yet");
    runRGA(fileondisk, uuid).then(
      outfile => {
        console.log(uuid, "Rga done", outfile);
        runGpuVis(outfile, uuid, fileondisk).then(
          outfile => {
            console.log(uuid, "gpuvis done ", outfile);
            res.type('application/octet-stream');
            try {
              res.sendFile(outfile);
            } catch (e) {
              console.error(uuid, "Can't sendfile ", outfile, e);
              res.status(500).send(uuid + " Internal Server Error");
            }
          },
          error => {
            console.error(uuid, "runGpuVis error", error);
            res.status(500).send(uuid + " Internal Server Error");
          });
      },
      error => {
        console.error(uuid, "runGpuVis error", error);
        res.status(500).send(uuid + " Internal Server Error");
      }
    )
  }
}


function runRGA(fn, uuid) {
  return new Promise(function (resolve, reject) {
    let intfilename = '\"' + options.rgaintDir + uuid + '.txt' + '\"';

    console.log(uuid, " Starting Rga");
    fs.copyFile(fn, fn + '.cl', (err) => {
      if (err) throw err;
      console.log(uuid, (inputfilename + '.cl'), "Written");
    });
    let inputfilename = '\"' + fn + '.cl\"';

    try {
      const ls = spawn(options.rga, ["-s rocm-cl -c gfx900 --line-numbers --isa " + intfilename, " " + inputfilename], {
        windowsVerbatimArguments: true,
        shell: true
      });
      ls.on('error', function (e) {
        console.error(uuid, "Can't spawn Rga", e);
        reject(e);
      });
      ls.stderr.on('data', (data) => {
        let str = "" + data;
        str.split(/\r?\n/).forEach((d) => console.log(uuid, "rga stderr:", d));
      });
      ls.on('close', (code) => {
        //find output files.
        let fileIndir = fs.readdirSync(options.rgaintDir);
        let files = [];
        for (let i = 0; i < fileIndir.length; i++) {
          if (fileIndir[i].includes(uuid)) {
            files.push(fileIndir[i]);
          }
        };
        if (files.length > 0) {
          console.log(uuid, "rga files", files);
          resolve(options.rgaintDir + files[0]);
        } else {
          console.error(uuid, "Can't see any RGA isa files!");
          reject("nofiles");
        }
      });
      ls.stdout.on('data', (data) => {
        let str = "" + data;
        str.split(/\r?\n/).forEach((d) => console.log(uuid, "rga stdout:", encodeURI(d)));
      });
    } catch (e) {
      console.error(uuid, "Can't spawn Rga", e);
      reject(e);
    }
  });
}


function runGpuVis(fn, uuid, source = "") {
  return new Promise(function (resolve, reject) {
    let intfilename = '\"' + options.gpuvisintDir + uuid + '.bin' + '\"';
    let inputfilename = '\"' + fn + '\"';
    console.log(uuid, "Starting gpuvis");
    try {
      const ls = spawn(options.gpuvis, ["-f " + inputfilename, "-o " + intfilename, "-m", (source != "" ? "-s " + source : "")], {
        windowsVerbatimArguments: true,
        shell: true
      });

      ls.on('error', function (e) {
        let str = "" + data;
        str.split(/\r?\n/).forEach((d) => console.log(uuid, "gpuvis stderr:", d));
        reject(e);
      });
      ls.stderr.on('data', (data) => {
        let str = "" + data;
        str.split(/\r?\n/).forEach((d) => console.log(uuid, "gpuvis stderr:", d));
      });
      ls.on('close', (code) => {
        resolve(options.gpuvisintDir + uuid + '.bin');
      });

    } catch (e) {
      console.error(uuid, "Can't spawn gpuvis", e);
      reject(e);
    }
  });
}

