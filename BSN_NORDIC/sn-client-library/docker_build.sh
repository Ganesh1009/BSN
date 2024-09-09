#!/bin/bash
if [ -z $1 ] 
then
	ubuntu=ubuntu1804	
else
	ubuntu=ubuntu$1
fi

dockerpath=$(pwd)/Dockerfiles/$ubuntu

docker build -t sn-$ubuntu $dockerpath
docker run \
  -it \
  -v$(pwd):/app  \
  --workdir /app \
  --user $(id -u):$(id -g) \
  sn-$ubuntu ./build_package.sh 
