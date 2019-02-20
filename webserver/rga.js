//handles calling the RGA compiler
const child_process = require('child_process');
const spawn = child_process.spawn;
const fs = require('fs');
const concat = (s) => s.split(/\r?\n/).reduce((a, c) => a + c, "");

class RGA {
    constructor(rgaBinaryLocation, rgaOutputDirectory) {
        this.rgaBinaryLocation = rgaBinaryLocation;
        this.rgaOutputDirectory = rgaOutputDirectory;
    }
    async version() {
        return concat(await this._call("--version"));
    }
    async doCL(inputfilename, outputfilename, verboseflag = false) {
        if (!inputfilename.endsWith(".cl")) {
            fs.copyFile(inputfilename, inputfilename + '.cl', (err) => {
                if (err) { return Promise.reject("can't make temp .cl file"); }
            });
            inputfilename = '\"' + inputfilename + '.cl\"';
        }
        let cmdline = ["-s rocm-cl -c gfx900 --line-numbers --isa ", outputfilename, inputfilename];
        //Todo: parse stdout to detect build failure.
        return this._call(cmdline,verboseflag);
    }
    async doFragShader(inputfilename, outputfilename, verboseflag = false) {
        let option = ["-s rocm-cl -c gfx900 --line-numbers --isa ", outputfilename, inputfilename];
        return this._call(cmdline,verboseflag);
    }
    async doShader(type, inputfilename, outputfilename, verboseflag = false) {
        let inputstring = "";
        if (type == "GLSL_VERT_SOURCE") {
            inputstring = "--vert";
        } else if (type == "GLSL_FRAG_SOURCE") {
            inputstring = "--frag";
        } else if (type == "GLSL_GEOM_SOURCE") {
            inputstring = "--geom";
        } else if (type == "GLSL_TESC_SOURCE") {
            inputstring = "--tesc";
        } else if (type == "GLSL_TESE_SOURCE") {
            inputstring = "--tese";
        }
        let cmdline = ["-s vulkan -c gfx900 --isa ", outputfilename, inputstring+' '+inputfilename];
        return this._call(cmdline,verboseflag);
    }


    _call(cmdline, verboseflag = false) {
        let p_e = () => { };
        let p_l = p_e;
        p_e = (...s) => console.error(verboseflag + " " + s);
        if (verboseflag) {
            console.log(cmdline);
            p_l = (...s) => console.log(verboseflag + " " + s);
        }
        if (typeof (cmdline) === "string") { cmdline = [cmdline]; }
        const binary = this.rgaBinaryLocation;
        return new Promise(function (resolve, reject) {
            try {
                let output = "";
                const ls = spawn(binary, cmdline, {
                    windowsVerbatimArguments: true,
                    shell: true
                });
                ls.on('error', function (e) {
                    p_e("Can't spawn Rga", e);
                    reject(e);
                });
                ls.stderr.on('data', (data) => {
                    let str = "" + data;
                    str.split(/\r?\n/).forEach((d) => p_l("rga stderr:", d));
                });
                ls.on('close', (code) => {
                    resolve(output);
                });
                ls.stdout.on('data', (data) => {
                    let str = "" + data;
                    output += data;
                    str.split(/\r?\n/).forEach((d) => p_l("rga stdout:", encodeURI(d)));
                });
            } catch (e) {
                p_e("Can't spawn Rga", e);
                reject(e);
            }
        });
    }
}
module.exports = RGA;
//export default RGA;
