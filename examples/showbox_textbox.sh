#!/bin/bash
#
# ShowBox Example: TextBox Widget
# Demonstrates the textbox widget with normal, password and placeholder modes
#

#SHOWBOX_BIN="${SHOWBOX_BIN:-./apps/runtime/bin/showbox}"

while IFS=$'=' read -r key value; do
	case ${key} in
	username)
		echo "Username: ${value}"
		;;
	password)
		echo "Password: [hidden]"
		;;
	email)
		echo "Email: ${value}"
		;;
	*) ;;
	esac
done < <(

	#	${SHOWBOX_BIN} <<EODEMO
	# shellcheck disable=SC2312  # exit do showbox propositalmente ignorado no while < <(showbox)

	showbox <<EODEMO
add label "<b>Login Form</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add groupbox "Credentials" grp1
add textbox "&Username" username
set username placeholder "Enter your username"

add textbox "&Password" password
set password password
set password placeholder "Enter your password"

add textbox "&Email" email
set email placeholder "user@example.com"
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&Login" login default apply exit
add pushbutton "&Cancel" cancel exit
end frame

set username focus
set title "ShowBox TextBox"
show
EODEMO

)

echo "Form submitted"
