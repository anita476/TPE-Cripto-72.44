
#!/bin/bash

docker run --privileged -v "$(dirname "$PWD"):/home" -it ubuntu:22.04 /bin/bash
