#!/bin/bash

unzip sources.zip
mv sources $1

cd /$1
chmod +x help.sh
chmod +x running.sh

cd /$1/unnamedpipe
gcc ProCliUnamedPipe.c -o ProCliUnamedPipe

cd /$1/namedpipe
gcc ProducerNamedPipe.c -o ProducerNamedPipe
gcc ClientNamedPipe.c -o ClientNamedPipe

cd /$1/socket
gcc ServerSocket.c -o ServerSocket
gcc ClientSocket.c -o ClientSocket
chmod +x runSocket.sh

cd /$1/circularbuffer
gcc CircularBufferProducer.c -lrt -pthread -o CircularBufferProducer
gcc gcc CircularBufferClient.c -lrt -pthread -o CircularBufferClient
chmod +x runCircularBuffer.sh


