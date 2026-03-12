import pygame

from input.wheel_input import WheelInput
from gui.dashboard import Dashboard
from control.control_loop import ControlLoop


ESP32_IP = "192.168.1.100"
ESP32_PORT = 3333

ENABLE_NETWORK = False


def main():

    pygame.init()

    wheel = WheelInput()
    gui = Dashboard()
    
    print(f"EnabbleNetwork: {ENABLE_NETWORK}")

    control = ControlLoop(
        wheel,
        gui,
        enable_network=ENABLE_NETWORK,
        host=ESP32_IP,
        port=ESP32_PORT
    )

    clock = pygame.time.Clock()

    running = True

    while running:

        for event in pygame.event.get():

            if event.type == pygame.QUIT:
                running = False

            wheel.process_event(event)

        pygame.event.pump()

        control.update()

        gui.draw()

        clock.tick(50)

    control.shutdown()

    pygame.quit()


if __name__ == "__main__":
    main()