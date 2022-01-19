#!/bin/bash

test_count=0
OCC="build/compiler/llircc"
OUTPUT="test/output"

function clean_up() {
    rm ./$1
	rm /tmp/$1.o
	rm /tmp/$1.s
	rm output.txt
}

function run_test() {
    for entry in $1
    do
    	name=`basename $entry .li`
    	echo $name
    	
        $OCC $entry $3 -o $name
    
	    ./$name 1>> output.txt
    	
    	diff ./output.txt $OUTPUT/$name.txt
    	if [[ $? == 0 ]] ; then
    	    echo "Pass"
    	    echo ""
    	else
    	    clean_up $name
    	    echo "Fail"
    	    echo ""
    	    exit 1
    	fi
    	
    	clean_up $name
    	
    	test_count=$((test_count+1))
    done
}

echo "Running all tests..."
echo ""

run_test 'test/*.li'

echo "$test_count tests passed successfully."
echo "Done"

