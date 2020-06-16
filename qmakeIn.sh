#!/bin/bash
ThisDir=`pwd`
ThatDir=`dirname $1`
cd $ThatDir
qmake
cd $ThisDir
