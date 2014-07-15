#!/bin/bash
. ~/.bash_profile

APP_PATH=[]
#删除超过七天的日志
find ${APP_PATH}/log/ -name "*" -ctime +7 | xargs rm -f
