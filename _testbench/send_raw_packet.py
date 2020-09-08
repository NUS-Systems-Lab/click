import socket

sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW) # Raw Ethernet Frame with MAC Header
sock.bind(("h1-eth0", 0))

src_addr = "\x66\xd3\x26\xf7\x8d\xd8"
dst_addr = "\xb2\xd6\xc8\x57\x26\x04"
# payload = ("["*30)+"PAYLOAD"+("]"*30)
# checksum = "\x1a\x2b\x3c\x4d"
ethertype = "\x08\x00"

ippkt = "\x45\x00"

payload = ippkt

# sock.send(dst_addr+src_addr+ethertype+payload+checksum)
sock.send(dst_addr+src_addr+ethertype+payload)