import re
import sys

# Threshold variable for numerical comparison
THRESHOLD = 20203
THRESHOLD2 = 20703
MAX = 23203
COUNT = 5  

def get_last_number(line):
    """Extract the number at the end of a line"""
    match = re.search(r'(\d+)\s*$', line)
    return int(match.group(1)) if match else None

def filter_logs(log_lines):
    filtered_logs = []
    i = 0
    is_vp8_mode = False  # Check if [VP8ParseIntraModeRow] mode is active
    last_woomin_x = -1  # Last position of [woomin_x] log
    mb_count = 0  # Counter for [VP8DecodeMB]
    flag_repeat = 0

    while i < len(log_lines):
        line = log_lines[i]
        print(f"[VP8ParseIntraModeRow] found at line {i+1} and again at line {i+3}")
        if "[VP8DecodeMB]" or "[GetCoeffsFast]" in line:
            if i + 1 >= len(log_lines):  
                return filtered_logs   

            if "[VP8ParseIntraModeRow]" in log_lines[i+1]:
                i = i+1
                continue


        # When [VP8ParseIntraModeRow] is found, set A to the line 2 lines below
        if "[VP8ParseIntraModeRow]" in line:
            if i > 100:
                min_line_index = i
                min_count = 0
                for j in range(max(0, i - 20), i-4):
                    if "[VP8ParseIntraModeRow]" in log_lines[j]:
                        min_line_index = min(min_line_index, j)  

                if min_line_index < i:
                    if flag_repeat != 1:
                        i = min_line_index
                        continue  
            
            print(f"[VP8ParseIntraModeRow] found at line {i+1}")
            A = i + 2  # A is set to 2 lines below
            is_vp8_mode = True  # Enable VP8 mode

            line2 = log_lines[i+2]
            if "[VP8ParseIntraModeRow]" in line2:
                i += 2
                continue

            line5 = log_lines[i+5]
            if "[VP8ParseIntraModeRow]" in line5:
                flag2 = 0
                j = i + 7
                while j < min(i + 5000, len(log_lines) - 2):
                    if "[VP8ParseIntraModeRow]" in log_lines[j] and "[VP8ParseIntraModeRow]" in log_lines[j + 2]:
                        if "[VP8ParseIntraModeRow]" in log_lines[j + 4]:
                            print(f"[VP8ParseIntraModeRow] found at line {j+1} and again at line {j+3}")
                            j += 4  
                            flag_repeat = 1
                            continue  
        
                        print(f"[VP8ParseIntraModeRow] found at line {j+1} and again at line {j+3}")
                        i = j + 2
                        flag2 = 1
                        break

                    else:
                        j += 1


                if flag2 == 1:
                    continue

                i += 11
         
                print(f"[VP8ParseIntraModeRow] found at line {i+1} and again at line {i+3} and again")
                continue

            # Record [woomin_y], [woomin_x], and [woomin_yac] logs
            if A < len(log_lines):
                filtered_logs.append("[woomin_y]\n")  # Record [woomin_y]
                filtered_logs.append("[woomin_x]\n")  # Record [woomin_x]
                filtered_logs.append("[woomin_yac]\n")  # Record [woomin_yac]
                mb_count = 0  # Reset counter after [woomin_yac]
                last_woomin_x = A  # Update last position of [woomin_x]

        # If [GetCoeffsFast] is found while VP8 mode is active, set A
        if is_vp8_mode and "[GetCoeffsFast]" in line:
            A = i  # A is set to the current line
            flag_repeat = 0

            # Prevent duplicate recording of [woomin_x]
            if i != last_woomin_x:  # Record only if different from last position
                filtered_logs.append("[woomin_x]\n")  # Record [woomin_x]
                filtered_logs.append("[woomin_yac]\n")  # Record [woomin_yac]
                mb_count = 0  # Reset counter after [woomin_yac]
                last_woomin_x = i  # Update last position of [woomin_x]

        # Check A + 48 and A + 50 in sequence
        if is_vp8_mode and A > 0:

            indices_to_check_newline = range(A + 2, A + 49, 2) # new line error solve woomin
            count_decode_log = 0
            flag = 0
            for idx in indices_to_check_newline:
                if idx < len(log_lines):
                    if "[VP8ParseIntraModeRow]" in log_lines[idx]:
                        collect_vp8decode(A, idx)
                        i = idx  # Move pointer to next y
                        flag = 1
                    count_decode_log += 1

            if flag == 1:
                continue

            target_index_48 = A + 48
            target_index_50 = A + 50

            # Function to collect VP8DecodeMB and record woomin_uvac
            def collect_vp8decode(start, end):
                nonlocal mb_count
                if (end - start) > 47: 
                    for j in range(start, end + 1):
                        if j >= len(log_lines):
                            print(f"Index out of range: j={j}, len(log_lines)={len(log_lines)}")
                            return filtered_logs                    
                        
                        if "[VP8DecodeMB]" in log_lines[j]:
                            filtered_logs.append(log_lines[j])
                            mb_count += 1
                            # Record [woomin_uvac] after every 16 [VP8DecodeMB]
                            if mb_count == 16:
                                filtered_logs.append("[woomin_uvac]\n")
                                mb_count = 0  # Reset counter
                else:
                    for j in range(start, end + 1):
                        if j >= len(log_lines):
                            print(f"Index out of range: j={j}, len(log_lines)={len(log_lines)}")
                            return filtered_logs       
                        
                        if "[VP8DecodeMB]" in log_lines[j]:
                            filtered_logs.append(log_lines[j])
                            mb_count += 1
                            # Record [woomin_uvac] after every 16 [VP8DecodeMB]
                            if mb_count == 16:
                                filtered_logs.append("[woomin_uvac]\n")
                    while mb_count < 25:
                        if mb_count == 16:
                            filtered_logs.append("[woomin_uvac]\n")
                        filtered_logs.append("[  PARSE LOG] [VP8DecodeMB] GPN [Padding] time diff : 0\n")
                        mb_count += 1
                    mb_count = 0  # Reset counter

            # 1. Check if [VP8ParseIntraModeRow] is at A+48
            if target_index_48 < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_48]:
                collect_vp8decode(A, target_index_48)
                i = target_index_48  # Move pointer to A+48 line
                continue

            # 2. Check if [VP8ParseIntraModeRow] is at A+50
            if target_index_50 < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_50]:
                collect_vp8decode(A+2, target_index_50)
                i = target_index_50  # Move pointer to A+50 line
                continue

            # 3. Check if number at A+48 exceeds THRESHOLD
            if target_index_48 < len(log_lines):
                number_48 = get_last_number(log_lines[target_index_48])
    
                if target_index_50 < len(log_lines):
                    number_50 = get_last_number(log_lines[target_index_50])

                else:
                    return filtered_logs

                if number_48 is None:
                    print(f"number_48 is None at index {target_index_48}. Returning filtered_logs.")
                    return filtered_logs

                if number_50 is None:
                    collect_vp8decode(A, target_index_48)
                    return filtered_logs

                if number_48 > THRESHOLD:
                    # First check if A+50 also exceeds THRESHOLD
                    if number_50 < THRESHOLD:
                        collect_vp8decode(A, target_index_48)
                        i = target_index_48  # Move to the next line
                        continue                    

                    if target_index_50 < len(log_lines):
                        number_50 = get_last_number(log_lines[target_index_50])
                        if number_50 > THRESHOLD and number_50 < MAX:
                            if number_48 > MAX:
                                collect_vp8decode(A + 2, target_index_50)
                                i = target_index_50  # Move to the next line
                                continue                            


                            target_index_48_48 = A + 48 + 48
                            target_index_middle = A + 48 + 50
                            target_index_50_50 = A + 50 + 50

                            if target_index_48_48 < len(log_lines):
                                number_48_48 = get_last_number(log_lines[target_index_48_48])


                            if target_index_middle < len(log_lines):
                                number_middle = get_last_number(log_lines[target_index_middle])

                            if target_index_50_50 < len(log_lines):
                                number_50_50 = get_last_number(log_lines[target_index_50_50])

                            if target_index_48_48 < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_48_48]:
                                collect_vp8decode(A, target_index_48)
                                i = target_index_48  # Move pointer to A+48 line
                                continue

                            if target_index_middle < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_middle]:
                                collect_vp8decode(A, target_index_48)
                                i = target_index_48  # Move to the next line
                                continue

                            if target_index_50_50 < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_50_50]:
                                collect_vp8decode(A+2, target_index_50)
                                i = target_index_50  # Move pointer to A+48 line
                                continue

                            if number_48_48 > THRESHOLD and number_50_50 < THRESHOLD:
                                collect_vp8decode(A, target_index_48)
                                i = target_index_48
                                continue

                            if number_48_48 < THRESHOLD and number_50_50 > THRESHOLD:
                                collect_vp8decode(A + 2, target_index_50)
                                i = target_index_50
                                continue

                            if number_middle > THRESHOLD and number_middle < MAX:
                                    collect_vp8decode(A, target_index_48)
                                    i = target_index_48  # Move to the next line
                                    continue
                            
                        else:
                            if number_48 > MAX and number_50 > MAX:
                                target_index_48_48 = A + 48 + 48
                                target_index_middle = A + 48 + 50
                                target_index_50_50 = A + 50 + 50

                                if target_index_48_48 < len(log_lines):                                
                                    number_48_48 = get_last_number(log_lines[target_index_48_48])


                                if target_index_middle < len(log_lines):
                                    number_middle = get_last_number(log_lines[target_index_middle])

                                if target_index_50_50 < len(log_lines):
                                    number_50_50 = get_last_number(log_lines[target_index_50_50])

                                if target_index_48_48 < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_48_48]:
                                    collect_vp8decode(A, target_index_48)
                                    i = target_index_48  # Move pointer to A+48 line

                                    continue                                

                                if target_index_middle < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_middle]:                     
                                    indices_to_check = range(A + 3, A + 32, 2)
                                    count_exceeding_threshold = 0
                                    for idx in indices_to_check:
                                        if idx < len(log_lines):
                                            number_test = get_last_number(log_lines[idx])
                                            if number_test > THRESHOLD2 and number_test < MAX:
                                                count_exceeding_threshold += 1
                                    if count_exceeding_threshold > COUNT:
                                        collect_vp8decode(A, target_index_48)
                                        i = target_index_48  # Move to the next line
                                        continue
                                    else:
                                        collect_vp8decode(A + 2, target_index_50)
                                        i = target_index_50  # Move to the next line
                                        
                                        continue

                                if target_index_50_50 < len(log_lines) and "[VP8ParseIntraModeRow]" in log_lines[target_index_50_50]:                                    
                                    collect_vp8decode(A+2, target_index_50)
                                    i = target_index_50  # Move pointer to A+48 line
                                    
                                    continue 

                                if number_48_48 > THRESHOLD and number_50_50 < THRESHOLD:
                                    collect_vp8decode(A, target_index_48)
                                    i = target_index_48
                                 
                                    continue

                                if number_48_48 < THRESHOLD and number_50_50 > THRESHOLD:
                                    collect_vp8decode(A + 2, target_index_50)
                                    i = target_index_50
                                    continue

                                if number_middle > THRESHOLD and number_middle < MAX:
                                    indices_to_check = range(A + 3, A + 32, 2)
                                    count_exceeding_threshold = 0
                                    for idx in indices_to_check:
                                        if idx < len(log_lines):
                                            number_test = get_last_number(log_lines[idx])
                                            if number_test > THRESHOLD2 and number_test < MAX:
                                                count_exceeding_threshold += 1
                                    if count_exceeding_threshold > COUNT:
                                        collect_vp8decode(A, target_index_48)
                                        i = target_index_48  # Move to the next line
                                        continue
                                    else:
                                        collect_vp8decode(A + 2, target_index_50)
                                        i = target_index_50  # Move to the next line
                                        continue

                            collect_vp8decode(A, target_index_48)
                            i = target_index_48
                            continue

            # 4. Check if number at A+50 exceeds THRESHOLD
            if target_index_50 < len(log_lines):
                number_50 = get_last_number(log_lines[target_index_50])
                if number_50 > THRESHOLD and number_50 < MAX:                
                    collect_vp8decode(A+2, target_index_50)
                    i = target_index_50  # Move to the next line
                    continue

            if target_index_50 < len(log_lines):
                number_50 = get_last_number(log_lines[target_index_50])
                if number_50 > THRESHOLD and number_48 < THRESHOLD:        
                    collect_vp8decode(A+2, target_index_50)
                    i = target_index_50  # Move to the next line
                    continue                

            if number_48 < THRESHOLD and number_50 < THRESHOLD:
                collect_vp8decode(A, target_index_48)
                i = target_index_48  # Move to the next line
                continue

            indices_to_check = range(A + 3, A + 32, 2)
            count_exceeding_threshold = 0
            for idx in indices_to_check:
                if idx < len(log_lines):
                    number_test = get_last_number(log_lines[idx])
                    if number_test > THRESHOLD2 and number_test < MAX:
                        count_exceeding_threshold += 1
            if count_exceeding_threshold > COUNT:
                collect_vp8decode(A, target_index_48)
                i = target_index_48  # Move to the next line
                continue
            else:
                collect_vp8decode(A + 2, target_index_50)
                i = target_index_50  # Move to the next line
                continue



        # Count VP8DecodeMB only if it's already recorded in the filtered logs
        if "[VP8DecodeMB]" in line and line in filtered_logs:
            mb_count += 1
            # Record [woomin_uvac] after every 16 [VP8DecodeMB]
            if mb_count == 16:
                filtered_logs.append("[woomin_uvac]\n")
                mb_count = 0  # Reset counter

        # Disable VP8 mode when next [VP8ParseIntraModeRow] is found
        if "[VP8ParseIntraModeRow]" in line:
            is_vp8_mode = False  # Disable VP8 mode

        # Move to the next line
        i += 1

    return filtered_logs

if len(sys.argv) != 2:
    print("Usage: python3 test.py <input_filename>")
    sys.exit(1)

input_filename = sys.argv[1]

# Read log file
with open(input_filename, "r", encoding="utf-8") as file:
    log_lines = file.readlines()

# Exclude lines from 1 to 18 
if len(log_lines) > 18:
#    log_lines = log_lines[18:]
    log_lines = log_lines[0:]
else:
    log_lines = []

# Perform filtering
filtered_logs = filter_logs(log_lines)

# Remove the last 2 lines
if len(filtered_logs) >= 2:
    filtered_logs = filtered_logs[:-1]

# Count x for each y and print
y_count = 0
x_count = 0
for line in filtered_logs:
    if "[woomin_y]" in line:
        if y_count > 0:  # If not the first y
            print(f"[y = {y_count}]: x : {x_count}")
        y_count += 1
        x_count = 0  # Reset x count for new y
    elif "[woomin_x]" in line:
        x_count += 1

# Print last y count
if y_count > 0:
    print(f"[y : {y_count}]: x : {x_count}")

# Output filtered logs (without x count) to file
with open("output.txt", "w", encoding="utf-8") as file:
    file.writelines(filtered_logs)

print("Filtering complete! Results are saved in output.txt.")


