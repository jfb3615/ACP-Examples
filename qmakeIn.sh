#!/bin/bash
ThisDir=`pwd`
ThatDir=`dirname $1`
cd $ThatDir
qmake6
cd $ThisDir
