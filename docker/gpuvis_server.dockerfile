FROM ubuntu:bionic
LABEL maintainer="Dooglz"

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    nodejs npm clang-6.0

EXPOSE 80
#RUN cd /src/webserver && npm install
CMD cd ~ && node /src/webserver/index.js .