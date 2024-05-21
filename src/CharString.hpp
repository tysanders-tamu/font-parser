#ifndef CHARSTRING_H
#define CHARSTRING_H
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include <utility>


struct hint{
  int x;
  int y;
  //bool active = true;
};

struct point{
  int x;
  int y;
};

struct oper{
  uint16_t op_val;
  std::string name;
  int numCount;
};


const std::string one_byte_operators[] = {
  "",
  "hstem",
  "",
  "vstem",
  "vmoveto",
  "rlineto",
  "hlineto",
  "vlineto",
  "rrcurveto",
  "",
  "callsubr",
  "return",
  "escape",
  "",
  "endchar",
  "",
  "",
  "",
  "hstemhm",
  "hintmask",
  "cntrmask",
  "rmoveto",
  "hmoveto",
  "vstemhm",
  "rcurveline",
  "rlinecurve",
  "vvcurveto",
  "hhcurveto",
  "shortint",
  "callgsubr",
  "vhcurveto",
  "hvcurveto"
};

const std::string two_byte_operators[] = {
  "",
  "",
  "",
  "and",
  "or",
  "not",
  "",
  "",
  "",
  "abs",
  "add",
  "sub",
  "div",
  "",
  "neg",
  "eq",
  "",
  "",
  "drop",
  "",
  "put",
  "get",
  "ifelse",
  "random",
  "mul",
  "",
  "sqrt",
  "dup",
  "exch",
  "index",
  "roll",
  "",
  "",
  "",
  "hflex",
  "flex",
  "hflex1",
  "flex1",
  ""
};

class CharString{
  CharString():x_pos(0),y_pos(0), defaultWidthX(256), nominalWidth(256), width(256) {};
  ~CharString(){};

  std::queue<uint8_t> values;
  std::vector<point> points;
  std::vector<hint> h_hints;
  std::vector<hint> v_hints;
  std::vector<bool> hintmasks;
  std::vector<oper> opers;
  std::vector<int32_t> nums;
  uint width;
  uint defaultWidthX;
  uint nominalWidth;
  int x_pos;
  int y_pos;  

  void addPoint(int x, int y);
  void addHint(int x, int y, bool isV);
  int32_t getNextNum();
  void getValsQueue(const std::vector<uint8_t> &vals);
  void parseVals();
  void updateArrs();
};

#endif