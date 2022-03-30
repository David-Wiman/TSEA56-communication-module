import socket
import threading

HOST = '192.168.1.33'
PORT = 1234

def client():
	threading.Timer(11, client).start()
	
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((HOST, PORT))
		
		request = input("Command...") + "\n"
		encoded_request = request.encode('utf-8')
		s.sendall(encoded_request)
		
		data = s.recv(1024).decode('utf-8')
		print(data)

if __name__ == "__main__":
	while True
		client()
		
