#!/bin/bash


trap 'onCtrlC' INT
function onCtrlC() {
    exit
}

runname=/home/bnunuclear/DAQ_2021/DAQ_IMP201405/Si_LaBr3_Test
start=0
stop=0
if [[ $# -eq 2 ]];
then
    start=$2
    stop=$2
fi

if [[ $# -eq 3 ]];
then
    start=$2
    stop=$3
fi

for (( i = start; i <= stop; i++ )); do
    printf "$1\n$i\n$i\n" | ./Data_convert
    printf "$1\n$i\n$i\n" | ./DataAna
done

