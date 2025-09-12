#!/bin/bash
set -e

# Step 1: Run Python scripts
python3 ./restore/pattern_first.py
python3 ./restore/pattern_second.py

# Step 2: File to read
file="output_pattern_average.txt"

# Initialize sums and counters
sum_A=0
count_A=0
sum_B=0
count_B=0
line_number=0

# Read the file line by line
while read -r line; do
  line_number=$((line_number + 1))
  value=$(echo "$line" | awk '{print $(NF-2)}')

  if [[ $line_number -ge 1 && $line_number -le 16 ]]; then
    sum_A=$(echo "$sum_A + $value" | bc)
    count_A=$((count_A + 1))
  elif [[ $line_number -ge 17 && $line_number -le 24 ]]; then
    sum_B=$(echo "$sum_B + $value" | bc)
    count_B=$((count_B + 1))
  fi
done < "$file"

# Step 3: Calculate A and B
if [[ $count_A -gt 0 && $count_B -gt 0 ]]; then
  avg_A=$(echo "scale=6; $sum_A / $count_A" | bc)
  avg_B=$(echo "scale=6; $sum_B / $count_B" | bc)

  # Step 4: Calculate final result
  result=$(echo "scale=0; (4*$avg_B + $avg_A) / 5" | bc) 

  echo "Average A (lines 1-16): $avg_A"
  echo "Average B (lines 17-24): $avg_B"
  echo "Final result (4B + A) / 5: $result"

  # Step 5: Update pattern_third.py
  pattern_third="../parse/restore/pattern_third.py"

  if [[ -f "$pattern_third" ]]; then
    threshold_value="$result"
    noise_floor_value=$(echo "$result - 500" | bc)
    noise_ceiling_value=$(echo "$result + 2500" | bc)

    sed -i "s/^threshold = .*/threshold = ${threshold_value}/" "$pattern_third"
    sed -i "s/^noise_floor = .*/noise_floor = ${noise_floor_value}/" "$pattern_third"
    sed -i "s/^noise_ceiling = .*/noise_ceiling = ${noise_ceiling_value}/" "$pattern_third"

    echo "Successfully updated $pattern_third with:"
    echo "  threshold = $threshold_value"
    echo "  noise_floor = $noise_floor_value"
    echo "  noise_ceiling = $noise_ceiling_value"
  else
    echo "Error: $pattern_third not found."
    exit 1
  fi
else
  echo "Error: Not enough lines in file."
fi

