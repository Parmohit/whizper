#!/bin/bash
echo "Script execution starts!"
g++ pollServer.cpp -o 0pollServer
echo "Script execution ends!"

echo "========================"
./0pollServer $1
echo "========================"

echo "WARNING: Removing the executable!!"

rm -f ./0pollServer
