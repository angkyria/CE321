#!/bin/sh

dd if=/dev/zero of=test.img bs=1024 count=0 seek=$[1024*200]
