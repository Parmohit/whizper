#!/bin/bash
echo "Script execution starts!"
g++ UDPserver.cpp -o 0UDPserver
echo "Script execution ends!"

echo "========================"
./0UDPserver $1
echo "========================"

echo "WARNING: Removing the executable!!"

rm -f ./0UDPserver
