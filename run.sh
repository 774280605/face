#!/bin/bash
CUR_PATH=`pwd`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUR_PATH/bin/lib

cd bin
nohup ./zbk_service &

