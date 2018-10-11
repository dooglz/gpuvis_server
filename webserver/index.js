const express = require('express');
const fileUpload = require('express-fileupload');
const app = express();
const uuidv4 = require('uuid/v4');
const port = 80;
const fs = require('fs');
const https = require('https');
const filesavedir = process.cwd() + '/incomming/';
const intDir = process.cwd() + '/output/';
const useHTTPS = true;
let gpuvis = '../../../BUILD/gpuvis_server/bin/Release/gpuvis_cli.exe';

var options = {
    key: fs.readFileSync('/ssl/private.key'),
    cert: fs.readFileSync('/ssl/soc-web-liv-32_napier_ac_uk.crt'),
};

if (process.argv.length >= 2) {
    gpuvis = process.argv[2];
}

const child_process = require('child_process');
const spawn = child_process.spawn;

if (!fs.existsSync(filesavedir)) {
  fs.mkdirSync(filesavedir);
}
if (!fs.existsSync(intDir)) {
  fs.mkdirSync(intDir);
}
app.use(fileUpload());


app.use(function(req, res, next) {
  res.setHeader('Access-Control-Allow-Headers', 'accept, authorization, content-type, x-requested-with');
  res.setHeader('Access-Control-Allow-Methods', 'GET,HEAD,PUT,PATCH,POST,DELETE');
  res.setHeader('Access-Control-Allow-Origin', "*");
  next();
});

app.post('/upload', function(req, res) {
  const uuid = uuidv4();
  const uuids = uuid.substr(0, 4);
  if (!req.files || !req.files.inputfile) {
    console.info(uuids, "/upload req, but no files!");
    return res.status(400).send('No files were uploaded.');
  }
  let inputfile = req.files.inputfile;
  console.log(uuids, "file uploaded", inputfile.name);

  let outputfilename = filesavedir + uuid + '.txt';
  inputfile.mv(outputfilename, function(err) {
    if (err) {
      console.error(uuids, err);
      return res.status(500).send(uuids + " Internal Server Error");
    }
    console.log(uuids, 'File saved', outputfilename);
    //res.send(200, 'File uploaded! ' + uuids);
    processUpload(outputfilename, uuid, inputfile, res);
  });
});

//app.use(express.static('static'));

app.use('/', express.static(__dirname + '/static'));
app.post('/run', function(req, res) {
  res.send('Got a post request at /run');
  console.log(req, res);
})

if(useHTTPS){
  let server = https.createServer(opts, app);
  server.on('error',(e)=>console.error("https server error, ",e));
  server.listen(port, ()=>console.log("SSL, Listening on port "+server.address().port));
}else{
  app.listen(port, () => console.log(`Example app listening on port ${port}!`));
}


function processUpload(fileondisk, uuid, fileinMemory, res) {

  let filetype = "NA";
  if (fileinMemory.startsWith("ShaderType = IL_SHADER_COMPUTE")) {
    filetype = "RGA_ASM_COMPUTE";
  }

  console.log(uuids, 'processUpload, filetype:', filetype);

  if (filetype != "RGA_ASM_COMPUTE") {
    res.status(400).send(uuid + " Can't read filetype:", filetype);
    return;
  }

  runGpuVis(fileondisk, uuid).then(
    outfile => {
      console.log("gpuvis done ", outfile);
      res.type('application/octet-stream');
      try{
        res.sendFile(outfile);
      }catch(e){
        console.error("Can't sendfile ", outfile, e );
        res.status(500).send(uuid + " Internal Server Error");
      }
      // res.send(200, 'gpuvis done! ');
    },
    error => {
      res.status(500).send(uuid + " Internal Server Error");
    });
}

function runGpuVis(fn, uuid) {
  return new Promise(function(resolve, reject) {
    let intfilename = '\"' + intDir + uuid + '.bin'+ '\"';
    let inputfilename = '\"' + fn + '\"';
        console.log("Starting gpuvis");
    try {
      const ls = spawn(gpuvis, ["-f " + inputfilename, "-o " + intfilename, "-m"], {
        windowsVerbatimArguments: true,
        shell: true
      });

      ls.on('error', function(e) {
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
        resolve(intDir + uuid + '.bin');
      });

    } catch (e) {
      console.error("Can't spawn gpuvis", e);
      reject(e);
    }
  });
}
