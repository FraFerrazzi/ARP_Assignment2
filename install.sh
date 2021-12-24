#!/bin/bash

unzip sources.zip
mv sources $1

cd /$1/unnamedpipe
gcc ProCliUnamedPipe.c -o ProCliUnamedPipe

cd /$1/namedpipe
gcc ProducerNamedPipe.c -o ProducerNamedPipe
gcc ClientNamedPipe.c -o ClientNamedPipe

cd /$1/socket
gcc ServerSocket.c -o ServerSocket
gcc ClientSocket.c -o ClientSocket

cd /$1/sharedmemory
gcc CircularBufferProducer.c -o CircularBufferProducer
gcc CircularBufferClient.c -o CircularBufferClient


