#include "letter.hpp"

letter::letter(FT_Face face, char symbol, int resolution, int screen_width, int screen_height)
: symbol(symbol), resolution(resolution), screen_width(screen_width), screen_height(screen_height)
{
  //init n_contours
  n_contours = face->glyph->outline.n_contours;
  
  if ( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
      std::cerr << "Glyph format is not outline." << std::endl;
      return;
  }

  FT_Outline* outline = &face->glyph->outline;


  FT_Glyph glyph;
  if (FT_Get_Glyph(face->glyph, &glyph)) {
      std::cerr << "Failed to get glyph" << std::endl;
      return;
  }
  FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &bbox);


  for (size_t contour_i = 0; contour_i < outline->n_contours; contour_i++)
  {
    size_t start = (contour_i == 0) ? 0 :  outline->contours[contour_i - 1] + 1;
    size_t end =  outline->contours[contour_i];

    std::vector<GLfloat> contour;


    int itr = start;
    fmt::print("start: {0}, end: {1}\n", start, end);
    for (;;)
    {
      //if linear, linearlly interpolate
      if ( outline->tags[itr + 1] & FT_CURVE_TAG_ON) {
        for (int j = 0; j < resolution; j++){
          FT_Vector point;
          linear_interpolation(point,  outline->points[itr],  outline->points[itr + 1], j / float(resolution));
          contour.push_back(point.x);
          contour.push_back(point.y);
          contour.push_back(0.0f);
        }
        itr += 1;
      }
      else if (  outline->tags[itr + 1] & FT_CURVE_TAG_CUBIC) {
        FT_Vector p0 =  outline->points[itr];
        FT_Vector p1 =  outline->points[itr + 1];
        FT_Vector p2 =  outline->points[itr + 2];
        FT_Vector p3 =  outline->points[itr + 3];

        std::vector<GLfloat> cubic_curve_points = get_cubic_curve_points(p0, p1, p2, p3, resolution);
        contour.insert(contour.end(), cubic_curve_points.begin(), cubic_curve_points.end());
        itr += 3;
      }

      if (itr == end) break;
    }

    // close the curve
    for (int j = 0; j < resolution; j++){
      FT_Vector point;
      linear_interpolation(point,   outline->points[itr],   outline->points[start], j / float(resolution));
      contour.push_back(point.x);
      contour.push_back(point.y);
      contour.push_back(0.0f);
    }

    //add the curve to the list of curves
    contours.push_back(contour);

  }//end for each contour

  normalize_coordinates();
}

letter::~letter()
{
}

void letter::linear_interpolation(FT_Vector& dest, FT_Vector p0, FT_Vector p1, float t) {
    dest.x = p0.x + t * (p1.x - p0.x);
    dest.y = p0.y + t * (p1.y - p0.y);
}

void letter::cubic_interpolation(FT_Vector& dest, FT_Vector a, FT_Vector b, FT_Vector c, FT_Vector d, float t) {
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
std::vector<GLfloat> letter::get_cubic_curve_points(FT_Vector p0, FT_Vector p1, FT_Vector p2, FT_Vector p3, int resolution) {
    std::vector<GLfloat> points;

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

void letter::normalize_coordinates(){
  for (size_t i = 0; i < n_contours; ++i){
    for (size_t j = 0; j < contours[i].size(); j += 3){
      // fmt::print("[{},{},{}]\n", contours[i][j], contours[i][j + 1], contours[i][j + 2]);
      contours[i][j] = ((contours[i][j] - bbox.xMin) / (bbox.xMax - bbox.xMin)) - 0.5f;
      contours[i][j + 1] = ((contours[i][j + 1] - bbox.yMin) / (bbox.yMax - bbox.yMin)) -0.5f;
    }
  }
}

//func to return the contours
std::vector<std::vector<GLfloat>> letter::get_contours(){
  return contours;
}

//print the contours
void letter::print_contours(){
  for (size_t i = 0; i < contours.size(); ++i){
    for (size_t j = 0; j < contours[i].size(); j += 3){
      std::cout << contours[i][j] << " " << contours[i][j + 1] << " " << contours[i][j + 2] << std::endl;
    }
  }
}