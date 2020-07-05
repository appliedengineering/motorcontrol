import os
import platform
import struct
import time
from pySerialTransfer import pySerialTransfer as txfer

def clear():
    os.system('cls' if os.name=='nt' else 'clear')

if __name__ == '__main__':
    try:
        clear()

        if platform.system() == 'Linux':
            link = txfer.SerialTransfer('/dev/ttyACM0', 115200)
        elif platform.system() == 'Darwin':
            link = txfer.SerialTransfer('/dev/cu.usbmodem14101', 115200)
        else:
            link = txfer.SerialTransfer('COM4', 115200)

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
                telemetryData = struct.unpack('<12shhhhff????', binaryData)
                #clear()
                print(telemetryData)

    except KeyboardInterrupt:
        link.close()

    except:
        import traceback
        traceback.print_exc()

        link.close()

