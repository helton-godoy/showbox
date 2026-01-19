#!/bin/bash
#
# ShowBox Example: Chart Widget
# Demonstrates the chart widget with pie chart and data visualization
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

while IFS=$'=' read key value; do
	# Chart slice clicks are reported as chart1.slice["label"]=value
	if [[ ${key} =~ ^chart1\.slice\[\"(.+)\"\]$ ]]; then
		label="${BASH_REMATCH[1]}"
		echo "Slice '${label}' clicked with value: ${value}"
	fi
done < <(

	${SHOWBOX_BIN} <<EODEMO
add label "<b>Sales Distribution</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add chart "Sales by Region" chart1
set chart1 append "North:35"
set chart1 append "South:25"
set chart1 append "East:20"
set chart1 append "West:20"
end chart

add separator

add groupbox "Chart Options" grp1
add checkbox "Show percentages" show_pct checked
add checkbox "Enable animations" anim checked
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&Export" export apply
add pushbutton "&Close" close exit
end frame

set close default
set title "ShowBox Charts"
show
EODEMO

)

echo "Chart demo completed"
