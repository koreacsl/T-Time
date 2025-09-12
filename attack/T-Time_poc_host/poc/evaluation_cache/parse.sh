#!/bin/bash

python3 parse_data.py logfile_0.txt 
python3 parse_data.py logfile_1.txt
python3 plot.py logfile_0_cleaned.csv logfile_1_cleaned.csv
