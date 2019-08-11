#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

// #include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

using namespace std;

using color = array<float, 4>;
int screen_width = 600;
int screen_height = 400;
float origin_x = -0.5;
float origin_y = 0;
float height = 2;
float width = static_cast<float>(screen_width) / screen_height * height;
float x_min = origin_x - 0.5 * width;
float y_min = origin_y - 0.5 * height;
float x_max = origin_x + 0.5 * width;
float y_max = origin_y + 0.5 * height;
vector<color> pixel_buffer(screen_width* screen_height);

complex<float> julia_coeff{};

void draw_gradient() {
  for (int j = 0; j < screen_height; ++j) {
    for (int i = 0; i < screen_width; ++i) {
      const auto index = j * screen_width + i;
      const auto scale = static_cast<float>(j) / screen_height;
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }
}

void draw_mandelbrot() {
#pragma omp parallel for schedule(dynamic)
  for (int j = 0; j < screen_height; ++j) {
    for (int i = 0; i < screen_width; ++i) {
      const auto index = j * screen_width + i;
      const auto x = static_cast<float>(i) / screen_width;
      const auto y = static_cast<float>(j) / screen_height;
      complex<float> c{(x_max - x_min) * x + x_min,
                       (y_max - y_min) * y + y_min};
      auto z = c;

      constexpr int max_it = 1 << 10;
      int it = 0;
      for (; (norm(z) < 4) && (it < max_it); ++it) z = z * z + c;

      auto scale = log(1.f + it) / log(1.f + max_it);
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }
}

void draw_julia() {
#pragma omp parallel for schedule(dynamic)
  for (int j = 0; j < screen_height; ++j) {
    for (int i = 0; i < screen_width; ++i) {
      const auto index = j * screen_width + i;
      const auto x = static_cast<float>(i) / screen_width;
      const auto y = static_cast<float>(j) / screen_height;
      complex<float> z{(x_max - x_min) * x + x_min,
                       (y_max - y_min) * y + y_min};
      // complex<float> c{0.5, -0.2};

      constexpr int max_it = 1 << 10;
      int it = 0;
      for (; (norm(z) < 4) && (it < max_it); ++it) z = z * z + julia_coeff;

      auto scale = log(1.f + it) / log(1.f + max_it);
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }
}

void compute_viewport() {
  width = static_cast<float>(screen_width) / screen_height * height;
  x_min = origin_x - 0.5 * width;
  y_min = origin_y - 0.5 * height;
  x_max = origin_x + 0.5 * width;
  y_max = origin_y + 0.5 * height;
}

int main() {
  // create the window
  sf::Window window({screen_width, screen_height}, "OpenGL", sf::Style::Default,
                    sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true);

  // activate the window
  window.setActive(true);

  // load resources, initialize the OpenGL states, ...
  glClearColor(0, 0, 0, 1);
  draw_mandelbrot();

  int old_mouse_x = 0;
  int old_mouse_y = 0;

  // run the main loop
  bool running = true;
  while (running) {
    const auto mouse_pos = sf::Mouse::getPosition(window);
    const int mouse_x = mouse_pos.x;
    const int mouse_y = mouse_pos.y;

    // handle events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        // end the program
        running = false;
      } else if (event.type == sf::Event::Resized) {
        // adjust the viewport when the window is resized
        screen_width = event.size.width;
        screen_height = event.size.height;
        compute_viewport();
        glViewport(0, 0, screen_width, screen_height);
        pixel_buffer.resize(screen_width * screen_height);
        draw_julia();
      } else if (event.type == sf::Event::MouseWheelMoved) {
        height *= exp(-event.mouseWheel.delta * 0.05f);
        height = clamp(height, 1e-4f, 6.f);
        compute_viewport();
        draw_julia();
      }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      const auto delta_x =
          static_cast<float>(mouse_x - old_mouse_x) / screen_width * width;
      const auto delta_y =
          static_cast<float>(mouse_y - old_mouse_y) / screen_height * height;

      origin_x -= delta_x;
      origin_y += delta_y;

      compute_viewport();
      draw_julia();

      // cout << "move\t" << delta_x << ", " ;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      const auto x = static_cast<float>(mouse_x) / screen_width * width + x_min;
      const auto y =
          static_cast<float>(mouse_y) / screen_height * height + y_min;
      julia_coeff = {x, y};
      compute_viewport();
      draw_julia();
    }

    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw...
    glDrawPixels(screen_width, screen_height, GL_RGBA, GL_FLOAT,
                 pixel_buffer.data());

    // end the current frame (internally swaps the front and back buffers)
    window.display();

    old_mouse_x = mouse_x;
    old_mouse_y = mouse_y;
  }
}