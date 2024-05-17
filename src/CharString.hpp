#ifndef CHARSTRING_H
#define CHARSTRING_H
#include <memory>
#include <queue>
#include <vector>
#include <utility>

class CharString{
  CharString():x_pos(0),y_pos(0), defaultWidthX(256) {};
  ~CharString(){};

  std::queue<uint8_t> values;
  std::vector<std::pair<int,int>> points;
  std::vector<std::pair<int,int>> h_hints;
  std::vector<std::pair<int,int>> v_hints;
  std::vector<bool> hintmasks;
  std::vector<uint8_t> operators;
  int defaultWidthX;
  int x_pos;
  int y_pos;  

  void addPoint(int x, int y);
  void addHint(int x, int y, bool isV);
  int getNextInt();
};

#endif 