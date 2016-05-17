#!/bin/bash

if [ $@ -eq 10 ]; then
    dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=10M
elif [ $@ -eq 100 ]; then
    dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=100M 
elif [ $@ -eq 250 ]; then
    dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=250M 
elif [ $@ -eq 500 ]; then
    dd if=/dev/zero of=/media/test_disk/test.img bs=1 count=0 seek=500M 
else
    echo "Wrong operation"
fi
