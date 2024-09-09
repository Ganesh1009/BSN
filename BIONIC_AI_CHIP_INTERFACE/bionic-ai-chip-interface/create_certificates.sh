#!/bin/bash

hostname=`hostname`.local
subject="/C=DE/CN=$hostname/O=DFKI/OU=AV"
openssl req -x509 -newkey rsa:4096 -keyout server_key.pem -out server_cert.pem -nodes -days 365 -subj "$subject"

installdir=dist/secrets

mkdir -p $installdir
cp server*.pem $installdir
