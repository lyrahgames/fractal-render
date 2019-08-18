#include <application.hpp>

using namespace std;

application::application() : pixel_buffer(screen_width * screen_height) {
  // Turn on vertical synchronisation for constant 60 FPS on monitor. Reduces
  // artifacts on screen due to human vision.
  window.setVerticalSyncEnabled(true);
  // Activate the window. Change OpenGL context to use this window. Call every
  // time you want to use another window.
  window.setActive(true);
}

void application::execute() {
  bool running = true;
  while (running) {
    const auto current_time = chrono::high_resolution_clock::now();
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
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      const auto x = static_cast<float>(mouse_x) / screen_width * width + x_min;
      const auto y =
          static_cast<float>(mouse_y) / screen_height * height + y_min;
      js.coeff = {x, y};
      compute_viewport();
      draw_julia();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      animate_julia(chrono::duration<float>(current_time - old_time).count());
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
    old_time = current_time;
  }
}

void application::draw_gradient() {
  for (int j = 0; j < screen_height; ++j) {
    for (int i = 0; i < screen_width; ++i) {
      const auto index = j * screen_width + i;
      const auto scale = static_cast<float>(j) / screen_height;
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }
}

void application::draw_mandelbrot() {
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

void application::draw_julia() {
  float max_distance = 0.0f;

#pragma omp parallel for schedule(dynamic)
  for (int j = 0; j < screen_height; ++j) {
    for (int i = 0; i < screen_width; ++i) {
      const auto index = j * screen_width + i;
      const auto x = static_cast<float>(i) / screen_width;
      const auto y = static_cast<float>(j) / screen_height;
      complex<float> z{(x_max - x_min) * x + x_min,
                       (y_max - y_min) * y + y_min};

      auto distance = js.distance(z);
      max_distance = max(max_distance, distance);
      // auto scale = log(1.f + js.iteration(z)) / log(1.f + js.max_iteration);
      // auto scale = 1 - sqrt(tanh(distance));
      // auto scale = distance;
      // scale = log(1.0f + scale) / log(2.0f);
      // auto scale = js.distance(z);
      auto scale = log(1.0f + js.distance(z));
      // auto scale = (js.distance(z) < 1e-4f) ? (1.0f) : (0.0f);
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }

#pragma omp parallel for schedule(static)
  for (int j = 0; j < screen_height; ++j) {
    for (int i = 0; i < screen_width; ++i) {
      const auto index = j * screen_width + i;
      // for (int k = 0; k < 3; ++k) pixel_buffer[index][k] /= max_distance;
      for (int k = 0; k < 3; ++k)
        pixel_buffer[index][k] /= log(1.0f + max_distance);
    }
  }
}

void application::animate_julia(float dt) {
  js.coeff *= complex{cos(dt), sin(dt)};
}

void application::compute_viewport() {
  width = static_cast<float>(screen_width) / screen_height * height;
  x_min = origin_x - 0.5 * width;
  y_min = origin_y - 0.5 * height;
  x_max = origin_x + 0.5 * width;
  y_max = origin_y + 0.5 * height;
}