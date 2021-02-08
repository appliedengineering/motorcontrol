# Print Telemetry to Console
# Copyright (c) 2021 Applied Engineering

import concurrent.futures
import logging
import msgpack
import queue
import serial
import serial.tools.list_ports
import threading
import sys

# Set logging verbosity.
# CRITICAL will not log anything.
# ERROR will only log exceptions.
# INFO will log more information.
log_level = logging.INFO

# Define message end sequence.
end = b'EOM\n'

# Open / Create log file
sys.stdout = open('log.txt', 'w')

def findArduinoPort():
    '''Locate Arduino serial port.'''

    arduino_ports = [
        p.device
        for p in serial.tools.list_ports.comports()
        if 'Arduino' in p.description or 'Generic CDC' in p.description or 'ttyACM0' in p.description
    ]
    if not arduino_ports:
        logging.error('No Arduino found.')
    if len(arduino_ports) > 1:
        logging.info('Multiple Arduinos found. Connecting to the first one.')
    
    logging.info('Found an Arduino at %s.', arduino_ports[0])
    return arduino_ports[0]

def readFromArduino(queue, exit_event):
    '''Read data from serial.'''
    
    while not exit_event.is_set():
        try:
            queue.put(link.read_until(end).rstrip(end))
            logging.info('Producer received data.')
        
        except Exception as e:
            logging.error('A %s error occurred.', e.__class__)
    
    logging.info('Producer received event. Exiting now.')
    link.close()

def outputData(queue, exit_event):
    '''Print data to console or txt file.'''
    while not exit_event.is_set() or not queue.empty():
        try:
            data = msgpack.unpackb(queue.get(), use_list=False, raw=False) 
            print(data)
            logging.info(data)
            logging.info('Consumer printed data. Queue size is %d.', queue.qsize())

        except Exception as e:
            logging.error('A %s error occurred.', e.__class__)
    
    logging.info('Consumer received event. Exiting now.')

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")

        # Detect Arduino port and open serial connection
        link = serial.Serial(findArduinoPort(), 500000)

        # Throw away first reading
        _ = link.read_until(end).rstrip(end)

        pipeline = queue.Queue(maxsize=100)
        exit_event = threading.Event()
        with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
            executor.submit(readFromArduino, pipeline, exit_event)
            executor.submit(outputData, pipeline, exit_event)
    
    except KeyboardInterrupt:
        logging.info('Setting exit event.')
        exit_event.set()

    except Exception as e:
        logging.error('A %s error occurred.', e.__class__)
        exit_event.set()