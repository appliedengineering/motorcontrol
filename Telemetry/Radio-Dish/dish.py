# Telemetry ZeroMQ Subscriber
# Copyright (c) 2020 Applied Engineering

import logging
import msgpack
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
sock = context.socket(zmq.DISH)
# Define subscription.
# Define connection address.
address = 'udp://224.0.0.1:28650'
sock.bind(address)
sock.rcvtimeo = 1000

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")
        
        while True:
            try:
                print(msgpack.unpackb(sock.recv(flags=zmq.NOBLOCK)))
                logging.info('Received data from %s.', address)
            except zmq.Again as e:
                print('ZMQ error - ', e)
            time.sleep(1);

    except KeyboardInterrupt:
        logging.info('Exiting now.')
        sock.close()
    except:
        import traceback
        traceback.print_exc()
        #exit_event.set()
