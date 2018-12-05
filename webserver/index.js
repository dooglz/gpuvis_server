const express = require('express');
const fileUpload = require('express-fileupload');
const uuidv4 = require('uuid/v4');
const fs = require('fs');
const https = require('https');
const commandLineArgs = require('command-line-args');
const RGA = require('./rga.js');
const GPUVIS = require('./gpuvis.js');

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
function PassReject(e) { return Promise.reject(e); }

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

const rga = new RGA(options.rga, options.rgaintDir);
rga.version().then(v => console.log(v));
const gpuvis = new GPUVIS(options.gpuvis, options.gpuvisintDir);
gpuvis.version().then(v => console.log(v));

app.post('/upload', function (req, res) {
  const uuid = uuidv4();
  const uuids = uuid.substr(0, 4);
  if (!req.files || !req.files.inputfile) {
    console.info(uuids, "/upload req, but no files!");
    return res.status(400).send('No files were uploaded.');
  }
  let inputfile = req.files.inputfile;
  let outputfilename = options.filesavedir + uuid + '.txt';
  inputfile.mv(outputfilename, function (err) {
    if (err) {
      console.error(uuids, err);
      return res.status(500).send(uuids + " Internal Server Error");
    }
    console.log(uuids, 'File saved', outputfilename);
    processUpload(outputfilename, uuid, inputfile, req, res);
  });
});

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
  } else if (filetypeHint == "GLSL_VERT_SOURCE") {
    filetype = "GLSL_VERT_SOURCE";
  } else if (filetypeHint == "GLSL_FRAG_SOURCE") {
    filetype = "GLSL_FRAG_SOURCE";
  } else if (filetypeHint == "GLSL_GEOM_SOURCE") {
    filetype = "GLSL_GEOM_SOURCE";
  } else if (filetypeHint == "GLSL_TESC_SOURCE") {
    filetype = "GLSL_TESC_SOURCE";
  } else if (filetypeHint == "GLSL_TESE_SOURCE") {
    filetype = "GLSL_TESE_SOURCE";
  }
  const shadertypes = ["GLSL_VERT_SOURCE", "GLSL_FRAG_SOURCE", "GLSL_GEOM_SOURCE", "GLSL_TESC_SOURCE", "GLSL_TESE_SOURCE"];
  console.log(uuid, 'processUpload, filetype:', filetype, " hint: ", filetypeHint);

  if (filetype === "NA") {
    console.warn("Can't determine file type")
    res.status(400).send(uuid + " Can't read filetype:" + filetype);
    return;
  }

  if (filetype === "RGA_ASM_COMPUTE") {
    gpuvis.run(fileondisk)
      .then((outfile) => fileExists_p(outfile), PassReject)
      .then((outfile) => sendDatatoSite_p(res, outfile), PassReject)
      .catch((rejectionError) => {
        console.error(uuid, "chain error ", rejectionError);
        res.status(500).send(uuid + " Internal Server Error");
      });
  } else if (filetype === "OCL_SOURCE") {
    //res.status(200).send(uuid + " Can't do that yet");
    let intfilename = '\"' + options.rgaintDir + uuid + '.txt' + '\"';
    rga.doCL(fileondisk, intfilename)
      .then(() => findFilesLike_p(options.rgaintDir, uuid), PassReject)
      .then((files) => gpuvis.run(files[0], fileondisk), PassReject)
      .then((outfile) => fileExists_p(outfile), PassReject)
      .then((outfile) => sendDatatoSite_p(res, outfile), PassReject)
      .catch((rejectionError) => {
        console.error(uuid, "chain error ", rejectionError);
        res.status(500).send(uuid + " Internal Server Error");
      });
  } else if (shadertypes.includes(filetype)) {
    let intfilename = '\"' + options.rgaintDir + uuid + '.txt' + '\"';
    rga.doShader(filetype, fileondisk, intfilename)
      .then(() => findFilesLike_p(options.rgaintDir, uuid), PassReject)
      .then((files) => gpuvis.run(files[0], fileondisk), PassReject)
      .then((outfile) => fileExists_p(outfile), PassReject)
      .then((outfile) => sendDatatoSite_p(res, outfile), PassReject)
      .catch((rejectionError) => {
        console.error(uuid, "chain error ", rejectionError);
        res.status(500).send(uuid + " Internal Server Error");
      });
  }
}


function sendDatatoSite_p(res, outfile) {
  return new Promise(function (resolve, reject) {
    try {
      res.type('application/octet-stream');
      res.sendFile(outfile);
      resolve();
    } catch (e) {
      //console.error(uuid, "Can't sendfile ", outfile, e);
      res.status(500).send(uuid + " Internal Server Error");
      reject("Server sendfile error", e);
    }
  });
}

//Return a promise that will resolve if file exists. Resolves with path value.
function fileExists_p(file) {
  return new Promise(function (resolve, reject) {
    fs.access(file, fs.constants.F_OK | fs.constants.R_OK, (err) => {
      if (!err) {
        resolve(file);
      } else {
        reject("Doesn't exist or Can't read " + file);
      }
    });
  });
}

//resolves to list of full file paths that match searchtag, rejects on no files.
function findFilesLike_p(directory, searchtag) {
  return new Promise(function (resolve, reject) {
    fs.readdir(directory, (err, dir) => {
      if (!err) {
        let files = [];
        dir.forEach(e => {
          if (e.includes(searchtag)) {
            files.push(directory + e);
          }
        });
        if (files < 1) {
          reject("No files like " + searchtag);
        } else {
          resolve(files);
        }
      } else {
        reject("Can't read Diriectory" + directory);
      }
    });
  });
}