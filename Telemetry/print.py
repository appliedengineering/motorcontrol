# Print Telemetry to Console
# Copyright (c) 2020 Applied Engineering

import concurrent.futures
import logging
import msgpack
import platform
import queue
import serial
import threading

# Set logging verbosity.
# CRITICAL will not log anything.
# ERROR will only log exceptions.
# INFO will log more information.
log_level = logging.INFO

def readFromArduino(queue, exit_event):
    '''Read data from serial.'''
    
    # Define message end sequence.
    end = b'\n\n'
    
    while not exit_event.is_set():
        try:
            if platform.system() == 'Darwin':
                link = serial.Serial('/dev/tty.usbmodem14101', 115200)
            elif platform.system() == 'Linux':
                link = serial.Serial('/dev/ttyACM0', 115200)
            else:
                link = serial.Serial('COM1', 115200)

            queue.put(link.read_until(end).rstrip(end))
            logging.info('Producer received data.')
        
        except Exception as e:
            logging.error('A %s error occurred.', e.__class__)
    
    logging.info('Producer received event. Exiting now.')
    link.close()

def printToConsole(queue, exit_event):
    '''Print data to console.'''
    while not exit_event.is_set() or not queue.empty():
        try:
            print(msgpack.unpackb(queue.get(), use_list=False, raw=False))
            logging.info('Consumer printed data. Queue size is %d.', queue.qsize())

        except Exception as e:
            logging.error('A %s error occurred.', e.__class__)
    
    logging.info('Consumer received event. Exiting now.')

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")

        pipeline = queue.Queue(maxsize=100)
        exit_event = threading.Event()
        with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
            executor.submit(readFromArduino, pipeline, exit_event)
            executor.submit(printToConsole, pipeline, exit_event)
    
    except KeyboardInterrupt:
        logging.info('Setting exit event.')
        exit_event.set()

    except Exception as e:
        logging.error('A %s error occurred.', e.__class__)