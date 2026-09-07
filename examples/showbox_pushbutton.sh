#!/bin/bash
#
# ShowBox Example: PushButton Widget
# Demonstrates various pushbutton styles and behaviors
#

# SHOWBOX_BIN="${SHOWBOX_BIN:-./apps/runtime/bin/showbox}"

while IFS=$'=' read -r key value; do
	case ${key} in
	btn_apply)
		echo "Apply button clicked: ${value}"
		;;
	btn_toggle)
		if [[ ${value} == "pressed" ]]; then
			echo "Toggle button pressed"
		else
			echo "Toggle button released"
		fi
		;;
	btn_ok)
		echo "OK button clicked"
		;;
	btn_cancel)
		echo "Cancel button clicked"
		;;
	*) ;;
	esac
done < <(

	#	${SHOWBOX_BIN} <<EODEMO
	# shellcheck disable=SC2312  # exit do showbox propositalmente ignorado no while < <(showbox)

	showbox <<EODEMO
add label "<b>PushButton Demo</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add groupbox "Button Types" grp1
add pushbutton "&Apply" btn_apply apply
add pushbutton "&Toggle Me" btn_toggle checkable
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&OK" btn_ok default apply exit
add pushbutton "&Cancel" btn_cancel exit
end frame

set title "ShowBox PushButton"
show
EODEMO

)

echo "Demo completed"
