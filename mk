#!/bin/bash

depends_template="\
"

if [ -s dependencies ]; then
	cmake_defines=()
	while read -r line; do
		echo "${line}" | grep -qE '^[[:space:]]*$' || cmake_defines+=("-D${line}")
	done < <(cat dependencies)
else
	echo "$depends_template" > dependencies
	echo "dependencies not set .. creating template"
	exit
fi

(cd build

if [ "${1,,}" == "build" ]; then
	rm -rf ../build/*
	cmake "${cmake_defines[@]}" ..
elif [ "$1" == "clear" ]; then
	rm -rf ../build/* ../bin/* ../lib/*
elif [ "$1" == "clean" ]; then
	make clean
	rm -rf ../bin/*
elif [ "$1" == "" ]; then
	make install
else
	echo "Invalid target '$1'"
fi

)

