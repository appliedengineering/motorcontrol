# Convert raw Telemetry data to a Google-Sheet ready form
# Copyright (c) 2021 Applied Engineering

import logging
import sys
import json

# Set logging verbosity.
# CRITICAL will not log anything.
# ERROR will only log exceptions.
# INFO will log more information.
log_level = logging.INFO

# Define message end sequence.
end = b'EOM\n'

# Set up logging format
logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")

# Open log file for reading only
log_file = open('log.txt', 'r')
log_file_lines = log_file.readlines()

# Create / Write std output to output file
sys.stdout = open('parsed_log.csv', 'w')

# Output helper functions
def output(s):
    print(s, end=" ")

def output_newline():
    print()

# Format the txt file to be json ready
def formatLine(s):
    s = s.replace('\'', '\"') # Replace single quotes with double quotes
    s = s.replace('True', 'true') # Uncaptialize True/False
    s = s.replace('False', 'false')
    return s

def printHeader(arr):
    arr_len = len(arr)
    i = 0
    for key in arr:
        output(key + (',' if i + 1 < arr_len else ''))
        i += 1
    output_newline()


hasOutputHeader = False
line_count = 0

for line in log_file_lines:
    data = line.strip() # Strips the newline character
    j = json.loads(formatLine(data))
    j_keys = j.keys()

    # print header
    if not hasOutputHeader:
        printHeader(j_keys)  
        hasOutputHeader = True
    
    key_len = len(j_keys)
    i = 0
    for key in j_keys:
        output(str(j[key]) + (',' if i + 1 < key_len else ''))
        i += 1
    output_newline()
    line_count += 1

logging.info("Finished parsing %i lines", line_count)
