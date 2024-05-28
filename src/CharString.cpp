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

// Parse the values in the values queue and populate the opers and nums vectors
void CharString::parseVals(){
  int num_count = 0;
  bool getWidth = true;
  int numHints = 0;

  while (!values.empty()){
    uint16_t val = values.front();

    if (val == 12){
      // Handle two-byte operators
      values.pop();
      val = values.front();
      uint16_t op_num = 256 + val;
      oper op = {op_num,two_byte_operators[val],num_count};
      values.pop();
      num_count = 0;

      if(op.name != ""){
        opers.push_back(op);
      } else {
        throw("Reserved Operator");
      }
    } else if (val == 19){
      // Handle hintmask
      opers.push_back({19,"hintmask",num_count});
      values.pop();
      int numBytes = (numHints + 7) / 8;
      for (int i = 0; i < numBytes; ++i){
        uint8_t mask = values.front();
        values.pop();
        hintmasks.push_back(mask);
      }
    } else if (val == 20){
      // Handle cntrmask
      opers.push_back({20,"cntrmask",num_count});
      values.pop();
      numHints = h_hints.size() + v_hints.size();
      int numBytes = (numHints + 7) / 8;
      for (int i = 0; i < numBytes; ++i){
        uint8_t mask = values.front();
        values.pop();
        hintmasks.push_back(mask);
      }
    } else if (val == 28){
      // Handle two-byte number
      values.pop();
      int32_t num = getNextNum();
      nums.push_back(num);
      num_count++;
    } else if (val == 255){
      // Handle long number
      values.pop();
      int32_t num = getNextNum();
      nums.push_back(num);
      num_count++;
    } 
    else if (val < 32 && val != 28){
      // Handle one-byte operators
      if (getWidth && (num_count%2 == 1)){
        width = nominalWidthX + nums[0];
        getWidth = false;
      }
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

void CharString::updateValues(){
  if (opers.empty() || nums.empty()){
    throw("Empty Queues");
  }
  int currNum = 0;
  for (int oper_loc = 0; oper_loc < opers.size(); ++oper_loc){
    oper current_op = opers[oper_loc];
    if (current_op.name == ""){
      throw("Empty Operator");
    }
    switch (current_op.op_val)
    {
    case 1://hstem
      for (int i = 0; i < current_op.numCount; i+=2){
        addHint(nums[currNum],nums[currNum+1], false);
        currNum += 2;
      }
      break;
    case 3://vstem
      for (int i = 0; i < current_op.numCount; i+=2){
        addHint(nums[currNum],nums[currNum+1], true);
        currNum += 2;
      }
      break;
    case 4://vmoveto
      y_pos += nums[currNum];
      currNum++;
      break;
    case 5://rlineto
      for (int i = 0; i < current_op.numCount; i+=2){
        x_pos += nums[currNum];
        y_pos += nums[currNum+1];
        addPoint(x_pos, y_pos);
        currNum += 2;
      }
      break;
    case 6://hlineto
    bool horizontal = true;
      for (int i = 0; i < current_op.numCount; i++){
        if (horizontal){
          x_pos += nums[currNum];
          addPoint(x_pos, y_pos);
        } else {
          y_pos += nums[currNum];
          addPoint(x_pos, y_pos);
        }
        currNum++;
      }
      break;
    case 7://vlineto
    bool vertical = true;
      for (int i = 0; i < current_op.numCount; i++){
        if (vertical){
          y_pos += nums[currNum];
          addPoint(x_pos, y_pos);
        } else {
          x_pos += nums[currNum];
          addPoint(x_pos, y_pos);
        }
        currNum++;
      }
      break;
    case 8://rrcurveto
      for (int i = 0; i < current_op.numCount; i+=6){
        x_pos += nums[currNum];
        y_pos += nums[currNum+1];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+2];
        y_pos += nums[currNum+3];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+4];
        y_pos += nums[currNum+5];
        addPoint(x_pos, y_pos);
        currNum += 6;
      }
      break;
    case 10: //callsubr
      //callsubr(currNum);
      break;
    case 11: //return
      break;
    case 12: //escape
      break;
    case 14: //endchar
      return;
      break;
    case 18://hstemhm
      for (int i = 0; i < current_op.numCount; i+=2){
        addHint(nums[currNum],nums[currNum+1], false);
        currNum += 2;
      }
      break;
    case 19: //hintmask
      //handled in parseVals
      break;
    case 20: //cntrmask
      //handled in parseVals
      break;
    case 23://vstemhm
      for (int i = 0; i < current_op.numCount; i+=2){
        addHint(nums[currNum],nums[currNum+1], true);
        currNum += 2;
      }
      break;
    default:
      break;
    }
  }
}
