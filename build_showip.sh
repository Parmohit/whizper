#!/bin/bash
echo "Script execution starts!"
#g++ TCPclient.cpp -o 0TCPclient
g++ showip.cpp -o 0showip
echo "Script execution ends!"

echo "========================"
#./0TCPclient
./0showip $1
echo "========================"

echo "WARNING: Removing the executable!!"

#rm -f 0TCPclient
rm -f 0showip
