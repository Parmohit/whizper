#!/bin/bash
echo "Script execution starts!"
g++ TCPserver.cpp -o 0TCPserver
echo "Script execution ends!"

echo "========================"
./0TCPserver $1
echo "========================"

echo "WARNING: Removing the executable!!"

rm -f ./0TCPserver
