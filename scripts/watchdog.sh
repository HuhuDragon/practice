#!/bin/bash
RUN=$1

SCRIPT="./monitor.sh"
LOG_DIR="../logs"
WD_LOG="$LOG_DIR/watchdog.log"

RESTART_DELAY=60   # 重启前等待 1 分钟
MAX_RESTART=20     # 防止死循环

mkdir -p "$LOG_DIR"

COUNT=0

echo "[WATCHDOG] started at $(date)" >> "$WD_LOG"

while true; do

    echo "[WATCHDOG] launching monitor.sh (attempt $COUNT)" >> "$WD_LOG"

    $SCRIPT "$RUN"
    EXIT_CODE=$?   #运行moniter脚本，如果退出就开始下一步

    if [ $EXIT_CODE -eq 0 ]; then
        echo "[WATCHDOG] monitor exited normally at $(date)" >> "$WD_LOG"
        exit 0
    fi

    COUNT=$((COUNT + 1))
    echo "[WATCHDOG] monitor crashed (code=$EXIT_CODE)" >> "$WD_LOG"

    if [ $COUNT -ge $MAX_RESTART ]; then
        echo "[WATCHDOG] too many restarts, giving up" >> "$WD_LOG"

        exit 1
    fi

    sleep $RESTART_DELAY
done

