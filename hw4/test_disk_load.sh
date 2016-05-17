#!/bin/bash

if [ $@ -eq 5 ]; then
    for i in {1..600}
    do
        sudo dd if=/dev/zero of=/media/test_disk/test$i.img bs=1 count=0 seek=5M
    done
elif [ $@ -eq 100 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=100M
elif [ $@ -eq 250 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=250M
elif [ $@ -eq 500 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=500M
else
    echo "Wrong operation"
	echo "Choose between 5, 100, 250, 500"
	echo "Example ./test_disk_load.sh 250"
fi
