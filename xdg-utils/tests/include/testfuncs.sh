mkworkdir() {
if [ "x$TEST_WORK_DIR" != "x" -a -d "$TEST_WORK_DIR" ]; then
	return
fi

NOW=`date '+%-Y-%-m-%-d_%-k-%-M-%-S.%N'`

if [ "x$TEMP" = x ] ; then
	TEMP="./tmp-xdg-test"
fi

DIR="xdgt_$NOW" 
while [ -d "$DIR" ]
do
	DIR="$DIR"a
done

export TEST_WORK_DIR="$TEMP/$DIR"
mkdir -p $TEST_WORK_DIR
}

verify_test_context() {
if [ "$TEST_NAME" = "" ]; then
	echo "A test context must be established with 'test_start <name>'!"
	exit 255
fi
}

## record the test a name.
test_start () {

TEST_NAME="$0: $*"
verify_test_context
TEST_STATUS=PASS

# if [ $USING_TET ]; then
# tet_infoline $TEST_NAME
# FAIL=N
# fi

echo -n "[`date`] $TEST_NAME: "
}

test_infoline() {
verify_test_context
#FAIL_MESSAGE="$*\n$FAIL_MESSAGE"
FAIL_MESSAGE="$FAIL_MESSAGE\n$*"
#tet_infoline $*
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
TEST_STATUS=$1
test_infoline $2
}

## Called after test_init()
test_procedure() {
verify_test_context
## Make sure nothing screwed up in initilization
if [ $TEST_STATUS != PASS ]
then
	# Something failed before we could get to the test.
	test_result NORESULT "Initilization failed!"
fi
}
	
## Report the test's result.
test_result() {
verify_test_context
	if [ "x$1" != "x" ]; then
		TEST_STATUS=$1
	fi
	if [ "x$2" != x ]; then
		test_infoline $2
	fi

	echo -n $TEST_STATUS

	## Result codes MUST agree with tet_codes
	## for LSB/tet integration.
	case $TEST_STATUS in
	PASS )  RESULT=0
		;;
	FAIL )  RESULT=1
		echo -en " $FAIL_MESSAGE"
		;;
	UNTESTED ) RESULT=5
		echo -en " $FAIL_MESSAGE"
		;;
	NORESULT ) RESULT=7
		echo -en " $FAIL_MESSAGE"
		;;
	*) 	echo -en " - UNKNOWN STATUS\n$FAIL_MESSAGE"
	esac

#	if [ $USING_TET ]; then
#		if [ $FAIL = N ] ; then
#			tet_result ${1-PASS}
#		else
#			tet_result FAIL
#		fi
#	fi
	echo ""
	exit $RESULT
}

