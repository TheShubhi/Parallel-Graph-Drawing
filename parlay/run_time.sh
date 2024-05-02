#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <testname>"
    exit 1
fi

# Define input directory
input_dir="../tests"

# Get the filename from the argument
filename="$1"

# Check if the input file exists
if [ ! -f "$input_dir/$filename" ]; then
    echo "Error: Input file '$filename' not found in $input_dir"
    exit 1
fi

# Read from input file and write to output file
./force_directed_drawing "$input_dir/$filename" -t
