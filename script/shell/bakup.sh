#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage:$0 verx.x_yyyyMMdd"
	exit 0
fi

APP_PATH=/home/**/
BAK_PATH=${APP_PATH}/run_bak/$1

if [ ! -d ${BAK_PATH} ]; then
	mkdir -p ${BAK_PATH}
fi

if [ -d ${APP_PATH}/bin ]; then
	echo "cp bin..."
	cp -r ${APP_PATH}/bin ${BAK_PATH}
fi

if [ -d ${APP_PATH}/lib ]; then
	echo "cp lib..."
	cp -r ${APP_PATH}/lib ${BAK_PATH}
fi

if [ -d ${APP_PATH}/conf ]; then
	echo "cp conf..."
	cp -r ${APP_PATH}/conf ${BAK_PATH}
fi

if [ -d ${APP_PATH}/src ]; then
	echo "cp src..."
	cp -r ${APP_PATH}/src ${BAK_PATH}
fi

echo "done."
exit 0
