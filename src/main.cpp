#include <iostream>
#include <fstream>
#include <vector>

#include <fmt/core.h>
#include <arpa/inet.h>
#include <inttypes.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "parserClass.hpp"
#include "windowing_util.hpp"

#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

using namespace std;


//CONST SHADER SOURCES
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
/////////////////////


void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


void print_outline(const FT_Outline* outline) {
    for (int contour = 0; contour < outline->n_contours; contour++) {
        int start = (contour == 0) ? 0 : outline->contours[contour - 1] + 1;
        int end = outline->contours[contour];
        std::cout << "Contour " << contour << ":\n";

        for (int i = start; i <= end; i++) {
            FT_Vector point = outline->points[i];
            char tag = outline->tags[i];
            if (tag & FT_CURVE_TAG_ON) {
                std::cout << "  On curve point: (" << point.x << ", " << point.y << ")\n";
            } else if (tag & FT_CURVE_TAG_CUBIC) {
                std::cout << "  Cubic control point: (" << point.x << ", " << point.y << ")\n";
            } else {
                std::cout << "  Quadratic control point: (" << point.x << ", " << point.y << ")\n";
            }
        }
        std::cout << "\n";
    }
}

void linear_interpolation(FT_Vector& dest, FT_Vector p0, FT_Vector p1, float t) {
    dest.x = p0.x + t * (p1.x - p0.x);
    dest.y = p0.y + t * (p1.y - p0.y);
}

void cubic_interpolation(FT_Vector& dest, FT_Vector a, FT_Vector b, FT_Vector c, FT_Vector d, float t) {
    FT_Vector ab, bc, cd, abbc, bccd;

    linear_interpolation(ab, a, b, t);
    linear_interpolation(bc, b, c, t);
    linear_interpolation(cd, c, d, t);
    linear_interpolation(abbc, ab, bc, t);
    linear_interpolation(bccd, bc, cd, t);
    linear_interpolation(dest, abbc, bccd, t);
}
/*
p0: oncurve origin
p1: control point 1
p2: control point 2
p3: oncurve destination
*/
vector<GLfloat> get_cubic_curve_points(FT_Vector p0, FT_Vector p1, FT_Vector p2, FT_Vector p3, int resolution = 10) {
    vector<GLfloat> points;

    for (int i = 0; i < resolution; i++) {
        float t = i / float(resolution);
        FT_Vector point;
        cubic_interpolation(point, p0, p1, p2, p3, t);
        points.push_back(point.x);
        points.push_back(point.y);
        points.push_back(0.0f);
    }

    return points;
}

int main() {
  FT_Library library;
  FT_Face face;

  if (FT_Init_FreeType(&library)){
    fmt::print("Failed to initialise FreeType\n");
    exit(EXIT_FAILURE);
  }

  if (FT_New_Face(library, "test.otf", 0, &face)){
    fmt::print("Failed to load font\n");
    exit(EXIT_FAILURE);
  }

  if (FT_Set_Char_Size(face, 0, 16*64, 300, 300)) {
    std::cerr << "Could not set character size\n";
    return 1;
  }


  //CHANGE LETTER HERE
  FT_UInt glyph_index = FT_Get_Char_Index(face, 'A');
  if (FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE)) {
    std::cerr << "Could not load glyph\n";
    return 1;
  }

  if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
    std::cerr << "Glyph is not in outline format\n";
    return 1;
  }

  FT_Glyph glyph;
  if (FT_Get_Glyph(face->glyph, &glyph)) {
      std::cerr << "Could not get glyph\n";
      return 1;
  }

  // print_outline(&face->glyph->outline);
  
  /////////////////////////////////OPENGL//////////////////////////////////////
  if (!glfwInit()){
    fmt::print("Failed to initialise GLFW\n");
    exit(EXIT_FAILURE);
  }


  //hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Font Parser", NULL, NULL);


  //initalization of window and glad
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
  ////////////////////////////////////////
  //shader initialization
  //vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if(!success){
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR:SHADER COMP FAIL::\n" << infoLog << std::endl;
  }
  //frag shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR:SHADER COMP FAIL::\n" << infoLog << std::endl;
  }
  //link shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  //check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);


  //get points
  // print_outline(&face->glyph->outline);
  FT_BBox bbox;
  FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &bbox);

  //get only on curve points
  std::vector<GLfloat> outline_points;
  int i = 0;

  int resolution = 100;


  //! last needs to connect to first
  for(;;){
    //if linear, linearlly interpolate
    if (face->glyph->outline.tags[i + 1] & FT_CURVE_TAG_ON){
      for (int j = 0; j < resolution; j++){
        FT_Vector point;
        linear_interpolation(point, face->glyph->outline.points[i], face->glyph->outline.points[i + 1], j / float(resolution));
        outline_points.push_back(point.x);
        outline_points.push_back(point.y);
        outline_points.push_back(0.0f);
      }
      i += 1;
    }
    else if (face->glyph->outline.tags[i + 1] & FT_CURVE_TAG_CUBIC){
      FT_Vector p0 = face->glyph->outline.points[i];
      FT_Vector p1 = face->glyph->outline.points[i + 1];
      FT_Vector p2 = face->glyph->outline.points[i + 2];
      FT_Vector p3 = face->glyph->outline.points[i + 3];

      vector<GLfloat> cubic_curve_points = get_cubic_curve_points(p0, p1, p2, p3, resolution);
      outline_points.insert(outline_points.end(), cubic_curve_points.begin(), cubic_curve_points.end());
      i += 3;
    }


    if (i >= face->glyph->outline.n_points)
      break;
  }



  //translates to normalized coordinates, scales to be positive
  for (int i = 0; i < outline_points.size(); i += 3) {
        outline_points[i] = ( (outline_points[i] + abs(bbox.xMin) )/float(abs(bbox.xMin) + abs(bbox.xMax)) ); //x
        outline_points[i+1] = ( (outline_points[i+1] + abs(bbox.yMin) )/float(abs(bbox.yMin) + abs(bbox.yMax)) ); //y
        //z stays the same
  }

  //translate to be centered
  for (int i = 0; i < outline_points.size(); i += 3) {
      outline_points[i] -= 0.5f;
      outline_points[i + 1] -= 0.5f;
  }

  // convert vector to array
  GLfloat points[outline_points.size()];
  for (int i = 0; i < outline_points.size(); i++) {
      points[i] = outline_points[i];
  }

    // Set up vertex buffer and vertex array objects
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    glPointSize(4.0f);

    // Render loop
    int frame = 0;

    double previousTime = glfwGetTime();
    int frameCount = 0;

    glfwSwapInterval(0);

    //! add timing function, so that it renders at a certain speed, regardless of the speed of the computer
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        frameCount++;
        if ( currentTime - previousTime >= 1.0 ){
            fmt::print("FPS: {}\n", frameCount);
            frameCount = 0;
            previousTime = currentTime;
        }
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw points
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        
        //slowly render points
        glDrawArrays(GL_POINTS, 0, frame);
        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        frame+=3;
        if (frame >= outline_points.size() / 3)
            frame = 0;
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    print_outline(&face->glyph->outline);

    return 0;
}


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}