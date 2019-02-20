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
         return concat(await this._call("--version"));
    }
    async run(inputfilenames, outputfilename, source = "", verboseflag = false) {
        let cmdline = [
            "-f " + inputfilenames.join(' '), "-o " + outputfilename, "-m", (source != "" ? "-s " + source : "")
        ];
        return this._call(cmdline,false,verboseflag).then(() => outputfilename);
    }

    _call(cmdline, rejectonStdError = false, verboseflag = false) {
        let p_e = () => { };
        let p_l = p_e;
        p_e = (...s) => console.error(verboseflag + " " + s);
        if (verboseflag) {
            console.log(cmdline);
            p_l = (...s) => console.log(verboseflag + " " + s);
        }

        if (typeof (cmdline) === "string") { cmdline = [cmdline]; }
        const binary = this.gpuvisBinaryLocation;
        return new Promise(function (resolve, reject) {
            let anystderror = false;
            let errorstrings = "";
            try {
                let output = "";
                const ls = spawn(binary, cmdline, {
                    windowsVerbatimArguments: true,
                    shell: false,
                    detached: true
                });
                ls.on('error', function (e) {
                    p_e("Can't spawn Gpuvis", e);
                    reject(e);
                });
                ls.stderr.on('data', (data) => {
                    let str = data.toString();
                    str.split(/\r?\n/).forEach((d) =>{ errorstrings+=(d+". "); p_e("Gpuvis stderr:", d)});
                    anystderror = true;
                });
                ls.on('close', (code) => {
                    if (rejectonStdError && anystderror) {
                        reject("GPUVIS gave errors: "+ errorstrings);
                    } else {
                        resolve(output);
                    }
                });
                ls.stdout.on('data', (data) => {
                    let str = data.toString()
                    output += str;
                    str.split(/\r?\n/).forEach((d) => p_l("Gpuvis stdout:", (d)));
                });
            } catch (e) {
                p_e("Can't spawn Gpuvis", e);
                reject(e);
            }
        });
    }
}
module.exports = GPUVIS;