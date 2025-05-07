import socket
from .config import UDP_IP, UDP_PORT

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
sock.setblocking(False)

def receive_udp():
    try:
        data, _ = sock.recvfrom(1024)
        return data.decode("utf-8").strip().lower()
    except BlockingIOError:
        return ""

def send_udp(message):
    sock.sendto(message.encode("utf-8"), ("localhost", UDP_PORT))

