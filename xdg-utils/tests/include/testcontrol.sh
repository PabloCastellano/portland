
if [ -z "$IC_NUM" ] ; then
	declare -i IC_NUM=1
fi

run_test() {
## USAGE:
# run_test test_func

# if tests are being run with TET or the test runner
if [  ! -z "$USING_TET" -o ! -z "$USING_TEST_RUNNER" ]; then
	## The TET framework REQUIRES variables of the form $ic1 ... $icN
	## where each variable is a list of test functions. Here, there is only
	## one test function per $icN variable. Each $icN variable MUST be present 
	## in the $iclist
	export ic$IC_NUM="$1"
	export iclist="$iclist ic$IC_NUM"

	## The standalone test runner executes each function in $TEST_LIST
	export TEST_LIST="$TEST_LIST $1"

else # test is being run directly.
 	( "$1" )
fi
IC_NUM=$(($IC_NUM+1))

}

repeat_test() {
## USAGE: 
# repeat_test test_func N var1 ... varN var1_value1 ... var1_valueM ... varN_valueM
# where N is the number of values to substiute and M is the number of 
# values each varable takes
# 
# EXAMPLE
# repeat_test copy_file 2 INPUT OUTPUT infile1 infile2 outfile1 outfile2
# 
# NOTE - all variables MUST have the same number of arguments

FUNC="$1"
VARS="$2"
shift 2

## get list of variables
declare -i I=1
while [ "$I" -le "$VARS" ]; do
	eval "v$I=$1"
	shift 1

	eval "out=\$v$I"
	#echo "v$I=$out"
	I=$(($I+1))
done

#echo "----"

## $LENGTH is the number of values each variable takes
declare -i LENGTH=$(( $# / $VARS ))
#echo "list size: $LENGTH"

## Main loop: create a test function for each set of values.
declare -i I=1
declare -i J=1
#set -x
while [ "$J" -le "$LENGTH" ] ; do
	declare -i I=1
	## Begin test function string
	str="t$IC_NUM() {" # it is only safe to use $IC_NUM since run_test is used later in this function.
	while [ "$I" -le "$VARS" ] ; do
		## Assign each value to appropriate variable
		eval "var=\$v$I"
		eval "value=\$$(( ($I-1)*$VARS+$J ))"
		#echo "$var: $value"
		str="$str
		$var=\"$value\""
	
		I=$(($I+1))
	done
	## Close the test function and load it
	str="$str
		$FUNC
	}"
	#echo "$str"
	eval "$str"
	run_test "t$IC_NUM"

	J=$(($J+1))
done
}

