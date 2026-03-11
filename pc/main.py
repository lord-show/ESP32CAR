import pygame
from input.wheel_input import WheelInput
from gui.dashboard import Dashboard


def main():

    pygame.init()

    wheel = WheelInput()
    gui = Dashboard()

    clock = pygame.time.Clock()
    running = True

    while running:

        for event in pygame.event.get():

            if event.type == pygame.QUIT:
                running = False

            wheel.process_event(event)

        pygame.event.pump()

        control = wheel.read()

        gui.update(control)
        gui.draw()

        clock.tick(60)

    pygame.quit()


if __name__ == "__main__":
    main()