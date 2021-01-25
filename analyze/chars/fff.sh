#!/usr/bin/env bash
for file in input/*
do
        echo processing $file
        file_size=$(wc -l $file | awk '{print $1}');
        if [ $(( $file_size )) -gt $MAX_SIZE ]; then
                echo 'Skipping[' $file_size ']'
                continue
        else echo 'Nooo' $file_size
        fi
        echo '___'
done
