DELETE_mkworkdir() {
	if [ ! -z "$TEST_WORK_DIR" -a -d "$TEST_WORK_DIR" ]; then
		return
	fi
	
	NOW=`date '+%-Y-%-m-%-d_%-k-%-M-%-S.%N'`
	
	if [ -z "$TEMP" ] ; then
		TEMP="./tmp-xdg-test"
	fi
	
	DIR="xdgt_$NOW" 
	while [ -d "$DIR" ]
	do
		DIR="$DIR"
	done
	
	export TEST_WORK_DIR="$TEMP/$DIR"
	mkdir -p "$TEST_WORK_DIR"
}

. "$XDG_TEST_DIR/include/tempfile.sh"


verify_test_context() {
	if [ -z "$TEST_NAME" -a ! "$USING_TET" ]; then
		echo "A test context must be established with 'test_start <name>'!"
		exit 255
	fi
}

## record the test a name.
test_start () {

	TEST_NAME="$*"
	verify_test_context
	TEST_STATUS=PASS
		
	if [ $USING_TET ]; then
		tet_infoline $TEST_NAME
		FAIL=N
	else
		echo -n "[`date`] $TEST_NAME: "
	fi

}

test_infoline() {
	verify_test_context
	FAIL_MESSAGE="$FAIL_MESSAGE\n$*"
	if [ "$USING_TET" ] ; then
		tet_infoline $*
	fi
}

test_fail() {
	FAIL=Y
	TEST_STATUS=FAIL
	test_infoline $*
}

## provide a nice way to document the test purpose
test_purpose() {
	verify_test_context
	# TODO: generate a manpage or something.
}
test_note() {
	verify_test_context
	# TODO: generate even more docs.
}

## Used for setup/dependency verification.
test_init() {
	verify_test_context
}

test_status() {
	TEST_STATUS="$1"
	test_infoline "$2"
}

## Called after test_init()
test_procedure() {
	verify_test_context
	## Make sure nothing screwed up in initilization
	if [ "$TEST_STATUS" != "PASS" ]; then
		# Something failed before we could get to the test.
		FAIL=N
		test_result NORESULT "Initilization failed!"
	fi
}
	
## Report the test's result.
test_result() {
	verify_test_context

	# Set status appropriately
	if [ ! -z "$1" ]; then
		TEST_STATUS=$1
		# account for TET
		if [ "$TEST_STATUS" = "FAIL" ] ; then 
			FAIL=Y
		fi
	fi
	# if we have a message, save it
	if [ ! -z "$2" ]; then
		test_infoline $2
	fi
	if [ "$USING_TET" ]; then
#		if [ $FAIL = N ] ; then
#			tet_result ${1-PASS}
#		else
#			tet_result FAIL
#		fi
		tet_result $TEST_STATUS
	fi
	# not using tet, so print nice explanation

		[ -z "$USING_TET" ] && echo -n "$TEST_STATUS"

		## Result codes MUST agree with tet_codes
		## for LSB/tet integration.
		case "$TEST_STATUS" in
		PASS )  RESULT=0
			;;
		FAIL )  RESULT=1
			[ -z "$USING_TET" ] && echo -ne " $FAIL_MESSAGE"
			;;
		UNTESTED ) RESULT=5
			[ -z "$USING_TET" ] && echo -ne " $FAIL_MESSAGE" 
			;;
		NORESULT ) RESULT=7
			[ -z "$USING_TET" ] && echo -ne " $FAIL_MESSAGE"
			;;
		*) 	RESULT=1
			[ -z "$USING_TET" ] && echo -ne " - UNKNOWN STATUS\n$FAIL_MESSAGE"
			;;
		esac
	#fi
	[ -z "$USING_TET" ] && echo ""
	exit "$RESULT"
}

