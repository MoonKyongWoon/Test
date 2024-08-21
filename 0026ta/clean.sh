#!/bin/bash

build_dir="./build"
buildui_dir="./build-ui"
debug_dir="./debug"
release_dir="./release"

if [ -d $build_dir ]
then
	echo "Clean $build_dir"
	rm -rf $build_dir/*
fi

if [ -d $buildui_dir ]
then
	echo "Clean $buildui_dir"
	rm -rf $buildui_dir/*
fi

if [ -d $debug_dir ]
then
	echo "Clean debug"
	rm -rf $debug_dir/*
fi

if [ -d $release_dir ]
then
	echo "Clean release"
	rm -rf $release_dir/*
fi

for entry in `ls`; do
	if [ -d $entry ]
	then
		if [ -d $entry/build ]
		then
			echo $entry/build" clean"
			rm -rf $entry/build/*
		fi
	fi
done
