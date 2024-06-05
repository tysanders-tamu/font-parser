#include "windowing_util.hpp"

windowing_util::windowing_util(){
  if (!glfwInit()){
    fmt::print("Failed to initialise GLFW\n");
    exit(EXIT_FAILURE);
  }

  //hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(800, 600, "Font Parser", NULL, NULL);

  if (!window){
    fmt::print("Failed to create GLFW window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    fmt::print("Failed to initialise GLAD\n");
    exit(EXIT_FAILURE);
  }
  
  while(!glfwWindowShouldClose(window)){
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

windowing_util::~windowing_util(){
  glfwTerminate();
}

//helpers
void windowing_util::processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void windowing_util::framebuffer_size_callback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}