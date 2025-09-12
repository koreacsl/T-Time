import re
import numpy as np
import cv2

MAX_BLOCKS = 10000000
BLOCK_SIZE = 16 
SUB_BLOCK_SIZE = 4
THRESHOLD_Y = 20103
THRESHOLD_UV = 20403


def parse_log_file(filename):
    macroblocks = {}
    current_y = -1  
    current_x = 0  
    current_y_ac_values = []
    current_u_ac_values = []
    current_v_ac_values = []
    parsing_yac = False
    parsing_uvac = False
    block_count = 0

    with open(filename, 'r') as file:
        for line in file:
            match_y = re.search(r"\[woomin_y\] GPN \[0x[0-9a-f]+\] time diff : (\d+)", line)
            if match_y:
                current_y += 1
                current_x = 0
                continue

            match_x = re.search(r"\[woomin_x\] GPN \[0x[0-9a-f]+\] time diff : (\d+)", line)
            if match_x:
                continue

            if "[woomin_y]" in line:
                current_y += 1
                current_x = 0
                continue

            if "[woomin_x]" in line:
                continue

            if "[woomin_yac]" in line:
                parsing_yac = True
                parsing_uvac = False
                current_y_ac_values = []
                continue

            if "[woomin_uvac]" in line:
                parsing_yac = False
                parsing_uvac = True
                current_u_ac_values = []
                current_v_ac_values = []
                continue

            if parsing_yac and len(current_y_ac_values) < 16:
                coeff_match = re.search(r"\[VP8DecodeMB\] GPN \[0x[0-9a-f]+\] time diff : (\d+)", line)
                if coeff_match:
                    current_y_ac_values.append(int(coeff_match.group(1)))

            if parsing_uvac:
                coeff_match = re.search(r"\[VP8DecodeMB\] GPN \[0x[0-9a-f]+\] time diff : (\d+)", line)
                if coeff_match:
                    if len(current_u_ac_values) < 4:
                        current_u_ac_values.append(int(coeff_match.group(1)))
                    elif len(current_v_ac_values) < 4:
                        current_v_ac_values.append(int(coeff_match.group(1)))

            if len(current_y_ac_values) == 16 and len(current_u_ac_values) == 4 and len(current_v_ac_values) == 4:
                macroblocks[(current_x, current_y)] = {
                    "y_ac": current_y_ac_values.copy(),
                    "u_ac": current_u_ac_values.copy(),
                    "v_ac": current_v_ac_values.copy()
                }
                block_count += 1
                if block_count >= MAX_BLOCKS:
                    print(f"⚠ Too many macroblocks! Stopping at {MAX_BLOCKS} blocks.")
                    return macroblocks

                current_x += 1
                current_y_ac_values.clear()
                current_u_ac_values.clear()
                current_v_ac_values.clear()

    return macroblocks


def yuv_to_rgb(y, u, v):
    r = y + 1.402 * (v - 128)
    g = y - 0.344136 * (u - 128) - 0.714136 * (v - 128)
    b = y + 1.772 * (u - 128)

    r = np.clip(r, 0, 255)
    g = np.clip(g, 0, 255)
    b = np.clip(b, 0, 255)

    return int(r), int(g), int(b)


def generate_yuv_pixel_map(macroblocks, output_file="output_yuv_inverse.png"):
    max_x = max(mb[0] for mb in macroblocks) + 1
    max_y = max(mb[1] for mb in macroblocks) + 1
    img_width = max_x * 4
    img_height = max_y * 4

    pixel_map = np.zeros((img_height, img_width, 3), dtype=np.uint8)

    for (x, y), values in macroblocks.items():
        y_ac_values = values["y_ac"]
        u_ac_values = values["u_ac"]
        v_ac_values = values["v_ac"]

        
        u_thresholded = [95 if val > THRESHOLD_UV and val else 200 for val in u_ac_values] # u
        v_thresholded = [120 if val > THRESHOLD_UV and val else 200 for val in v_ac_values] # v

        u_expanded = np.repeat(np.repeat(np.array(u_thresholded, dtype=np.uint8).reshape(2, 2), 2, axis=0), 2, axis=1)
        v_expanded = np.repeat(np.repeat(np.array(v_thresholded, dtype=np.uint8).reshape(2, 2), 2, axis=0), 2, axis=1)

        for sub_idx, y_value in enumerate(y_ac_values):
            sub_x = sub_idx % 4
            sub_y = sub_idx // 4
            px_x = (x * 4) + sub_x
            px_y = (y * 4) + sub_y

            y_pixel = 255 if y_value <= THRESHOLD_Y else 0 # y

            u_pixel = u_expanded[sub_y, sub_x]
            v_pixel = v_expanded[sub_y, sub_x]

            r, g, b = yuv_to_rgb(y_pixel, u_pixel, v_pixel)

            pixel_map[px_y, px_x] = (b, g, r)

    cv2.imwrite(output_file, pixel_map)
    print(f"Image saved as {output_file}")


def main():
    filename = "output.txt"
    macroblocks = parse_log_file(filename)
    
    if not macroblocks:
        print(" No valid macroblocks found!")
        return
    
    generate_yuv_pixel_map(macroblocks)


if __name__ == "__main__":
    main()

