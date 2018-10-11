#!/bin/bash
sudo docker kill gpuvis_server || true
sudo docker rm gpuvis_server || true

sudo docker build -t gpuvis_server -f gpuvis_server.dockerfile . || exit 1;

sudo docker run -d --name gpuvis_server \
		--hostname=gpuvis_server \
		--mount source=gpuvis_server_src,target=/src \
		--mount source=/ssl,target=/ssl
		--restart=always \
		-p 80:80 \
		gpuvis_server || exit 1;

#Run this to get inside
#dk exec -it gpuvis_server /bin/bash
