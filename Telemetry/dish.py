# Telemetry ZeroMQ Subscriber
# Copyright (c) 2020 Applied Engineering

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
dish = context.socket(zmq.DISH)
# Define connection address.
address = 'udp://224.0.0.1:28650'
# Define subscription.
group = 'telemetry'
# Establish the connection.
dish.bind(address)
dish.join(group)
# Set receive timeout.
dish.rcvtimeo = 1000

if __name__ == '__main__':
    try:
        logging.basicConfig(format='[%(asctime)s] %(levelname)s: %(message)s', level=log_level, datefmt="%H:%M:%S")
        logging.info('Listening for "%s" group data from %s.', group, address)
        while True:
            try:
                print(msgpack.unpackb(dish.recv(copy=False, flags=zmq.NOBLOCK), use_list=False, raw=False))
                logging.info('Received data.')
            except zmq.ZMQError as e:
                if e.errno == zmq.EAGAIN:
                    pass    # no message ready yet
                else:
                    traceback.print_exc()
    except KeyboardInterrupt:
        logging.info('Exiting now.')
        dish.close()
    except:
        traceback.print_exc()