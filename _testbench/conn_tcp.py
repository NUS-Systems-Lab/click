import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    # TCP Packet (data only)
sock.connect(("10.0.0.2", 22))
sock.close()