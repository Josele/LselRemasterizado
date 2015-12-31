#!/bin/sh 
echo start
sudo rm socket;
sudo fuser -k 3000/tcp;

sudo ../node-v0.10.28-linux-arm-pi/bin/node ../Raspserver/bin/www & sudo ./daemon 

gpio write 19 0
gpio write 20 0

