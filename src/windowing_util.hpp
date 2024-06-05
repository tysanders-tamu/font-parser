#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <thread>


class windowing_util
{
private:
  GLFWwindow *window;
  static void glfwErrorCallback(int error, const char *description);
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void processInput(GLFWwindow *window);
public:
  windowing_util();
  ~windowing_util();
};
