#!/bin/bash

source config.sh

mkdir -p $certs_dir

openssl req -newkey rsa:2048 -nodes \
  -keyout $certs_dir/prvtkey.pem \
  -x509 \
  -days 3650 \
  -out $certs_dir/cacert.pem \
  -subj "/CN=ESP32_Server"
