#!/bin/sh

dd if=/dev/zero of=/media/test_disk/test.img bs=1024 count=0 seek=$[1024*200]
