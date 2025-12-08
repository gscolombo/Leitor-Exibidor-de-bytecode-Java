#! /bin/bash

read -p "Examples folder path: " path
if [ ! -d $path ]; then
    echo "Path given is not a folder.";
    exit 1;
fi

classfiles=$(find $path -iname "*.class") # Look recursively for ".class" files in path given
mkdir -p outputs
for file in $classfiles; do
    fname=$(echo $file | awk -F"/" '{print $NF}')

    output=$(make debug FILE=$file 2>&1)

    echo "Saving debug output for $fname"
    echo "$output" > outputs/$fname.out
done


