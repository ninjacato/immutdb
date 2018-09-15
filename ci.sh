#!/bin/sh
while true
do
	inotifywait -r -e modify --format '%w%f' . | while read NEWFILE
	do
		asd=$(echo "$NEWFILE" | awk -F . '{print $NF}')
		if [ "$asd" = "cpp" ] ; then
			make tests
		elif [ "$asd" = "h" ] ; then
			make tests
		else
			echo "Changed: $NEWFILE"
		fi
	done
done
