#/usr/bin/bash

if [ "$1" = "--prune" ]; then
    echo "Prune images..."
    docker container prune
    docker image rm tex
    docker image prune
fi

docker build -t tex .
