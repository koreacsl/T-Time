#!/bin/bash

for dir in */ ; do
    if [ -d "$dir" ]; then
        echo "Entering $dir"
        cd "$dir"
        if [ -f "Makefile" ]; then
            echo "Found Makefile in $dir. Running make all..."
            make all
        else
            echo "No Makefile found in $dir. Skipping..."
        fi
        cd ..
    fi
done

