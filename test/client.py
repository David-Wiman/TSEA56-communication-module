import socket

def client(IP, PORT):
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((IP, PORT))
		
		while (True):
			# take input and send it
			request = input("Command...") + "\n"
			request = """{ "mapGraph": { "A1": [{"K1", 5}]}}""" + "\n"
			encoded_request = request.encode('utf-8')
			s.sendall(encoded_request)
			
			# recieve data and print it
			data = s.recv(1024).decode('utf-8')
			print(data)

if __name__ == "__main__":
	while True:
		client('192.168.1.33', 1234)
