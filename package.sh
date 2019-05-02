#!/usr/bin/bash

SELFDIR=$(cd $(dirname $0);pwd)


#	决定 MODULE_ROOT 是哪个目录
MODULE_ROOT="${SELFDIR}"
while [ ! -f "${MODULE_ROOT}/MODULE" ]; do
	MODULE_ROOT=$(dirname "${MODULE_ROOT}")
	if [ "x${MODULE_ROOT}" == "x/" ]; then
		echo	"Can not determine the MODULE_ROOT - Not found the 'MODULE' file"
		exit	1
	fi
done


#	决定 PROJECT_ROOT 是哪个目录
PROJECT_ROOT="${MODULE_ROOT}"
while [ ! -f "${PROJECT_ROOT}/PROJECT" ]; do
	PROJECT_ROOT=$(dirname "${PROJECT_ROOT}")
	if [ "x${PROJECT_ROOT}" == "x/" ]; then
		echo 	"Can not determine the PROJECT_ROOT - Not found the 'PROJECT' file"
		exit	2
	fi
done


#	决定 BUILD_ROOT 是哪个目录
BUILD_ROOT="${PROJECT_ROOT}"
while [ ! -f "${BUILD_ROOT}/.BUILD_ROOT" ]; do
	BUILD_ROOT=$(dirname "${BUILD_ROOT}")
	if [ "x${BUILD_ROOT}" == "x/" ]; then
		echo 	"Can not determine the BUILD_ROOT - Not found the '.BUILD_ROOT' file"
		exit	3
	fi
done


#	生成构建环境变量
dos2unix 	"${BUILD_ROOT}/.BUILD_ROOT"
dos2unix 	"${PROJECT_ROOT}/PROJECT"
dos2unix 	"${MODULE_ROOT}/MODULE"
source 		"${BUILD_ROOT}/.BUILD_ROOT"	&&	\
source 		"${PROJECT_ROOT}/PROJECT"	&&	\
source 		"${MODULE_ROOT}/MODULE"
RESULT=$?
if [ ${RESULT} -ne 0 ]; then
	echo	"Load"
fi


#	清理出包目录下所有的文件
rm  -rf		"${SELFDIR}"/*.tar.gz
mkdir	-p	"${SELFDIR}/Pkg"
rm  -rf		"${SELFDIR}/Pkg"/*
mkdir	-p	"${SELFDIR}/Pkg/include"
mkdir	-p	"${SELFDIR}/Pkg/lib"


#	重新生活层出包目录下的东西
cd		"${SELFDIR}" 												&& \
find 	"DUILib" -name '*.h' -exec cp {} ${SELFDIR}/Pkg/include/ \;	&& \
cp	 	"${SELFDIR}/Bin"/*		"${SELFDIR}/Pkg/lib"				&& \
cp	 	"${SELFDIR}/Lib"/*.lib	"${SELFDIR}/Pkg/lib"				&& \
cd		"${SELFDIR}/Pkg"											&& \
tar cvf "${MODULE_NAME}-${MODULE_VERSION}.tar"	*					&& \
gzip 	"${MODULE_NAME}-${MODULE_VERSION}.tar"						&& \
mv 		"${MODULE_NAME}-${MODULE_VERSION}.tar.gz"	"${SELFDIR}"	&& \
rm	-rf	"${SELFDIR}/Pkg"
RESULT=$?
if [ ${RESULT} -ne 0 ]; then
	echo	"Create package for '${MODULE_NAME}' failed. errno=${RESULT}"
	exit	4
fi
echo	"Create package for '${MODULE_NAME}' success."


exit	0

