#!/bin/bash
#
# Demo2 - Input de dados com ShowBox
# Teste de compatibilidade - widgets de entrada
#

# SHOWBOX_BIN="${SHOWBOX_BIN:-./apps/runtime/bin/showbox}"

while IFS=$'=' read -r key value; do
	case ${key} in
	cb1)
		if [[ ${value} == "1" ]]; then
			echo "Option 1 is checked"
		else
			echo "Option 1 is unchecked"
		fi
		;;
	txt1)
		echo "Text entered: ${value}"
		;;
	okay)
		echo "User clicked Ok pushbutton"
		;;
	cancel)
		echo "User clicked Cancel pushbutton"
		;;
	*) ;;
	esac
done < <(

	#	${SHOWBOX_BIN} <<EODEMO
	# shellcheck disable=SC2312  # exit do showbox propositalmente ignorado no while < <(showbox)

	showbox <<EODEMO
add checkbox "&Option 1" cb1
add textbox "&Text field" txt1 "text to edit"
add frame horizontal
add stretch
add pushbutton O&k okay apply exit
add pushbutton &Cancel cancel exit
end frame
set okay default
set cb1 focus
set title "ShowBox Demo 2"
show
EODEMO

)

echo "Demo2 completed"
