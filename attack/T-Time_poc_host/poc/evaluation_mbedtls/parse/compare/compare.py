import time
import sys

# ANSI color codes
GREEN = "\033[92m"
RED = "\033[91m"
BLUE = "\033[94m"
RESET = "\033[0m"

def compare_files_char_by_char(file1, file2):
    try:
        with open(file1, 'r') as f1, open(file2, 'r') as f2:
            content1 = f1.read().rstrip('\n')
            content2 = f2.read().rstrip('\n')

            len1 = len(content1)
            len2 = len(content2)
            min_len = min(len1, len2)

            print("Comparing files character by character...\n")
            time.sleep(0.5)

            for i in range(min_len):
                if content1[i] == content2[i]:
                    print(f"{GREEN}{content1[i]}{RESET}", end='', flush=True)
                else:
                    print(f"{RED}Mismatch at position {i}: '{content1[i]}' (result.txt) vs '{content2[i]}' (answer.txt){RESET}", flush=True)
                    print(f"{RED}Stopping comparison due to mismatch.{RESET}")
                    sys.exit(1)
                time.sleep(0.0005)

            if len1 != len2:
                print(f"{RED}Files have different lengths: result.txt ({len1}) vs answer.txt ({len2}){RESET}")
                sys.exit(1)

            print(f"\n{BLUE}All secret bits match perfectly.{RESET}")

    except FileNotFoundError as e:
        print(f"{RED}Error: {e}{RESET}")

if __name__ == "__main__":
    compare_files_char_by_char("./compare/result.txt", "./compare/answer.txt")

