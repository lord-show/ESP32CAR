import socket


class TCPClient:

    def __init__(self, host, port):

        self.host = host
        self.port = port
        self.sock = None

    def connect(self):

        print(f"Connecting to {self.host}:{self.port}")

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))

        print("Connected to ESP32")

    def send(self, data):

        if not self.sock:
            return

        try:
            self.sock.sendall(data)

        except Exception as e:
            print("Send error:", e)
            self.sock.close()
            self.sock = None

    def close(self):

        if self.sock:
            self.sock.close()
            self.sock = None