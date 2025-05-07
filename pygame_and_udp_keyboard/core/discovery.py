import socket
import threading

paired_ips = set()  # <- Armazena os IPs que já foram pareados

def start_discovery_server(port=4211):
    def run_server():
        UDP_PORT = port
        RESPONSE = b'esp32-discovery'

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(('', UDP_PORT))

        print(f"Discovery Server ativo na porta {UDP_PORT}...")

        while True:
            try:
                data, addr = sock.recvfrom(1024)
                ip = addr[0]

                if data.strip() == b"who-is-pc":
                    if ip not in paired_ips:
                        sock.sendto(RESPONSE, addr)
                        paired_ips.add(ip)
                        print(f"Enviado '{RESPONSE.decode()}' para {ip}")
                    else:
                        print(f"Ignorado: {ip} já pareado")
                        sock.sendto(RESPONSE, addr)
                else:
                    print(f"Ignorado (mensagem não reconhecida): {data}")

            except Exception as e:
                print(f"[ERRO] Discovery server: {e}")

    thread = threading.Thread(target=run_server, daemon=True)
    thread.start()
