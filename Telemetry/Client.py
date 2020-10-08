import socket
import sys
import struct

if len(sys.argv) == 3:
    # Get "IP address of Server" and also the "port number" from argument 1 and argument 2
    ip = sys.argv[1]
    port = int(sys.argv[2])
else:
    print("Run like : python3 client.py <arg1 server ip 192.168.1.102> <arg2 server port 4444 >")
    exit(1)

# Create a UDP socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the port
server_address = (ip, port)
s.bind(server_address)
print("Do Ctrl+c to exit the program !!")

# Let's send data through UDP protocol
while True:
    # send_data = input("Type some text to send =>")
    # s.sendto(send_data.encode('utf-8'), (ip, port))
    # print("\n\n 1. Client Sent : ", send_data, "\n\n")
    binaryData, address = s.recvfrom(64)
    print(binaryData.decode('utf-8'))
    telemetryData = struct.unpack('<12shhhhfffff???', binaryData)
    print(telemetryData)
# close the socket
s.close()