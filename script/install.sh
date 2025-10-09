#!/bin/bash

docker build -t my-ubuntu-dev .

# running with --privileged to be able to run strace and etc
docker run --privileged -v "$(dirname "$PWD"):/home" -it my-ubuntu-dev
