#!/bin/sh
python3 ./restore/pattern_first.py
python3 ./restore/pattern_second.py
python3 ./restore/pattern_third.py

mv result.txt compare/

python3 ./compare/compare.py

