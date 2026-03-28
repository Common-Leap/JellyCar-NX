#!/bin/bash
NRO=/home/leap/Workshop/JellyCar/Build/Switch/JellyCar.nro
LOG_DIR=/home/leap/.local/share/eden/sdmc/switch/JellyCar
OUT=/home/leap/Workshop/kiro_output/eden_run.txt

echo "Starting eden for 20 seconds..." > "$OUT"
timeout 22 eden "$NRO" >> "$OUT" 2>&1
echo "eden exit: $?" >> "$OUT"

# copy any log files written by the game
echo "--- Game log files ---" >> "$OUT"
ls -la "$LOG_DIR"/*.txt >> "$OUT" 2>&1
echo "--- Log content ---" >> "$OUT"
cat "$LOG_DIR"/Info*.txt >> "$OUT" 2>&1
