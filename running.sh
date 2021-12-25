#!/bin/bash

cd /$1

echo Command: 
read user
echo

if [ $user -eq 1 ]
then
	cd /$1/unnamedpipe
	echo Launching unnamed pipe for data transferring 
	echo
	./ProCliUnamedPipe
	sleep 1

elif [ $user -eq 2 ]
then
	cd /$1/namedpipe
	echo Launching named pipe for data transferring 
	echo
	./ProducerNamedPipe
	sleep 1

elif [ $user -eq 3 ]
then
	cd /$1/socket
	> socketdata.txt
	echo Launching socket for data transferring 
	echo
	echo Decide the amount of random Data that will be generated in Mb from 1 to 100: 
	read datasizeso
	if [ $datasizeso -gt 0 ] && [ $datasizeso -lt 101 ]
	then
		echo $datasizeso >> socketdata.txt
		source runSocket.sh
		cat socketdata.txt
		sleep 1
	else
		echo
		echo ERROR!! Not the right input! Please choose an integer number between 1 and 100
		sleep 1
	fi

elif [ $user -eq 4 ]
then
	cd /$1/circularbuffer
	> circularbufferdata.txt
	echo Launching circular buffer for data transferring 
	echo
	echo Decide the amount of random Data that will be generated in Mb from 1 to 100: 
	read datasizecb
	if [ $datasizecb -gt 0 ] && [ $datasizecb -lt 101 ]
	then
		echo $datasizecb >> circularbufferdata.txt
		source runCircularBuffer.sh
		cat circularbufferdata.txt
		sleep 1
	else
		echo
		echo ERROR!! Not the right input! Please choose an integer number between 1 and 100
		sleep 1
	fi

else 
	echo Not a correct input. Please choose a number between 1 and 4.
	echo Type 1 for unnamed pipe
	echo Type 2 for named pipe
	echo Type 3 for socket
	echo Type 4 for circular buffer
	sleep 1
fi


