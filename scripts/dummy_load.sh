#!/bin/sh

insmod c64.ko debug=30

sleep 1

#./c64_load_notestmsg core.out
./c64_load core.out

./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10
./c64_tc 10



