#include "CharString.hpp"

void CharString::addPoint(int x, int y){
  points.push_back({x,y});
}

void CharString::addHint(int x, int y, bool isV){
  if(isV){
    v_hints.push_back({x,y});
  }else{
    h_hints.push_back({x,y});
  }
}

int32_t CharString::getNextNum(){
  int32_t result = 0;
  uint8_t val = values.front();
  if (val = 28){
    values.pop();
    result = values.front();
    values.pop();
    result = (result << 8) | values.front();
    values.pop();
  }
  else if(val < 32){
    throw("NaN - Reserved Operator");
  }else if (val < 247){
    values.pop();
    result = val - 139;
  } else if (val < 251){
    values.pop();
    result = (((val - 247) << 8) + values.front() + 108);
    values.pop();
  } else if (val < 255){
    values.pop();
    result = -((val - 251) << 8) - values.front()  - 108;
    values.pop();
  } else{
    values.pop();
    result = (val << 24) | (values.front() << 16);
    values.pop();
    result = (result | (values.front() << 8));
    values.pop();
    result = (result | values.front());
    values.pop();
  }   
  return result;
}

void CharString::getValsQueue(const std::vector<uint8_t> &vals){
  for (auto val : vals){
    values.push(val);
  }
}

void CharString::parseVals(){
  while (!values.empty()){
    uint16_t val = 0x0000 | values.front();
    if (val == 12){
      values.pop();
      val = values.front();
      values.pop();
      std::string op = two_byte_operators[val];
      if(op != ""){
        opers.push(op);
      } else {
        throw("Reserved Operator");
      }
    } 
    else if (val < 32 && val != 28){
      opers.push(one_byte_operators[val]);
      values.pop();
    } 
    else {
      nums.push(getNextNum());
    }
  }
}