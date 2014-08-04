#!/bin/sh

cd /mnt/utmp/$1

shift

sudo /usr/pandora/scripts/op_dsp_c64.sh

./$*
