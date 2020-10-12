import platform
import struct
import time
import zmq

from pySerialTransfer import pySerialTransfer as txfer

# ZeroMQ Context
context = zmq.Context()
# Define the socket using the "Context"
sock = context.socket(zmq.PUB)
sock.bind("epgm://224.0.0.1:28650")

if __name__ == "__main__":
    try:
        if platform.system() == "Linux":
            link = txfer.SerialTransfer("/dev/ttyACM0", 115200)
        elif platform.system() == "Darwin":
            link = txfer.SerialTransfer("/dev/tty.usbmodem14B01", 115200, False)
        else:
            link = txfer.SerialTransfer("COM1", 115200)

        if link.open():
            time.sleep(0)

            while True:
                while not link.available():
                    if link.status < 0:
                        if link.status == txfer.CRC_ERROR:
                            print('ERROR: CRC_ERROR')
                        elif link.status == txfer.PAYLOAD_ERROR:
                            print('ERROR: PAYLOAD_ERROR')
                        elif link.status == txfer.STOP_BYTE_ERROR:
                            print('ERROR: STOP_BYTE_ERROR')
                        else:
                            print('ERROR: {}'.format(link.status))

                binaryData = bytearray(link.rxBuff[:link.bytesRead])
                
                # Uncomment below to deserialize (read) and print the data.
                telemetryData = struct.unpack('<12shhhhfffff???', binaryData)
                print(telemetryData)

                ### EPGM MULTICAST BINARY DATA ###
                sock.send(binaryData)                                   # send the data to client
                # print("\n\n 1. Server sent ", len(send_data),"\n\n")  # print length of data sent

    except KeyboardInterrupt:
        link.close()

    except:
        import traceback
        traceback.print_exc()
        link.close()
