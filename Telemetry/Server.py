import os
import platform
import socket
import struct
import sys
import time
from pySerialTransfer import pySerialTransfer as txfer

if len(sys.argv) == 3:
    # Get "IP address of Server" and also the "port number" from argument 1 and argument 2
    ip = sys.argv[1]
    port = int(sys.argv[2])
else:
    print("Run like : python3 server.py <arg1:server ip:this system IP 192.168.1.6> <arg2:server port:4444 >")
    exit(1)

# Creates a UDP socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
print("Do Ctrl+C to exit the program.")

def clear():
    os.system('cls' if os.name=='nt' else 'clear')

if __name__ == '__main__':
    try:
        if platform.system() == 'Linux':
            link = txfer.SerialTransfer('/dev/ttyACM0', 115200)
        elif platform.system() == 'Darwin':
            link = txfer.SerialTransfer('/dev/cu.usbmodem14101', 115200, False)
        else:
            link = txfer.SerialTransfer('COM3', 115200)

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

                rawData = link.rxBuff[:link.bytesRead]
                #print(rawData)
                binaryData = bytearray(rawData)
                telemetryData = struct.unpack('<12shhhhfffff???', binaryData)
                # print(telemetryData)
                
                # UDP MULTICAST BINARY DATA
                # print("####### Server is listening #######")
                # data, address = s.recvfrom(4096)                                      # receives the data from client
                # print("\n\n 2. Server received: ", data.decode('utf-8'), "\n\n")      # prints the data received (not sure if you want it printed or how you want it displayed, feel free to edit)
                send_data = binaryData                                                  # receives input and stores in send_data var
                s.sendto(send_data, (ip, port))                                     # sends the data to client
                # print("\n\n 1. Server sent ", len(send_data),"\n\n")                  # prints the data that was sent

    except KeyboardInterrupt:
        link.close()

    except:
        import traceback
        traceback.print_exc()

        link.close()