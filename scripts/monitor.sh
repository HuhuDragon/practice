#!/bin/bash
RUN=$1

RAW_DIR="../raw"
ROOT_DIR="../root"
PLOT_DIR="../plots"
LOG_DIR="../logs"

CONVERT_MACRO="convert.C"
ONLINE_MACRO="online.C"

INTERVAL=300   

if [ -z "$RUN" ]; then
    echo "Usage: ./monitor.sh"
    exit 1
fi

mkdir -p "$ROOT_DIR" "$PLOT_DIR" "$LOG_DIR"

trap '
status=$?
if [ $status -ne 0 ]; then
    echo "[ERROR] monitor.sh crashed at $(date)" >> "$LOGFILE"
fi
exit $status
' EXIT          #异常退出

LOGFILE="$LOG_DIR/run.log"
LAST_RAW=""

echo "[$(date)] Start monitoring" >> "$LOGFILE"


while true; do

    RAW_FILE=$(ls -t $RAW_DIR/raw*.dat 2>/dev/null | head -n 1)

    if [ -n "$RAW_FILE" ] && [ "$RAW_FILE" != "$LAST_RAW" ]; then

        RAW_NAME=$(basename "$RAW_FILE" .dat)
        CAL_NAME=${RAW_NAME/raw/cal}   #文件名中的raw替换为cal 
        ROOT_FILE="$ROOT_DIR/${CAL_NAME}.root"
        PLOT_FILE="$PLOT_DIR/${CAL_NAME}.png"
        
        echo "[$(date)] New raw file: $RAW_FILE" >> "$LOGFILE"

        if [ ! -f "$ROOT_FILE" ]; then
            echo "[$(date)] Converting raw to ROOT..." >> "$LOGFILE"
            root -l -q "$CONVERT_MACRO(\"$RAW_FILE\")" >> "$LOGFILE" 2>&1
            if [ $? -ne 0 ]; then
            echo "[ERROR] convert failed, exiting monitor" >> "$LOGFILE"
            exit 1    #失败报警
        fi
        fi

        if [ -f "$ROOT_FILE" ]; then
            root -l -q "$ONLINE_MACRO(\"$ROOT_FILE\",\"$PLOT_FILE\")" >> "$LOGFILE" 2>&1
            echo "[$(date)] Plot saved: $PLOT_FILE" >> "$LOGFILE"
        else
            echo "[$(date)] ROOT file not found after conversion!" >> "$LOGFILE"
        fi

        LAST_RAW="$RAW_FILE"
    fi

    sleep $INTERVAL
done

