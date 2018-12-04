//handles calling gpuvis
const child_process = require('child_process');
const spawn = child_process.spawn;
const fs = require('fs');
const concat = (s) => s.split(/\r?\n/).reduce((a, c) => a + c, "");

class GPUVIS {
    constructor(gpuvisBinaryLocation, gpuvisOutputDirectory) {
        this.gpuvisBinaryLocation = gpuvisBinaryLocation;
        this.gpuvisOutputDirectory = gpuvisOutputDirectory;
    }
    async version() {
        // return concat(await this._call("--version"));
        return "";
    }
    async run(inputfilename, source = "") {
        let outfile = inputfilename + ".bin";
        let cmdline = [
            "-f " + inputfilename, "-o " + outfile, "-m", (source != "" ? "-s " + source : "")
        ];
        return this._call(cmdline).then(() => outfile);
    }

    _call(cmdline, verboseflag) {
        let p_e = () => { };
        let p_l = p_e;
        if (verboseflag) {
            p_e = (...s) => console.error(verboseflag + " " + s);
            p_l = (...s) => console.log(verboseflag + " " + s);
        }
        if (typeof (cmdline) === "string") { cmdline = [cmdline]; }
        const binary = this.gpuvisBinaryLocation;
        return new Promise(function (resolve, reject) {
            try {
                let output = "";
                const ls = spawn(binary, cmdline, {
                    windowsVerbatimArguments: true,
                    shell: true
                });
                ls.on('error', function (e) {
                    p_e("Can't spawn Gpuvis", e);
                    reject(e);
                });
                ls.stderr.on('data', (data) => {
                    let str = "" + data;
                    str.split(/\r?\n/).forEach((d) => p_l("Gpuvis stderr:", d));
                });
                ls.on('close', (code) => {
                    resolve(output);
                });
                ls.stdout.on('data', (data) => {
                    let str = "" + data;
                    output += data;
                    str.split(/\r?\n/).forEach((d) => p_l("Gpuvis stdout:", encodeURI(d)));
                });
            } catch (e) {
                p_e("Can't spawn Gpuvis", e);
                reject(e);
            }
        });
    }
}
module.exports = GPUVIS;