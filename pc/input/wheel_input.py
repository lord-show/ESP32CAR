import pygame


class WheelInput:

    def __init__(self):

        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            raise RuntimeError("No joystick detected")

        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()

        print("Controller:", self.joystick.get_name())

    def process_event(self, event):
        pass

    def read(self):

        steer = self.joystick.get_axis(0)

        throttle = (self.joystick.get_axis(5) + 1) / 2
        brake = (self.joystick.get_axis(2) + 1) / 2

        buttons = 0

        if self.joystick.get_button(0):
            buttons |= 1 << 0

        if self.joystick.get_button(1):
            buttons |= 1 << 1

        if self.joystick.get_button(2):
            buttons |= 1 << 2

        if self.joystick.get_button(3):
            buttons |= 1 << 3

        return {
            "steer": steer,
            "throttle": throttle,
            "brake": brake,
            "buttons": buttons
        }