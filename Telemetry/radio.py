# Telemetry ZeroMQ Publisher
# Copyright (c) 2020 Applied Engineering

import concurrent.futures
import logging
import msgpack
import platform
import queue
import serial
import threading
import traceback
import zmq

# Set logging verbosity.
# CRITICAL will not log anything.
# ERROR will only log exceptions.
# INFO will log more information.
log_level = logging.INFO

# ZeroMQ Context.
context = zmq.Context.instance()
# Define the socket using the Context.
radio = context.socket(zmq.RADIO)
radio.connect('udp://224.0.0.1:28650')

# Define message end sequence.
end = b'EOM\n'

def readFromArduino(queue, exit_event):
    '''Read data from serial.'''
    while not exit_event.is_set():
        try:
            queue.put(link.read_until(end).rstrip(end))
            logging.info('Producer received data.')
        except:
            traceback.print_exc()
            exit_event.set()
    logging.info('Producer received event. Exiting now.')
    link.close()

def sendZmqMulticast(queue, exit_event):
    '''Multicast data with ZeroMQ.'''
    while not exit_event.is_set() or not queue.empty():
        try:
            # queue.get(True, 2) blocks with a 2 second timeout
            # If still empty after 2 seconds, throws Queue.Empty
            radio.send(queue.get(True, 2), group='telemetry')
            logging.info('Consumer sending data. Queue size is %d.', queue.qsize())
        except Queue.Empty:
            pass    # no message ready yet
        except:
            traceback.print_exc()
            exit_event.set()
    logging.info('Consumer received event. Exiting now.')
    radio.close()

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")
        # Open serial connection
        if platform.system() == 'Darwin':
            link = serial.Serial('/dev/tty.usbmodem14101', 500000)
        elif platform.system() == 'Linux':
            link = serial.Serial('/dev/ttyACM0', 500000)
        else:
            link = serial.Serial('COM9', 500000)
        # Throw away first reading
        _ = link.read_until(end).rstrip(end)
        # Set up data queue
        pipeline = queue.Queue(maxsize=100)
        # Create exit event
        exit_event = threading.Event()
        # Spawn worker threads
        with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
            executor.submit(readFromArduino, pipeline, exit_event)
            executor.submit(sendZmqMulticast, pipeline, exit_event)
    except KeyboardInterrupt:
        logging.info('Setting exit event.')
        exit_event.set()
    except:
        traceback.print_exc()
        exit_event.set()