#!/bin/bash                                                                     
more project_des.txt

cd /$1/unnamedpipe
more -p unnamedpipe.txt

cd /$1/namedpipe
more -p namedpipe.txt

cd /$1/socket
more -p socket.txt

cd /$1/circularbuffer
more -p circularbuffer.txt


