import socket
import time
from PyQt6.QtCore import QThread, pyqtSignal
from telemetry.forza_parser import parse_forza_packet

class UDPReceiver(QThread):
    packet = pyqtSignal(dict, float)
    status = pyqtSignal(str)
    def __init__(self, host='127.0.0.1', port=5005):
        super().__init__()
        self.host, self.port = host, port
        self.running = True
    def run(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            sock.bind((self.host, self.port))
            sock.settimeout(.2)
            self.status.emit(f'UDP dinleniyor: {self.host}:{self.port}')
            while self.running:
                try:
                    data, _ = sock.recvfrom(2048)
                    at = time.monotonic()
                except socket.timeout:
                    continue
                except OSError:
                    break
                try:
                    p = parse_forza_packet(data)
                except (ValueError, OverflowError):
                    continue
                self.packet.emit(p, at)
        except OSError as e:
            self.status.emit(f'UDP hatası: {e}')
        finally:
            sock.close()
    def stop(self):
        self.running = False
        self.wait(1000)
