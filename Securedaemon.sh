#!/bin/sh 
echo start
sudo fuser -k 45000/tcp;
sudo fuser -k 3000/tcp;
sudo ./daemon 2> treneserrores.log &
sudo ../node-v0.10.28-linux-arm-pi/bin/node ../Raspserver/bin/www;


gpio write 19 0
gpio write 20 0

