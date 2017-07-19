import socket
import binascii
import struct

source = 1
target = 5
host = '0.0.0.0'
mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
addr = (host,1234)
mysock.connect(addr)
str1 = str(source)+' '+str(target)
struct1 = 'I '+str(len(str1))+'s'
values = (0,str1)
packer = struct.Struct(struct1)
packed_data = packer.pack(*values)
mysock.send(packed_data)
data = mysock.recv(1024)
print data
mysock.close()
