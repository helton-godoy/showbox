j#!/bin/bash
SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"
DIALOGBOX=${1:-./dist_qt6/dialogbox}

if [[ ! -x ${SHOWBOX_BIN} ]]; then
	echo "Error: ${SHOWBOX_BIN} not found or not executable."
	exit 1
fi

FIFO=$(mktemp -u)
mkfifo "${FIFO}"
trap "rm -f ${FIFO}" EXIT

${SHOWBOX_BIN} <"${FIFO}" &
DIALOG_PID=$!

exec 3>"${FIFO}"

# Setup UI
echo "add label 'ZFS Dashboard' header" >&3
echo "set header stylesheet 'font-size: 18px; font-weight: bold; color: #333;'" >&3

echo "add chart 'IOPS (Log Scale)' iops_chart" >&3
echo "set iops_chart axis 'log 1 10000'" >&3
echo "set iops_chart data 'Read:500;Write:150'" >&3
echo "set iops_chart append" >&3

echo "add chart 'Pool Capacity' capacity_chart" >&3
echo "set capacity_chart data 'Used:75;Free:25'" >&3

echo "add table 'Dataset;Used;Available;Refer' datasets_table" >&3
echo "set datasets_table add_line 'tank;1.2T;2.8T;96K'" >&3
echo "set datasets_table add_line 'tank/root;10G;2.8T;10G'" >&3
echo "set datasets_table add_line 'tank/home;500G;2.8T;500G'" >&3

echo "add pushbutton 'Export Charts' btn_export" >&3
echo "add pushbutton 'Refresh Data' btn_refresh" >&3
echo "add pushbutton 'Exit' btn_exit exit" >&3

echo "show" >&3

# Interaction Loop (Simulated)
# In real scenario, we would read stdout of dialogbox
# But for demo we just keep it open.

# Let's verify export works by command
# echo "set iops_chart export 'iops.png'" >&3

wait "${DIALOG_PID}"
