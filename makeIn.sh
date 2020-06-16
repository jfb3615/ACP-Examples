#!/bin/bash
ThisDir=`pwd`
ThatDir=`dirname $1`
cd $ThatDir
make 
echo "             "
echo "             "
cd $ThisDir
