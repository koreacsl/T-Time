log_file = "input_log.txt"
output_file_case2 = "output_pattern_case2.txt"

target_pattern = [
    "mpi_select",
    "mpi_core_montmul",
    "mpi_select",
    "mpi_core_montmul",
    "mpi_select",
    "mbedtls_mpi_exp_mod",
    "mpi_core_montmul",
    "mbedtls_mpi_exp_mod"
]

with open(log_file, "r") as f:
    lines = [line.strip() for line in f.readlines()]

extracted = []
for line in lines:
    if "] [" in line:
        func = line.split("] [")[1].split("]")[0]
        extracted.append(func)
    else:
        extracted.append(None)

pattern_len = len(target_pattern)

prev_start_line = -9999  
experiment_count = 1

with open(output_file_case2, "w") as out_f:
    for i in range(len(extracted) - pattern_len + 1):
        if extracted[i:i + pattern_len] == target_pattern:
            if i - prev_start_line >= 100:
                out_f.write(f"\n--- Experiment {experiment_count} ---\n")
                experiment_count += 1

            out_f.write(lines[i] + "\n")
            out_f.write(lines[i + 6] + "\n")

            prev_start_line = i

