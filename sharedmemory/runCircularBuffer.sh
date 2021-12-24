#!/bin/bash

./CircularBufferProducer &
sleep 1
./CircularBufferClient
