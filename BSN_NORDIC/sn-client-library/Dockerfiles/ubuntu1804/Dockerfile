FROM ubuntu:18.04
LABEL org.label-schema.name = "sn-1804"

# update packages and install ca-certificates
RUN apt-get update && apt-get -y upgrade && apt-get -y --no-install-recommends install \
    apt-transport-https \
    apt-utils \
    ca-certificates \
    curl \
    gnupg \
    software-properties-common \
    wget

# Replace shell with bash so we can source files
RUN rm /bin/sh && ln -s /bin/bash /bin/sh

#add repositories
#gcc7
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
#cmake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
RUN apt-get update
RUN apt-get install kitware-archive-keyring
RUN rm /etc/apt/trusted.gpg.d/kitware.gpg


# build tools
RUN apt-get update --fix-missing
RUN apt-get install -y git
RUN apt-get install -y zip
RUN apt-get install -y ninja-build
RUN apt-get install -y doxygen
RUN apt-get install -y graphviz
RUN apt-get install -y cppcheck
RUN apt-get install -y lcov
RUN apt-get install -y clang-tidy
RUN apt-get install -y build-essential libssl-dev
RUN apt-get install -y gcc-7 g++-7 
RUN apt-get install -y cmake

#boost libraries  
RUN apt-get install -y libboost-system-dev 
RUN apt-get install -y libboost-program-options-dev
RUN apt-get install -y libboost-regex-dev

#setup workdir etc
WORKDIR /app
