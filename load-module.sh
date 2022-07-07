#!/bin/bash

trap onexit SIGTERM

function onexit () {
	echo "EXITING with extreme prejudice"
	for i in $(ls $MODPATH/*.ko)
	do
		echo "removing $i"
		MODNAME=$(basename -s .ko $i)
		/usr/sbin/rmmod ${MODNAME}
	done
	exit 0
}
MODPATH=${MODPATH:-/modules}

for i in $(ls $MODPATH/*.ko) 
do
	echo "inserting $i"
	/usr/sbin/insmod ${i}
done

while :
do
	sleep 60
done


onexit
