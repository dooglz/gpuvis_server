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
  { name: 'filesavedir', type: String, defaultValue: process.cwd() + '/incomming/' },
  { name: 'intDir', type: String, defaultValue: process.cwd() + '/output/' },
]
const options = commandLineArgs(optionDefinitions);

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
if (!fs.existsSync(options.intDir)) {
  fs.mkdirSync(options.intDir);
}

app.use(fileUpload());

app.use(function (req, res, next) {
  res.setHeader('Access-Control-Allow-Headers', 'accept, authorization, content-type, x-requested-with');
  res.setHeader('Access-Control-Allow-Methods', 'GET,HEAD,PUT,PATCH,POST,DELETE');
  res.setHeader('Access-Control-Allow-Origin', "*");
  next();
});

app.post('/upload', function (req, res) {

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

app.use('/', express.static(__dirname + '/static'));
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
  if (filetypeHint == "RGA_ASM_COMPUTE" && headder.startsWith("ShaderType = IL_SHADER_COMPUTE")) {
    filetype = "RGA_ASM_COMPUTE";
  } else if (filetypeHint == "OCL_SOURCE" && headder.includes("__kernel")) {
    filetype = "OCL_SOURCE"
  }

  console.log(uuid, 'processUpload, filetype:', filetype, filetypeHint);

  if (filetype === "NA") {
    res.status(400).send(uuid + " Can't read filetype:" + filetype);
    return;
  }

  if (filetype === "RGA_ASM_COMPUTE") {
    runGpuVis(fileondisk, uuid).then(
      outfile => {
        console.log("gpuvis done ", outfile);
        res.type('application/octet-stream');
        try {
          res.sendFile(outfile);
        } catch (e) {
          console.error("Can't sendfile ", outfile, e);
          res.status(500).send(uuid + " Internal Server Error");
        }
        // res.send(200, 'gpuvis done! ');
      },
      error => {
        console.error("runGpuVis error", error);
        res.status(500).send(uuid + " Internal Server Error");
      });
  }else if(filetype === "OCL_SOURCE"){
    res.status(200).send(uuid + " Can't do that yet");
  }
}

function runGpuVis(fn, uuid) {
  return new Promise(function (resolve, reject) {
    let intfilename = '\"' + options.intDir + uuid + '.bin' + '\"';
    let inputfilename = '\"' + fn + '\"';
    console.log("Starting gpuvis");
    try {
      const ls = spawn(options.gpuvis, ["-f " + inputfilename, "-o " + intfilename, "-m"], {
        windowsVerbatimArguments: true,
        shell: true
      });

      ls.on('error', function (e) {
        console.error("Can't spawn gpuvis", e);
        reject(e);
      });

      //  let stdout = "";
      ls.stdout.on('data', (data) => {
        //stdout += `${data}`;
        //console.log(`stdout: ${data}`);
      });

      ls.stderr.on('data', (data) => {
        console.log(`stderr: ${data}`);
      });
      ls.on('close', (code) => {
        resolve(options.intDir + uuid + '.bin');
      });

    } catch (e) {
      console.error("Can't spawn gpuvis", e);
      reject(e);
    }
  });
}
