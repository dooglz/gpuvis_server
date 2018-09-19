const express = require('express');
const fileUpload = require('express-fileupload');
const app = express();
const uuidv4 = require('uuid/v4');
const port = 3000;
const fs = require('fs');
const filesavedir = __dirname + '/incomming/';
const intDir = __dirname + '/output/';
const gpuvis = '../../../BUILD/gpuvis_server/bin/Release/gpuvis_cli.exe';
const child_process = require('child_process');
const spawn = child_process.spawn;

if (!fs.existsSync(filesavedir)) {
  fs.mkdirSync(filesavedir);
}
if (!fs.existsSync(intDir)) {
  fs.mkdirSync(intDir);
}
app.use(fileUpload());

app.post('/upload', function (req, res) {
  const uuid = uuidv4();
  const uuids = uuid.substr(0, 4);
  if (!req.files || !req.files.inputfile) {
    console.info(uuids, "/upload req, but no files!");
    return res.status(400).send('No files were uploaded.');
  }
  let inputfile = req.files.inputfile;
  console.log(uuids, "file uploaded", inputfile.name);

  let outputfilename = filesavedir + uuid + '.txt';
  inputfile.mv(outputfilename, function (err) {
    if (err) {
      console.error(uuids, err);
      return res.status(500).send(uuids + " Internal Server Error");
    } console.log(uuids, 'File saved', outputfilename);
    //res.send(200, 'File uploaded! ' + uuids);
    dowork(outputfilename, uuid, res);
  });
});

//app.use(express.static('static'));
app.use('/', express.static(__dirname + '/static'));
app.post('/run', function (req, res) {
  res.send('Got a post request at /run');
  console.log(req, res);
})
app.listen(port, () => console.log(`Example app listening on port ${port}!`));

function dowork(fn, uuid, res) {
  parse(fn,uuid).then((outfile) => {
    console.log("gpuvis done ", outfile);
    res.type('application/octet-stream');
    res.sendFile(outfile);
   // res.send(200, 'gpuvis done! ');
  }, () => {
    res.status(500).send(uuids + " Internal Server Error");
  });

}

function parse(fn, uuid) {
  return new Promise(function (resolve, reject) {
    let intfilename =  intDir + uuid + '.bin';
    const ls = spawn(gpuvis, ['-f ' + fn, '-o \"' + intfilename + '\"', '-m'], {
      windowsVerbatimArguments: true
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

      resolve(intfilename);
    });
  });
}