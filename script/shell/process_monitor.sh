#!/bin/bash
APP_PATH=[]
. ${APP_PATH}/.bash_profile

xid=`ps ux|grep redis-server$|grep -v grep|awk '{ print $2 }'`
if [[ ${xid} = '' ]]
then
echo "run redis-server"
nohup /usr/local/bin/redis-server > /dev/null 2>&1 &
sleep 1
fi


