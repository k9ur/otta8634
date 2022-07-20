import random
from pyglet.gl import *

TICKS_PER_SEC = 60.0
MAX_NUM = 25000
SIZE = 20

class Window(pyglet.window.Window):
	def __init__(self, *args, **kwargs):
		super(Window, self).__init__(*args, **kwargs)
		pyglet.clock.schedule_interval(self.update, 1.0 / TICKS_PER_SEC)
		self.batch = pyglet.graphics.Batch()
		self.width, self.height = self.get_size()
		self.x = self.width // 2
		self.y = self.height // 2
		self.num = 0
	def on_draw(self):
		self.batch.draw()
	def update(self, dt):
		if self.num < MAX_NUM:
			for _ in range(round(dt / 0.001)):
				x = -SIZE if (random.randint(0, 1) == 0 and self.x >= 0) or self.x >= self.width else SIZE
				y = -SIZE if (random.randint(0, 1) == 0 and self.y >= 0) or self.y >= self.height else SIZE
				self.batch.add(2, GL_QUADS, None, ("v2i", (self.x, self.y, self.x + x, self.y + y)), ("c3B", (self.num // (MAX_NUM // 255),) * 6))
				self.x += x
				self.y += y
			self.num += round(dt / 0.001)

if __name__ == "__main__":
	window = Window(width = 1500, height = 700, caption = "Random", resizable = False)
	glClearColor(0, 0, 0, 0)
	pyglet.app.run()
