#! /bin/sh

cat /dev/zero | aespipe -P aeskey | ./dieharder -g 200 -a
