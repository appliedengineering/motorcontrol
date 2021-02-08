# Telemetry ZeroMQ Subscriber
# Copyright (c) 2021 Applied Engineering

import logging
import msgpack
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
sub = context.socket(zmq.SUB)

# Define connection address. 
# NOTE: 127.0.0.1 is localhost. If the transmitter.py is not running on the same computer, make sure to replace 127.0.0.1 with the ip address of the other computer.
address = 'tcp://127.0.0.1:2000'

# Establish the connection.
sub.connect(address)
sub.subscribe("")

# Set receive timeout.
sub.rcvtimeo = 1000

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")
        logging.info('Listening for data from %s.', address)
        while True:
            try:
                print(msgpack.unpackb(sub.recv(copy=False, flags=zmq.NOBLOCK), use_list=False, raw=False))
                logging.info('Received data.')
            except zmq.ZMQError as e:
                if e.errno == zmq.EAGAIN:
                    pass    # no message ready yet
                else:
                    traceback.print_exc()
    except KeyboardInterrupt:
        logging.info('Exiting now.')
        sub.close()
    except:
        traceback.print_exc()


