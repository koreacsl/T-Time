#!/bin/bash
set -e

#if [ $# -ne 1 ]; then
#    echo "Usage: $0 <input_text_file>"
#    exit 1
#fi

INPUT_FILE=threshold_test.txt

if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found!"
    exit 1
fi

echo "Running webp_parse.py with '$INPUT_FILE'..."
python3 ./restore/webp_parse.py "$INPUT_FILE"
echo "Successfully executed webp_parse.py"

echo "Running threshold_average.py..."
average_line=$(python3 ./restore/threshold_average.py "$INPUT_FILE" | grep "Average after removing outliers")

# Extract integer part
average_value=$(echo "$average_line" | grep -oP '[0-9]+(?=\.)')

if [ -z "$average_value" ]; then
    echo "Failed to extract average value."
    exit 1
fi

echo "Parsed average value: $average_value"

# Calculate new thresholds
threshold=$((average_value + 500))
threshold2=$((average_value + 1000))
max=$((average_value + 3500))

threshold_y=$((average_value + 500)) # general
threshold_uv=$((average_value + 800)) # general

#threshold_y=$((average_value + 400)) # detail
#threshold_uv=$((average_value + 700)) # detail

echo "Updating ./restore/webp_parse.py with THRESHOLD=$threshold, THRESHOLD2=$threshold2, MAX=$max..."
sed -i "s/^THRESHOLD = .*/THRESHOLD = $threshold/" ./restore/webp_parse.py
sed -i "s/^THRESHOLD2 = .*/THRESHOLD2 = $threshold2/" ./restore/webp_parse.py
sed -i "s/^MAX = .*/MAX = $max/" ./restore/webp_parse.py

files_to_update=(
    "./restore/webp_restore_y.py"
    "./restore/webp_restore_y_inverse.py"
    "./restore/webp_restore_yuv.py"
    "./restore/webp_restore_yuv_inverse.py"
)

for file in "${files_to_update[@]}"; do
    if [ -f "$file" ]; then
        echo "Updating $file with THRESHOLD_Y=$threshold_y and THRESHOLD_UV=$threshold_uv..."
        sed -i "s/^THRESHOLD_Y = .*/THRESHOLD_Y = $threshold_y/" "$file"
        sed -i "s/^THRESHOLD_UV = .*/THRESHOLD_UV = $threshold_uv/" "$file"
    else
        echo "Warning: File $file not found, skipping."
    fi
done

echo "All threshold values updated successfully!"

