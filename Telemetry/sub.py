# Telemetry ZeroMQ Subscriber
# Copyright (c) 2020 Applied Engineering

import logging
import msgpack
import zmq

# Set logging verbosity.
# CRITICAL will not log anything.
# ERROR will only log exceptions.
# INFO will log more information.
log_level = logging.INFO

# ZeroMQ Context.
context = zmq.Context()
# Define the socket using the Context.
sock = context.socket(zmq.SUB)
# Define subscription.
sock.setsockopt(zmq.SUBSCRIBE, b'')
sock.connect('epgm://224.0.0.1:28650')

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")
        
        while True:
            print(msgpack.unpackb(sock.recv(), use_list=False, raw=False))

    except KeyboardInterrupt:
        logging.info('Exiting now.')
        sock.close()

    except Exception as e:
        logging.error('A %s error occurred.', e.__class__)