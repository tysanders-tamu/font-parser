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
  int num_count = 0;
  while (!values.empty()){
    uint16_t val = values.front();
    if (val == 12){
      values.pop();
      val = values.front();
      uint16_t op_num = 12<<2 | val;
      oper op = {op_num,two_byte_operators[val],num_count};
      values.pop();
      num_count = 0;
      if(op.name != ""){
        opers.push_back(op);
      } else {
        throw("Reserved Operator");
      }
    } 
    else if (val < 32 && val != 28){
      oper op = {(uint16_t)(val),one_byte_operators[val],num_count};
      opers.push_back(op);
      values.pop();
      num_count = 0;
    } 
    else {
      nums.push_back(getNextNum());
      num_count++;
    }
  }
}

void CharString::updateArrs(){
  if (opers.empty() || nums.empty()){
    throw("Empty Queues");
  }
  int currNum = 0;
  for (int oper_loc = 0; oper_loc < opers.size(); ++oper_loc){
    oper current_op = opers[oper_loc];
    if (current_op.name == ""){
      throw("Empty Operator");
    }

    switch(current_op.op_val){
      case 1: // hstem
        if ()
      default:
        throw("Invalid Operator or Reserved Operator");
    }
  }
}