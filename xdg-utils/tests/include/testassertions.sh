# Adapted from 
# shfuncs : test suite common shell functions
# which was shipped with the TET example code.

. $TEST_CODE_DIR/include/testfuncs.sh

assert_exit() # execute command (saving output) and check exit code
{
    # $1 is command, $2 is expected exit code (0 or "N" for non-zero)
    eval $1 > out.stdout 2> out.stderr
    CODE=$?
    EXPECT=$2

    if [ -z "$EXPECT" ]; then
	EXPECT=0;
    fi
    if [ $EXPECT = N -a $CODE -eq 0 ]; then
	test_fail "Command ($1) gave exit code $CODE, expected nonzero"
    elif [ $EXPECT -ne $CODE ]; then
	test_fail "Command ($1) gave exit code $CODE, expected $EXPECT"
    fi
}

assert_file() {
	if [ ! -e $1 ] ; then
		test_fail "'$1' does not exist"
	elif [ ! -f $1 ] ; then
		test_fail "'$1' is not a regular file"
	elif [ ! -s $1 ] ; then
		test_fail "$1 exists, but is empty"
	fi
}

assert_nofile() {
	if [ -e $1 ] ; then
		test_fail "'$1' exists."
	fi
}


assert_nostdout() # check that nothing went to stdout
{
    if [ -s out.stdout ]
    then
	test_infoline "Unexpected output written to stdout, as shown below:"
	infofile out.stdout stdout:
	test_fail
    fi
}

assert_nostderr() # check that nothing went to stderr
{
    if [ -s out.stderr ]
    then
	test_infoline "Unexpected output written to stderr, as shown below:"
	infofile out.stderr stderr:
	test_fail
    fi
}

assert_stderr() # check that stderr matches expected error
{
    # $1 is file containing regexp for expected error
    # if no argument supplied, just check out.stderr is not empty

    case $1 in
    "")
	if [ ! -s out.stderr ]
	then
	    test_infoline "Expected output to stderr, but none written"
	    test_fail
	fi
	;;
    *)
	expfile="$1"
	OK=Y
	exec 4<&0 0< "$expfile" 3< out.stderr
	while read expline
	do
	    if read line <&3
	    then
		if expr "$line" : "$expline" > /dev/null
		then
		    :
		else
		    OK=N
		    break
		fi
	    else
		OK=N
	    fi
	done
	exec 0<&4 3<&- 4<&-
	if [ $OK = N ]
	then
	    test_infoline "Incorrect output written to stderr, as shown below"
	    infofile "$expfile" "expected stderr:"
	    infofile out.stderr "received stderr:"
	    test_fail
	fi
	;;
    esac
}

assert_stdout() # check that stderr matches expected error
{
    # $1 is file containing regexp for expected error
    # if no argument supplied, just check out.stderr is not empty

    case $1 in
    "")
	if [ ! -s out.stdout ]
	then
	    test_infoline "Expected output to stdout, but none written"
	    test_fail
	fi
	;;
    *)
	expfile="$1"
	OK=Y
	exec 4<&0 0< "$expfile" 3< out.stdout
	while read expline
	do
	    if read line <&3
	    then
		if expr "$line" : "$expline" > /dev/null
		then
		    :
		else
		    OK=N
		    break
		fi
	    else
		OK=N
	    fi
	done
	exec 0<&4 3<&- 4<&-
	if [ $OK = N ]
	then
	    test_infoline "Incorrect output written to stdout, as shown below"
	    infofile "$expfile" "expected stdout:"
	    infofile out.stdout "received stdout:"
	    test_fail
	fi
	;;
    esac
}


infofile() # write file to journal using tet_infoline
{
    # $1 is file name, $2 is prefix for tet_infoline

    prefix=$2
    while read line
    do
	test_infoline "$prefix$line"
    done < $1
}
