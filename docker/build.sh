mkdir -p /build && cd /build
/cmake/bin/cmake /src -DGPUVIS_build_tests=ON && \
/cmake/bin/cmake --build . --config Release && \
cd /src/webserver && npm install && \
cd /
