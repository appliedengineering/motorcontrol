import struct
import zmq

# ZeroMQ Context
context = zmq.Context()
# Define the socket using the "Context"
sock = context.socket(zmq.SUB)
# Define subscription and messages with prefix to accept.
sock.setsockopt(zmq.SUBSCRIBE, b"")
sock.connect("epgm://224.0.0.1:28650")

if __name__ == "__main__":
    try:
        # Let's send data through UDP protocol
        while True:
            # send_data = input("Type some text to send =>")
            # s.sendto(send_data.encode('utf-8'), (ip, port))
            # print("\n\n 1. Client Sent : ", send_data, "\n\n")
            # print("recieve")
            binaryData = sock.recv()
            # print(binaryData.decode('utf-8'))
            telemetryData = struct.unpack('<12shhhhfffff???', binaryData)
            print(telemetryData)

    except KeyboardInterrupt:
        # close the socket
        sock.close()

    except:
        import traceback
        traceback.print_exc()
        sock.close()
