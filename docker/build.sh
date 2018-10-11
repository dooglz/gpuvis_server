mkdir -p /build && cd /build
export CC=/usr/bin/clang-6.0 && export CXX=/usr/bin/clang++-6.0
/cmake/bin/cmake /src -DGPUVIS_build_tests=ON && \
/cmake/bin/cmake --build . --config Release && \
cd /src/webserver && npm install && \
cd /
