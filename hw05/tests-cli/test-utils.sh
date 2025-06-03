#!/bin/false


#   ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
#   ┃   𝐃𝐎 𝐍𝐎𝐓 𝐄𝐃𝐈𝐓 𝐓𝐇𝐈𝐒 𝐅𝐈𝐋𝐄   ┃
#   ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


# This file contains utils that can help you create directory trees. You will
# not find any tests here, however. Look for files named ‹test-*.sh› instead.



if [ -n "${_du_test_utils+loaded}" ]; then
	return
fi

_du_test_utils=1

executable="${script_bin}"/dt

du() {
	timeout --kill-after 5 ${TIMEOUT:-30} \
	valgrind --log-file="$stash/valgrind.log" --errors-for-leak-kinds=all \
		--leak-check=full --show-leak-kinds=all --track-fds=yes \
		"${script_bin}/dt" "$@"
}

utils_init() {
	if [ ! -x ${executable} ]; then
		_msg_fatal "Executable '$executable' not found"
	fi
}

_get_lines() {
	if [ -f "$1" ]; then
		wc -l "$1" | cut -d' ' -f1
	else
		echo 0
	fi
}

_run_check_stderr() {
	actual=$(_get_lines "$1")
	expected=$(_get_lines "$2")

	if [ $actual -eq $expected ]; then
		return 0
	fi

	_msg_fail "stderr: Expected $expected lines, got $actual"
	_msg_info "Standard error output follows:";
	cat "$1"
	return 1
}

_begins_with() {
	case "$1" in
	"$2"*)
		true
		;;
	*)
		false
		;;
	esac
}

_check_path() {
	if ! _begins_with "$(readlink -f $1)" "$workdir"; then
		echo "BUG: Unsafe path '$1'" >&3
		exit 1
	fi
}

create_path() {
	if [ $# -ne 1 ]; then
		echo "usage: create_path PATH" >&3
		exit 1
	fi

	_check_path "$1"

	if ! mkdir --parents --mode=0755 "$1" 2>&3; then
		exit 1
	fi
}

create_file() {
	if [ $# -eq 0 ] || [ $# -gt 2 ]; then
		echo "usage: create_file PATH [SIZE]" >&3
		exit 1
	fi

	_check_path "$1"

	create_path $(dirname $1)

	if ! truncate --size=${2:-0} $1 2>&3; then
		exit 1
	fi

	if [ ${2:-0} -gt 0 ] && ! dd count="$2" bs=1 if=/dev/zero of="$1" >/dev/null 2>&1; then
		exit 1
	fi
}
