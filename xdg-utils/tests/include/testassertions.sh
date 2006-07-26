# Adapted from 
# shfuncs : test suite common shell functions
# which was shipped with the TET example code.

. "$XDG_TEST_DIR/include/testfuncs.sh"

assert_exit() # execute command (saving output) and check exit code
{
    EXPECT="$1"
    shift 1

    # make sure nothing is hanging around from a prev. test.
    rm -f out.stdout out.stderr

    # $1 is command, $2 is expected exit code (0 or "N" for non-zero)
    ( "$@" > out.stdout 2> out.stderr ) 
    CODE="$?"

    if [ -z "$EXPECT" ]; then
	EXPECT=0;
    fi
    if [ "$EXPECT" = N -a "$CODE" -eq 0 ]; then
	test_fail "Command ($*) gave exit code $CODE, expected nonzero"
    elif [ "$EXPECT" != N ] && [ "$EXPECT" -ne "$CODE" ]; then
	test_fail "Command ($*) gave exit code $CODE, expected $EXPECT"
    fi
}

assert_interactive_notroot() {
	if [ `whoami` != 'root' ] ; then
		assert_interactive "$@"
	fi
}

assert_interactive() {
	query=$1
	expect=$2
# It seems valuable to see what happens even if the test has failed. 
# (eg fail on stdout.)
	if [ "$TEST_STATUS" = 'FAIL' -a -z "$XDG_UTILS_DEBUG_LEVEL" ] ; then
		## Don't waste user's time if test has already failed.
		test_infoline "Test has already failed. Not bothering to ask '$query'" 
		return
	fi

	if [ ! -z "$XDG_TEST_NO_INTERACTIVE" ] ; then
		test_infoline "Assumed '$query' is '$expect'"
	 	return
	fi

	if [  ! -z "$expect" ] ; then
		if [ "$expect" != y -a "$expect" != n -a "$expect" != s ] ; then
			echo "TEST SYNTAX ERROR: interactive assertions require one of (y,n,s) as choices. (found '$expect')" >&2
			exit 255
		fi
		unset result
		while [ "$result" != y -a "$result" != n ] ; do 
			echo -ne "\n\t$query [y/n]: " >&2
			read result
		done

		if [ "$expect" = s ] ; then
			if [ -z "$3" ] ; then
				echo "TEST SYNTAX ERROR: 's' requires a variable name"
				exit 255
			fi
			eval "$3=$result"
		elif [ "$result" != "$expect" ] ; then
			test_fail "User indicated '$result' instead of '$expect' in respnonse to '$query'"
		fi
	else
		echo -ne "\n\t$query [enter to continue] " >&2
		read result
	fi
}
		

assert_file_in_path() {
	search_dirs=`echo "$2" | tr ':' ' '`
	found_files=`find $search_dirs -name "$1" 2>/dev/null`

	if [ -z "$found_files" ] ; then
		test_fail "Did not find '$1' in '$2'"
	fi
}

assert_file_not_in_path() {
	search_dirs=`echo "$2" | tr ':' ' '`
	found_files=`find $search_dirs -name "$1" 2>/dev/null`

	if [ ! -z "$found_files" ] ; then
		test_fail "Found '$found_files' in $2"
	fi
}


assert_file() {
	if [ ! -e "$1" ] ; then
		test_fail "'$1' does not exist"
	elif [ ! -f "$1" ] ; then
		test_fail "'$1' is not a regular file"
	fi
	if [ -f "$2" ] ; then
		compare=`diff -wB "$1" "$2"`
		if [ ! -z "$compare" ] ; then
			test_fail "'$1' is different from '$2'. Diff is:\n$compare"
		fi
	fi
}

assert_nofile() {
	if [ -e "$1" ] ; then
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
    if [ ! -z "$XDG_UTILS_DEBUG_LEVEL" ] ; then
	test_infoline "not checking STDERR because XDG_UTILS_DEBUG_LEVEL=$XDG_UTILS_DEBUG_LEVEL"
	test_infoline out.stderr stderr:
    elif [ -s out.stderr ] ; then
	test_infoline "Unexpected output written to stderr, as shown below:"
	infofile out.stderr stderr:
	test_fail
    fi
}

assert_stderr() # check that stderr matches expected error
{
    # $1 is file containing regexp for expected error
    # if no argument supplied, just check out.stderr is not empty

    case "$1" in
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
	if [ "$OK" = N ]
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

    case "$1" in
    "")
	if [ ! -s out.stdout ]
	then
	    test_infoline "Expected output to stdout, but none written"
	    test_fail
	fi
	;;
    *)
	expfile="$1"

	if [ ! -e "$expfile" ] ; then
		test_status NORESULT "Could not find file '$expfile' to look up expected pattern!"
		return
	fi
	#expline="$*"
	#expfile="out.stdout"
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
	if [ "$OK" = N ]
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

    prefix="$2"
    while read line
    do
	test_infoline "$prefix$line"
    done < "$1"
}

require_interactive() {
    if [ ! -z "$XDG_TEST_NO_INTERACTIVE" ] ; then
	test_result UNTESTED "XDG_TEST_NO_INTERACTIVE is set, but this test needs interactive"
    fi
}

require_root() {
    if [ `whoami` != 'root' ] ; then
	test_result UNTESTED "not running as root, but test requires root privileges"
    fi
}

require_notroot() {
    if [ `whoami` = 'root' ] ; then
	test_result UNTESTED "running as root, but test must be run as a normal user"
    fi
}

set_no_display() {
	unset DISPLAY
}

assert_display() {
	if [ -z "$DISPLAY" ] ; then 
		test_fail "DISPLAY not set!"
	fi
}

assert_util_var() {
if [ "x$XDGUTIL" = x ]; then
	test_fail "XDGUTIL variable not set"
fi
}

