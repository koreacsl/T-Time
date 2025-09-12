import re

log_file = "output_pattern_case2.txt"
output_file = "output_pattern_average.txt"

def calculate_iqr(values):
    sorted_vals = sorted(values)
    n = len(sorted_vals)
    q1 = sorted_vals[n // 4]
    q3 = sorted_vals[(3 * n) // 4]
    iqr = q3 - q1
    lower_bound = 0
    upper_bound = q3 + 1.0 * iqr
    RED = "\033[91m"
    RESET = "\033[0m"
    print(f"{RED}Lower bound: {lower_bound:.2f}{RESET}")
    print(f"{RED}Upper bound: {upper_bound:.2f}{RESET}")
    return lower_bound, upper_bound

# Step 1: Read all lines
with open(log_file, "r") as f:
    lines = [line.strip() for line in f.readlines()]

# Step 2: Collect all valid time diff numbers
all_values = []
aggregated = {}

for line in lines:
    match = re.search(r'time diff\s*:\s*(\d+)', line)
    if match:
        val = int(match.group(1))
        all_values.append(val)

# Step 3: Calculate IQR for all values
lower, upper = calculate_iqr(all_values)

# Step 4: Re-aggregate logs but apply the global filter
current_group = []
grouped = []

for line in lines:
    if line.startswith("--- Experiment"):
        if current_group:
            grouped.append(current_group)
            current_group = []
    elif line:
        current_group.append(line)
if current_group:
    grouped.append(current_group)

aggregated = {}

for group in grouped:
    for i, line in enumerate(group):
        match = re.search(r'time diff\s*:\s*(\d+)', line)
        if match:
            val = int(match.group(1))
            if lower <= val <= upper:  # <<<<< Global IQR filter applied here
                if i not in aggregated:
                    aggregated[i] = {"log": line.rsplit(":", 1)[0], "values": []}
                aggregated[i]["values"].append(val)

# Step 5: Write output
with open(output_file, "w") as out_f:
    for i in sorted(aggregated.keys()):
        log_prefix = aggregated[i]["log"]
        values = aggregated[i]["values"]
        if values:
            avg = sum(values) / len(values)
            out_f.write(f"{log_prefix}: {avg:.2f}  ({len(values)} samples)\n")
        else:
            out_f.write(f"{log_prefix}: No valid samples\n")

