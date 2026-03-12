import struct


HEADER = 0xAA
PACKET_TYPE_CONTROL = 0x01


class ControlPacket:

    def __init__(self, steer, throttle, brake, buttons):

        self.steer = steer
        self.throttle = throttle
        self.brake = brake
        self.buttons = buttons

    def encode(self):

        steer_i = int(self.steer * 1000)
        throttle_i = int(self.throttle * 1000)
        brake_i = int(self.brake * 1000)

        packet = struct.pack(
            "<BBhhhB",
            HEADER,
            PACKET_TYPE_CONTROL,
            steer_i,
            throttle_i,
            brake_i,
            self.buttons
        )

        checksum = sum(packet) % 256

        packet += struct.pack("B", checksum)

        return packet