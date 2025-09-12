#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_text_file>"
    exit 1
fi

INPUT_FILE=$1
LOGFILE="logfile.txt"

if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found!"
    exit 1
fi

cp "$INPUT_FILE" "$LOGFILE"
echo " Copied '$INPUT_FILE' to '$LOGFILE'"

echo " Running webp_parse.py..."

python3 ./restore/webp_parse.py $LOGFILE

if [ $? -ne 0 ]; then
    echo "❌ Error occurred in webp_parse.py"
    exit 1
fi
echo " Successfully executed webp_parse.py"
echo " Restore images..."

python3 ./restore/webp_restore_y.py
python3 ./restore/webp_restore_y_inverse.py
python3 ./restore/webp_restore_yuv.py
python3 ./restore/webp_restore_yuv_inverse.py


sz output_y.png output_y_inverse.png output_yuv.png output_yuv_inverse.png


