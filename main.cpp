#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include <array>
#include <complex>
#include <vector>

using namespace std;

using color = array<float, 4>;
int width = 600;
int height = 400;
vector<color> pixel_buffer(width* height);

void draw_gradient() {
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const auto index = j * width + i;
      const auto scale = static_cast<float>(j) / height;
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }
}

void draw_mandelbrot() {
#pragma omp parallel for schedule(dynamic)
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const auto index = j * width + i;
      constexpr float x_min = -2;
      constexpr float y_min = -1;
      constexpr float x_max = 1;
      constexpr float y_max = 1;
      const auto x = static_cast<float>(i) / width;
      const auto y = static_cast<float>(j) / height;
      complex<float> c{(x_max - x_min) * x + x_min,
                       (y_max - y_min) * y + y_min};
      auto z = c;

      constexpr int max_it = 1 << 10;
      constexpr int step = 1 << 4;
      int it = 0;
      for (; (norm(z) < 4) && (it < max_it); it += step)
        for (int k = 0; k < step; ++k) z = z * z + c;
      pixel_buffer[index] =
          (it == max_it) ? (color{0, 0, 0, 1}) : (color{1, 1, 1, 1});
    }
  }
}

int main() {
  // create the window
  sf::Window window({width, height}, "OpenGL", sf::Style::Default,
                    sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true);

  // activate the window
  window.setActive(true);

  // load resources, initialize the OpenGL states, ...
  glClearColor(0, 0, 0, 1);
  draw_mandelbrot();

  // run the main loop
  bool running = true;
  while (running) {
    // handle events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        // end the program
        running = false;
      } else if (event.type == sf::Event::Resized) {
        // adjust the viewport when the window is resized
        glViewport(0, 0, event.size.width, event.size.height);
        width = event.size.width;
        height = event.size.height;
        pixel_buffer.resize(width * height);
        draw_mandelbrot();
      }
    }

    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw...
    glDrawPixels(width, height, GL_RGBA, GL_FLOAT, pixel_buffer.data());

    // end the current frame (internally swaps the front and back buffers)
    window.display();
  }
}