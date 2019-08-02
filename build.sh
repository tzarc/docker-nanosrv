#!/bin/sh

docker build --tag tzarc/nanosrv .
docker build -t tzarc/nanosrv-test -f Dockerfile.example .
docker run --rm -p 11253:11253 -it tzarc/nanosrv-test