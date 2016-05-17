#!/bin/bash

if [ $@ -eq 10 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=10M
	sudo dd if=/dev/zero of=/media/test_disk/test1.img bs=1 count=0 seek=10M
	sudo dd if=/dev/zero of=/media/test_disk/test2.img bs=1 count=0 seek=10M
	sudo dd if=/dev/zero of=/media/test_disk/test3.img bs=1 count=0 seek=10M
	sudo dd if=/dev/zero of=/media/test_disk/test4.img bs=1 count=0 seek=10M
	sudo dd if=/dev/zero of=/media/test_disk/test5.img bs=1 count=0 seek=10M
elif [ $@ -eq 100 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=100M
elif [ $@ -eq 250 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=250M
elif [ $@ -eq 500 ]; then
    sudo dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=500M
else
    echo "Wrong operation"
	echo "Choose between 10, 100, 250, 500"
	echo "Example ./test_disk_load.sh 250"
fi
