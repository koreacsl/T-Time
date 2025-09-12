import re
import numpy as np

# Read the log lines
with open("output.txt", "r", encoding="utf-8") as file:
    lines = file.readlines()

# Extract numbers at the end of lines
numbers = []
for line in lines:
    match = re.search(r'(\d+)\s*$', line)
    if match:
        numbers.append(int(match.group(1)))

if not numbers:
    print("No numbers found.")
    exit(1)

# Calculate IQR
q1 = np.percentile(numbers, 25)
q3 = np.percentile(numbers, 75)
iqr = q3 - q1

# Define lower and upper bounds
lower_bound = q1 - 1.5 * iqr
upper_bound = q3 + 1.5 * iqr

# Filter outliers
filtered_numbers = [n for n in numbers if lower_bound <= n <= upper_bound]

if not filtered_numbers:
    print("All numbers are outliers.")
    exit(1)

# Calculate the mean
average = sum(filtered_numbers) / len(filtered_numbers)

# Output
print(f"Average after removing outliers: {average:.2f}")

