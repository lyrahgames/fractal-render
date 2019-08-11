#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include <array>
#include <vector>

using namespace std;

using color = array<float, 4>;
int width = 800;
int height = 600;
vector<color> pixel_buffer(width* height);

int main() {
  // create the window
  sf::Window window({width, height}, "OpenGL", sf::Style::Default,
                    sf::ContextSettings(32));
  window.setVerticalSyncEnabled(true);

  // activate the window
  window.setActive(true);

  // load resources, initialize the OpenGL states, ...
  glClearColor(1, 0, 0, 1);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const auto index = j * width + i;
      const auto scale = static_cast<float>(j) / height;
      pixel_buffer[index] = {scale, scale, scale, 1};
    }
  }

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