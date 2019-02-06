
### Native C++ simple-simulator that parses AMDGPU assembly and provides static analysis.

[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/2198/badge)](https://bestpractices.coreinfrastructure.org/projects/2198)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/83f648c94e4d41d09a57e279c611d380)](https://www.codacy.com/app/dooglz/gpuvis_server?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=dooglz/gpuvis_server&amp;utm_campaign=Badge_Grade)
[![Coverage Status](https://coveralls.io/repos/github/dooglz/gpuvis_server/badge.svg?branch=master)](https://coveralls.io/github/dooglz/gpuvis_server?branch=master)

| ApVyr Windows | ApVyr Ubuntu 16.04 | Travis Ubuntu 14.04| Travis OSX |
| ------------- | ------------- |------------- |------------- |
| [![Windows Build status](https://appveyor-matrix-badges.herokuapp.com/repos/dooglz/gpuvis-server/branch/master/1)](https://ci.appveyor.com/project/dooglz/gpuvis-server) | [![Unix Build status](https://appveyor-matrix-badges.herokuapp.com/repos/dooglz/gpuvis-server/branch/master/2)](https://ci.appveyor.com/project/dooglz/gpuvis-server) | [![](https://badges.herokuapp.com/travis/dooglz/gpuvis_server?env=CACHE_NAME=JOB1&label=clang-6)](https://travis-ci.org/dooglz/gpuvis_server) [![](https://badges.herokuapp.com/travis/dooglz/gpuvis_server?env=CACHE_NAME=JOB2&label=gcc-8)](https://travis-ci.org/dooglz/gpuvis_server)|[![](https://badges.herokuapp.com/travis/dooglz/gpuvis_server?env=CACHE_NAME=JOB0&label=osx)](https://travis-ci.org/dooglz/gpuvis_server) | 
| [![Test status](http://teststatusbadge.azurewebsites.net/api/status/dooglz/gpuvis-server)](https://ci.appveyor.com/project/dooglz/gpuvis-server) | | | |


# Building

### Use Cmake. 
#### Options:
- GPUVIS_build_tests (default: no)
  - build googletest, add tests to build.

## Project Structure:

- simulator/
    - Main simulator code. Builds to a library.
- cli /
    - cli wrapper for the lib. Does a bit of validation and can base64 encode results.
- tests /
     - googletests
- webserver /
    - nodejs webserver to interface with the native executable.
    - designed to be used with: [dooglz/gpuvis](https://github.com/dooglz/gpuvis)
- docker
    - dockerfile for building and running gpuvis+webserver

## CONTRIBUTING
See [CONTRIBUTING.md](https://github.com/dooglz/gpuvis_server/blob/master/CONTRIBUTING.md)