#!/bin/bash

docker pull ubuntu:22.04 
# running with --privileged to be able to run strace and etc
docker run --privileged -v "$(dirname "$PWD"):/home" -it ubuntu:22.04 /bin/bash
