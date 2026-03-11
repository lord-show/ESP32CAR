import pygame


class Dashboard:

    def __init__(self):

        self.screen = pygame.display.set_mode((600, 400))
        pygame.display.set_caption("ESPCar Control")

        self.font = pygame.font.SysFont("consolas", 24)

        self.control = {
            "steer": 0,
            "throttle": 0,
            "brake": 0,
            "buttons": 0
        }

    def update(self, control):

        self.control = control

    def draw(self):

        self.screen.fill((20, 20, 20))

        self._draw_text(f"Steer: {self.control['steer']:.2f}", 50)
        self._draw_text(f"Throttle: {self.control['throttle']:.2f}", 100)
        self._draw_text(f"Brake: {self.control['brake']:.2f}", 150)
        self._draw_text(f"Buttons: {self.control['buttons']:08b}", 200)

        pygame.display.flip()

    def _draw_text(self, text, y):

        surface = self.font.render(text, True, (200, 200, 200))
        self.screen.blit(surface, (40, y))