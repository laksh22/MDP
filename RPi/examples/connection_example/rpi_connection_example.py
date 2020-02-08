import socket

TCP_IP = '192.168.101.1'
TCP_PORT = 4042
BUFFER_SIZE = 1024

if __name__ == "__main__":
    # A message that will be sent to the tcp connection port
    tcp_message = str.encode("@txxx_tcp_msg_xxx!")

    # A message that will be sent to the bluetooth connection port
    bt_message = str.encode("@bxxx_bluetooth_msg_xxx!")

    # A message that will be sent to the serial connection port
    serial_message = str.encode("@sxxx_serial_msg_xxx!")

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    s.send(tcp_message)
    data = s.recv(BUFFER_SIZE)
    print ("Received data:", data)

    s.send(bt_message)
    s.send(serial_message)
    s.close()
