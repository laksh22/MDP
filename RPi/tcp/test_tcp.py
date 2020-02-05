import socket

TCP_IP = '192.168.101.1'
TCP_PORT = 4042
BUFFER_SIZE = 1024
MESSAGE = str.encode("txxxxxxxxx")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
data = s.recv(BUFFER_SIZE)
s.close()

print ("received data:", data)