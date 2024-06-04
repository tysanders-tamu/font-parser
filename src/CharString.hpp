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

struct byte_parser{ //vector parser helper struct. Keeps position between functions
  std::vector<uint8_t> vals; //values in the charstring
  int oper_pos; //position in the operators vector
  int pos; //position in the vector
  // std::vector<uint8_t> subrs;
};


//reserved operators left blank
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
//reserved operators left blank
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
  CharString():x_pos(0),y_pos(0), defaultWidthX(256), nominalWidthX(256), width(256) {};
  ~CharString(){};

  // std::queue<uint8_t> values;
  byte_parser parser;
  std::vector<point> points;
  std::vector<hint> h_hints;
  std::vector<hint> v_hints;
  std::vector<bool> hintmasks;
  std::vector<oper> opers;
  std::vector<int32_t> nums;
  uint width;
  uint defaultWidthX;
  uint nominalWidthX;
  int x_pos;
  int y_pos;  

  void addPoint(int x, int y);
  void addHint(int x, int y, bool isV);
  int32_t getNextNum();
  void getValsQueue(const std::vector<uint8_t> &vals);
  void parseVals();
  void updateValues();

  void rmoveto();
  void hmoveto();
  void vmoveto();
  void rlineto();
  void hlineto();
  void vlineto();
  void rrcurveto();
  void hhcurveto();
  void hvcurveto();
  void rcurveline();
  void rlinecurve();
  void vhcurveto();
  void vvcurveto();
  void flex();
  void hflex();
  void hflex1();
  void flex1();
  void hstem();
  void vstem();
  void hstemhm();
  void vstemhm();


  void callsubr();
};

#endif