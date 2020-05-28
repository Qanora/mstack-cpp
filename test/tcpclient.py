import socket

# client
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect(('192.168.1.1', 30000))
print("connect successful")
buf = client.recv(1000)
print(buf)
while True:
    sendbuf = input()
    client.send(sendbuf.encode('utf-8'))
client.close()
print('Connection was closed...')