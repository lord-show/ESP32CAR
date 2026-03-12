from protocol.control_packet import ControlPacket
from network.tcp_client import TCPClient


class ControlLoop:

    def __init__(self, wheel, gui, enable_network=False, host=None, port=3333):

        self.wheel = wheel
        self.gui = gui
        self.enable_network = enable_network

        self.client = None

        if self.enable_network:

            self.client = TCPClient(host, port)

            try:
                self.client.connect()
            except Exception as e:
                print("Network disabled:", e)
                self.enable_network = False

    def update(self):

        control = self.wheel.read()

        # отправка по сети
        if self.enable_network and self.client:

            packet = ControlPacket(
                control["steer"],
                control["throttle"],
                control["brake"],
                control["buttons"]
            )

            self.client.send(packet.encode())

        # обновление GUI
        self.gui.update(control)

    def shutdown(self):

        if self.client:
            self.client.close()