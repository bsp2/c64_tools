#!/bin/sh

fbset -depth 16 -rgba 5,6,5

sleep 2

sudo /etc/init.d/pvr-init stop
sudo /etc/init.d/pvr-init start
