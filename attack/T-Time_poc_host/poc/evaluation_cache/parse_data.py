import re
import csv
import sys
import os
import pandas as pd

def extract_numbers_from_log(file_path):
    numbers = []
    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            if "[start]" in line:
                match = re.search(r'(\d+)\s*$', line)
                if match:
                    numbers.append(int(match.group(1)))
    return numbers

def remove_outliers_iqr(numbers):
    s = pd.Series(numbers)
    q1 = s.quantile(0.25)
    q3 = s.quantile(0.75)
    iqr = q3 - q1
    lower_bound = q1 - 2.0 * iqr
    upper_bound = q3 + 2.0 * iqr
    return s[(s >= lower_bound) & (s <= upper_bound)].tolist()

def save_to_csv(output_file, numbers, limit=1000000):
    limited = numbers[:limit]
    df = pd.DataFrame(limited, columns=["value"])
    df.to_csv(output_file, index=False, header=False)

def main():
    if len(sys.argv) != 2:
        print("Usage: python parse_log_to_csv.py <input_log_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    if not os.path.exists(input_file):
        print(f" File '{input_file}' not found.")
        sys.exit(1)

    output_file = os.path.splitext(input_file)[0] + "_cleaned.csv"

    raw_numbers = extract_numbers_from_log(input_file)
    cleaned_numbers = remove_outliers_iqr(raw_numbers)

    if cleaned_numbers:
        save_to_csv(output_file, cleaned_numbers, limit=1000000)
        print(f" Saved {min(len(cleaned_numbers), 1000000)} numbers to '{output_file}' (from {len(raw_numbers)} raw entries).")
    else:
        print(" No valid numbers after outlier removal.")

if __name__ == "__main__":
    main()

