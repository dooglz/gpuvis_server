FROM ubuntu:bionic
LABEL maintainer="Dooglz"

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential nodejs npm wget git tar make ca-certificates

#ENV CC="/usr/bin/clang" CXX="/usr/bin/clang++"

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    libxml2

RUN CMAKE_URL="https://cmake.org/files/v3.13/cmake-3.13.0-rc2-Linux-x86_64.tar.gz" && \
    echo "pulling down cmake" ${CMAKE_URL}  && \
    mkdir cmake && wget -q  --no-check-certificate ${CMAKE_URL}
RUN tar --strip-components=1 -C cmake -xz -f cmake*.tar.gz

EXPOSE 80
EXPOSE 443
#RUN cd /src/webserver && npm install
CMD  cd /src && git reset origin/master || true && cd / \
     git clone https://github.com/dooglz/gpuvis_server.git /src || true && \
     cd /src && git reset --hard origin/master || true && git pull && \
     git clone https://github.com/dooglz/gpuvis.git /web || true && \
     cd /web && git pull && \
     mkdir -p /build && cd /build && /cmake/bin/cmake /src -DGPUVIS_build_tests=OFF && /cmake/bin/cmake --build . --config Release && \
     cd /src/webserver && npm install && \
     node /src/webserver/index.js --gpuvis /build/bin/gpuvis_cli --rga /rga/rga --key /ssl/private.key --cert /ssl/soc-web-liv-32_napier_ac_uk.crt --https --webdir /web
