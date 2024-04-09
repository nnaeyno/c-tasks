#!/bin/sh

# Without -type f flag we aren't checking for results to be a valid files
# find . -name "*.c" 


# Finding all C files 
find . -name "*.c" -type f | while read -r file; do
    # We will use this if we want to copy files in working directory (not in subdirectories they are currently in)
    # base_file=$(basename "$file")
    
    # Copy and rename 
    cp "$file" "${file}.org"
done