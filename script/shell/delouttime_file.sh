#!/bin/bash
. ~/.bash_profile

ACCOUTSYS_PATH=/home/accountSys

find ${ACCOUTSYS_PATH}/log/ -name "*" -ctime +7 | xargs rm -f
