#!/bin/bash
echo "Script execution starts!"
g++ TCPclient.cpp -o 0TCPclient
echo "Script execution ends!"

echo "========================"
./0TCPclient
echo "========================"

echo "WARNING: Removing the executable!!"

rm -f 0TCPclient
