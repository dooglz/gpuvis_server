FROM ubuntu:bionic
LABEL maintainer="Dooglz"

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    nodejs npm clang-6.0 wget git tar make ca-certificates

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    libxml2

RUN CMAKE_URL="https://cmake.org/files/v3.12/cmake-3.12.1-Linux-x86_64.tar.gz" && \
    echo "pulling down cmake" ${CMAKE_URL}  && \
    mkdir cmake && wget -q  --no-check-certificate ${CMAKE_URL}
RUN tar --strip-components=1 -C cmake -xz -f cmake-3.12.1-Linux-x86_64.tar.gz

ADD build.sh /build.sh
RUN chmod +x /build.sh

EXPOSE 80
EXPOSE 443
#RUN cd /src/webserver && npm install
CMD  git clone https://github.com/dooglz/gpuvis_server.git /src || true && cd /src && git pull && cd / && ./build.sh && cd ~ && \
     node /src/webserver/index.js --gpuvis /build/bin/gpuvis_cli --rga /rga/rga --key /ssl/private.key --cert /ssl/soc-web-liv-32_napier_ac_uk.crt --https
