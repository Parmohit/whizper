#!/bin/bash
echo "Script execution starts!"
g++ TCPclient.cpp -o 0TCPclient
echo "Script execution ends!"

echo "========================"
./0TCPclient $1
echo "========================"

echo "WARNING: Removing the executable!!"

rm -f 0TCPclient
