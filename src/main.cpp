#include <iostream>
#include <fstream>
#include <vector>

#include <fmt/core.h>
#include <arpa/inet.h>
#include <inttypes.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "parserClass.hpp"

using namespace std;

static void glfwErrorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW returned an error:\n\t%s (%i)\n", description, error);
}

GLFWwindow* initialise() {
  if (!glfwInit()) {
    fmt::print("Failed to initialise GLFW\n");
    exit(EXIT_FAILURE);
  }


    glfwSetErrorCallback(glfwErrorCallback);

  // Create a window
  GLFWwindow* window = glfwCreateWindow(800, 600, "Font Parser", NULL, NULL);
  if (!window) {
    fmt::print("Failed to create GLFW window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Make the window the current context
  glfwMakeContextCurrent(window);

  return window;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  GLFWwindow* window = initialise();

  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}



// w? {hs* vs* cm* hm* mt subpath}? {mt subpath}* endchar