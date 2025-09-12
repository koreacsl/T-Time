import re

input_file = "output_pattern_average.txt"
output_file = "result.txt"

threshold = 19745
noise_floor = 19245
noise_ceiling = 22245

result_bits = []
bit_index = 0

with open(input_file, "r") as f:
    lines = [line.strip() for line in f if line.strip()]

i = 0
while i < len(lines):
    try:
        val1 = float(lines[i].split("time diff :")[1].split("(")[0].strip())
        val2 = float(lines[i + 1].split("time diff :")[1].split("(")[0].strip())
    except Exception as e:
        print(f"[SKIP] Bit {bit_index} | Line {i} → parse error: {e}")
        i += 1
        continue

    valid1 = val1 >= noise_floor
    valid2 = val2 >= noise_floor

    if valid1 and valid2 and val1 < threshold and val2 < threshold:
        result_bits.append("0")
        print(f"[INFO] Bit {bit_index} | Line {i} → Result: 0 (both below threshold)")
        bit_index += 1
        i += 2
        continue

    if valid1 and valid2 and val1 >= threshold and val2 >= threshold:
        count_above = 0
        for j in range(4):
            try:
                val = float(lines[i + j].split("time diff :")[1].split("(")[0].strip())
                if threshold <= val < noise_ceiling:
                    count_above += 1
            except:
                continue

        if count_above >= 3:
            result_bits.append("1")
            print(f"[INFO] Bit {bit_index} | Line {i} → Result: 1 (≥3 values ≥ threshold)")
        else:
            result_bits.append("0")
            print(f"[INFO] Bit {bit_index} | Line {i} → Result: 0 (4-line check failed)")

        bit_index += 1
        i += 4
        continue

    if (valid1 and val1 < threshold) or (valid2 and val2 < threshold):
        result_bits.append("0")
        print(f"[INFO] Bit {bit_index} | Line {i} → Result: 0 (one value below threshold)")
        bit_index += 1
        i += 2
        continue

    print(f"[SKIP] Bit {bit_index} | Line {i} → insufficient data (both below noise_floor)")
    i += 2

with open(output_file, "w") as f:
    f.write("".join(result_bits) + "\n")

print("\n[RESULT] Final bit sequence:", "".join(result_bits))
print(f"[SAVED] Output written to '{output_file}'")

