#!/bin/bash
OUT=/home/leap/Workshop/JellyCar/Build/Switch/build_debug_output.txt
make -C /home/leap/Workshop/JellyCar/Build/Switch FILE_LOG=1 > "$OUT" 2>&1
echo "exit:$?" >> "$OUT"
