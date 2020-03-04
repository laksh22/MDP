import socket
import select
import sys

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ip_address = "192.168.101.1"
port = 4042
server.connect((ip_address, port))


while True:
    # Maintains a list of possible input streams
    sockets_list = [sys.stdin, server]
    read_sockets, write_sockets, error_socket = select.select(sockets_list, [],
                                                             [])

    for socks in read_sockets:
        if socks == server:
            message = socks.recv(2048)
            print("Received: %s" % message.decode())
        else:
            message = sys.stdin.readline()
            server.send(str.encode(message, encoding="utf-8"))
            sys.stdout.write("Sending: %s" % message)
            sys.stdout.flush()
server.close()
