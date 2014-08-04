#!/bin/sh

fbset -depth 32 -rgba 8/16,8/8,8/0,8/24

sleep 2

sudo /etc/init.d/pvr-init stop
sudo /etc/init.d/pvr-init start
