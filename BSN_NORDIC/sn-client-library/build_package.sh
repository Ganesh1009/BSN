#!/bin/bash

if [ -z $1 ] 
then
	sourcepath=$(pwd)	
else
	sourcepath=$(pwd)/$1
fi

if [ ! -f /etc/lsb-release ]; then
  echo "lsb-release missing, unlikely to be a Ubuntu system"
  ubuntu=unknown
  exit 1
fi
. /etc/lsb-release

#git submodule update --init

builddir=$(pwd)/build
rm -rf $builddir
mkdir -p $builddir

packagedir=$(pwd)/package/$DISTRIB_ID-$DISTRIB_RELEASE
rm -rf $packagedir
mkdir -p $packagedir

echo $builddir 

cd $builddir
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo  -D CMAKE_INSTALL_PREFIX:PATH=$packagedir $sourcepath
cmake --build . -j
cmake --build . --target install

