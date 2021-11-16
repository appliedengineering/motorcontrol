# Telemetry ZeroMQ Transmitter
# Copyright (c) 2020 Applied Engineering

import concurrent.futures
import logging
import msgpack
import queue
import serial
import serial.tools.list_ports
import threading
import traceback
import zmq
import time

# Set logging verbosity.
# CRITICAL will not log anything.
# ERROR will only log exceptions.
# INFO will log more information.
log_level = logging.INFO

# ZeroMQ Context.
context = zmq.Context.instance()

# Define the socket using the Context.
pub = context.socket(zmq.PUB)
pub.bind("tcp://*:5556")

# Define message end sequence.
end = b'EOM\n'

def addTimestampToStruct(data):
    buffer = msgpack.unpackb(data)
    buffer["timeStamp"] = round(time.time(), 3)
    # NOTE: timeStamp is a 64 bit Float or Double NOT a 32 bit float as is the case with the other data
    return msgpack.packb(buffer)

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
            queue.put(addTimestampToStruct(link.read_until(end).rstrip(end)))
            logging.info('Producer received data.')
        except:
            traceback.print_exc()
            exit_event.set()
    logging.info('Producer received event. Exiting now.')
    link.close()

def broadcastDataZmq(queue, exit_event):
    '''Broadcast data with ZeroMQ.'''
    while not exit_event.is_set() or not queue.empty():
        try:
            # queue.get(True, 2) blocks with a 2 second timeout
            # If still empty after 2 seconds, throws Queue.Empty
            data = queue.get(True, 2)
            pub.send(data)
            logging.info('Consumer sending data. Queue size is %d.', queue.qsize())
        except Queue.Empty:
            pass    # no message ready yet
        except:
            traceback.print_exc()
            exit_event.set()
    logging.info('Consumer received event. Exiting now.')
    pub.close()

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")
        # Detect Arduino port and open serial connection
        link = serial.Serial(findArduinoPort(), 500000)
        # Throw away first and second reading
        _ = link.read_until(end).rstrip(end)
        _2 = link.read_until(end).rstrip(end)
        # Set up data queue
        pipeline = queue.Queue(maxsize=100)
        # Create exit event
        exit_event = threading.Event()
        # Spawn worker threads
        with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
            executor.submit(readFromArduino, pipeline, exit_event)
            executor.submit(broadcastDataZmq, pipeline, exit_event)
    except KeyboardInterrupt:
        logging.info('Setting exit event.')
        exit_event.set()
    except:
        traceback.print_exc()
        exit_event.set()
