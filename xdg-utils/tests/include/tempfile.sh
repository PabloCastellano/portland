## create a globally unique identifier (GUID)
## note that this is LIKELY to be unique, but not 100% guaranteed.
get_guid() {
	prefix=$1
	now=`date '+%F-%H%M%S.%N'`
	GUID="$prefix$now-$RANDOM"
}

get_tmpsubdir() {
	if [ ! -z "$1" ] ; then
		tmp="$1"
	else
		tmp=${TMPDIR-/tmp}
	fi
	if [ -z "$GUID" ] ; then
		get_guid
	fi
	TMPSUBDIR="$tmp/$GUID-$$"
	(umask 077 && mkdir -p $TMPSUBDIR) || {
		echo "Could not create temporary directory!" >&2 
		exit 255
	}
}

