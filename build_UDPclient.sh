#!/bin/bash
echo "Script execution starts!"
g++ UDPclient.cpp -o 0UDPclient
echo "Script execution ends!"

echo "========================"
./0UDPclient $1 $2
echo "========================"

echo "WARNING: Removing the executable!!"

rm -f ./0UDPclient
