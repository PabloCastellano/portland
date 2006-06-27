## Some tasks run without an X server (eg icon install)
## This function clears the display variable.

set_no_display() {
	unset DISPLAY
}

assert_util_var() {
if [ "x$XDGUTIL" = x ]; then
	test_fail "XDGUTIL variable not set"
fi
}

