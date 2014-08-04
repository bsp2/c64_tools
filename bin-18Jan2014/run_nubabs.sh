#!/bin/sh

#NUB0MODE=`cat /proc/pandora/nub0/mode`
#NUB1MODE=`cat /proc/pandora/nub1/mode`

/usr/pandora/scripts/op_nubchange.sh absolute absolute

$*

#/usr/pandora/scripts/op_nubchange.sh $NUB0MODE $NUB1MODE

echo mouse >/proc/pandora/nub0/mode
echo mbuttons >/proc/pandora/nub1/mode
