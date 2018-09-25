FROM ubuntu:bionic
LABEL maintainer="Dooglz"

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    nodejs npm clang-6.0 wget git tar make ca-certificates

RUN CMAKE_URL="https://cmake.org/files/v3.12/cmake-3.12.1-Linux-x86_64.tar.gz" && \
    echo "pulling down cmake" ${CMAKE_URL}  && \
    mkdir cmake && wget -q  --no-check-certificate ${CMAKE_URL}
RUN tar --strip-components=1 -C cmake -xz -f cmake-3.12.1-Linux-x86_64.tar.gz

RUN echo "mkdir -p /build && cd /build && export CC=/usr/bin/clang-6.0 && export CXX=/usr/bin/clang++-6.0 && /cmake/bin/cmake /src -DGPUVIS_build_tests=ON && /cmake/bin/cmake --build . --config Release" >> /build.sh
RUN chmod +x /build.sh

EXPOSE 80
#RUN cd /src/webserver && npm install
CMD ./build.sh && cd ~ && node /src/webserver/index.js /build/bin/gpuvis_cli