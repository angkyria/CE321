#!/bin/bash

<<<<<<< HEAD
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
||||||| merged common ancestors
dd if=/dev/zero of=test.img bs=1024 count=0 seek=$[1024*200]
=======
dd if=/dev/zero of=/media/test_disk/test.img bs=1024 count=0 seek=$[1024*200]
>>>>>>> c87b30b5f53c614c6ef24b35a423ad524f69efe8
