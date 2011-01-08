#!/bin/sh

# 2011-01-07  Dirk Eddelbuettel  <edd@debian.org>
#
# Sorry for the odd filename -- feel free to rename -- but with 
# SVN rev510 (from today),  and on a current Ubuntu 10.10 machine 
# I can not build from source.
#
# The standard solution is recreate a full suite of auto* files as below.
#
# One reference: 
#   http://stackoverflow.com/questions/3096989/libtool-version-mismatch-error

autoreconf 
./configure
make
