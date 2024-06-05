#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <vector>

class letter
{
private:
  char symbol;
  int resolution;
  int screen_width;
  int screen_height;
  int n_contours;
  FT_BBox bbox;
  std::vector<std::vector<GLfloat>> contours;
public:
  letter(FT_Face face, char symbol, int resolution, int screen_width, int screen_height);
  ~letter();

//funcs
private:
  void linear_interpolation(FT_Vector& dest, FT_Vector p0, FT_Vector p1, float t);
  void cubic_interpolation(FT_Vector& dest, FT_Vector a, FT_Vector b, FT_Vector c, FT_Vector d, float t);
  std::vector<GLfloat> get_cubic_curve_points(FT_Vector p0, FT_Vector p1, FT_Vector p2, FT_Vector p3, int resolution);
  void normalize_coordinates();
public:
 std::vector<std::vector<GLfloat>> get_contours();
  void print_contours();
};
