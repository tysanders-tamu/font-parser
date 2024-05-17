#include "CharString.hpp"

void CharString::addPoint(int x, int y){
  points.push_back(std::make_pair(x,y));
}

void CharString::addHint(int x, int y, bool isV){
  if(isV){
    v_hints.push_back(std::make_pair(x,y));
  }else{
    h_hints.push_back(std::make_pair(x,y));
  }
}

int CharString::getNextInt(){
  int result = 0;
  while(!values.empty()){
    uint8_t val = values.front();
    values.pop();
    if (val = 28){
      result = values.front();
      values.pop();
      result = (result << 8) | values.front();
      values.pop();
    }
    else if(val < 32){
      operators.push_back(val);
      return getNextInt();
    }else if (val < 247){
      result = val - 139;
    } else if (val < 251){
      result = (result << 8) | (((val - 247) << 8) | values.front());
      values.pop();
    } else if (val < 255){
      result = (result << 8) | (-(val - 251) << 8) | values.front();
      values.pop();
    } else{
      result = (result << 8) | (values.front() << 8) | values.front();
      values.pop();
      values.pop();
    }
    if(val < 32){
      break;
    }
  }
  return result;
}

