#include <iostream>
#include "CharString.hpp"

void CharString::addPoint(const int &x, const int & y){
  points.push_back({x,y});
}

void CharString::addHint(const int & x, const int & y,const bool &isV){
  if(isV){
    v_hints.push_back({x,y});
  }else{
    h_hints.push_back({x,y});
  }
}

void CharString::setWidths(const int &defaultWidthX, const int &nominalWidthX){
  this->defaultWidthX = defaultWidthX;
  this->nominalWidthX = nominalWidthX;
  this->width = defaultWidthX;
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
      opers.push_back({val,one_byte_operators[val],num_count});
      values.pop();
      int numBytes = (numHints + 7) / 8;
      for (int i = 0; i < numBytes; ++i){
        uint8_t mask = values.front();
        values.pop();
        hintmasks.push_back(mask);
      }
    } else if (val == 20){
      // Handle cntrmask
      opers.push_back({val,one_byte_operators[val],num_count});
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
    //init variables for switch statement
    bool horizontal = true;
    bool vertical = true;
    bool has_extra = false;
    int sumy = 0;
    int sumx = 0;
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
      horizontal = true;
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
      vertical = true;
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
    case 21://rmoveto
      x_pos += nums[currNum];
      y_pos += nums[currNum+1];
      addPoint(x_pos, y_pos);
      currNum += 2;
      break;
    case 22://hmoveto
      x_pos += nums[currNum];
      addPoint(x_pos, y_pos);
      currNum++;
      break;
    case 23://vstemhm
      for (int i = 0; i < current_op.numCount; i+=2){
        addHint(nums[currNum],nums[currNum+1], true);
        currNum += 2;
      }
      break;
    case 24://rcurveline
      for (int i = 0; i < current_op.numCount-2; i+=6){
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
      x_pos += nums[currNum];
      y_pos += nums[currNum+1];
      addPoint(x_pos, y_pos);
      currNum += 2;
      break;
    case 25://rlinecurve
      for (int i = 0; i < current_op.numCount-6; i+=2){
        x_pos += nums[currNum];
        y_pos += nums[currNum+1];
        addPoint(x_pos, y_pos);
        currNum += 2;
      }
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
      break;
    case 26://vvcurveto
      if (current_op.numCount % 4 != 0){
        x_pos += nums[currNum];
        currNum++;
        addPoint(x_pos, y_pos);
      }
      for (int i = 0; i < current_op.numCount; i+=4){
        y_pos += nums[currNum];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+1];
        y_pos += nums[currNum+2];
        addPoint(x_pos, y_pos);
        y_pos += nums[currNum+3];
        currNum += 4;
      }
      break;
    case 27://hhcurveto
      if (current_op.numCount % 4 != 0){
        y_pos += nums[currNum];
        currNum++;
        addPoint(x_pos, y_pos);
      }
      for (int i = 0; i < current_op.numCount; i+=4){
        x_pos += nums[currNum];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+1];
        y_pos += nums[currNum+2];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+3];
        currNum += 4;
      }
      break;
    case 29://callgsubr
      //callgsubr(currNum);
      break;
    case 30://vhcurveto
      has_extra = false;
      if (current_op.numCount % 4 != 0){
        current_op.numCount--;
        has_extra = true;
      }
      //- dy1 dx2 dy2 dx3 {dxa dxb dyb dyc dyd dxe dye dxf}* dyf? 
      if ((current_op.numCount-4)%8 == 0){
        y_pos += nums[currNum];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+1];
        y_pos += nums[currNum+2];
        addPoint(x_pos, y_pos);
        x_pos += nums[currNum+3];
        addPoint(x_pos, y_pos);
        currNum += 4;
        for(int i = 0; i < current_op.numCount-4; i+=8){
          x_pos += nums[currNum];//dxa
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+1];//dxb
          y_pos += nums[currNum+2];//dyb
          addPoint(x_pos, y_pos);
          y_pos += nums[currNum+3];//dyc
          addPoint(x_pos, y_pos);
          y_pos += nums[currNum+4];//dyd
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+5];//dxe
          y_pos += nums[currNum+6];//dye
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+7];//dxf
          addPoint(x_pos, y_pos);
        }
        if (has_extra){//can have an extra point altering the last curve
          y_pos += nums[currNum];//dyf
          points.back().y = y_pos;
          current_op.numCount++;
          currNum++;
        }
      } else{
        //- {dya dxb dyb dxc dxd dxe dye dyf}+ dxf?
        for (int i = 0; i < current_op.numCount; i+=8){
          y_pos += nums[currNum];//dya
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+1];//dxb
          y_pos += nums[currNum+2];//dyb
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+3];//dxc
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+4];//dxd
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+5];//dxe
          y_pos += nums[currNum+6];//dye
          addPoint(x_pos, y_pos);
          y_pos += nums[currNum+7];//dyf
          addPoint(x_pos, y_pos);
          currNum += 8;
        }
        if(has_extra){//can have an extra point altering the last curve
          x_pos += nums[currNum];//dxf
          points.back().x = x_pos;
          current_op.numCount++;
          currNum++;
        }
      }
      break;
    case 31://hvcurveto
      has_extra = false;
      if (current_op.numCount % 4 != 0){
        current_op.numCount--;
        has_extra = true;
      }
      //- dx1 dx2 dy2 dy3 {dya dxb dyb dxc dxd dxe dye dyf}* dxf?
      if ((current_op.numCount-4)%8 == 0){
        x_pos += nums[currNum];
        addPoint(x_pos, y_pos);
        y_pos += nums[currNum+1];
        x_pos += nums[currNum+2];
        addPoint(x_pos, y_pos);
        y_pos += nums[currNum+3];
        addPoint(x_pos, y_pos);
        currNum += 4;
        for(int i = 0; i < current_op.numCount-4; i+=8){
          y_pos += nums[currNum];//dya
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+1];//dxb
          y_pos += nums[currNum+2];//dyb
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+3];//dxc
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+4];//dxd
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+5];//dxe
          y_pos += nums[currNum+6];//dye
          addPoint(x_pos, y_pos);
          y_pos += nums[currNum+7];//dyf
          addPoint(x_pos, y_pos);
          currNum += 8;
        }
        if (has_extra){//can have an extra point altering the last curve
          x_pos += nums[currNum];//dxf
          points.back().x = x_pos;
          current_op.numCount++;
          currNum++;
        }
      } else{
        //- {dxa dxb dyb dyc dyd dxe dye dxf}+ dyf?
        for (int i = 0; i < current_op.numCount; i+=8){
          x_pos += nums[currNum];//dxa
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+1];//dxb
          y_pos += nums[currNum+2];//dyb
          addPoint(x_pos, y_pos);
          y_pos += nums[currNum+3];//dyc
          addPoint(x_pos, y_pos);
          y_pos += nums[currNum+4];//dyd
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+5];//dxe
          y_pos += nums[currNum+6];//dye
          addPoint(x_pos, y_pos);
          x_pos += nums[currNum+7];//dxf
        }
        if(has_extra){//can have an extra point altering the last curve
          y_pos += nums[currNum];//dyf
          points.back().y = y_pos;
          current_op.numCount++;
          currNum++;
        }
      }
      break;
    case (256+3):
      //and
      break;
    case (256+4):
      //or
      break;
    case (256+5):
      //not
      break;
    case (256+9):
      //abs
      break;
    case (256+10):
      //add
      break;
    case (256+11):
      //sub
      break;
    case (256+12):
      //div
      break;
    case (256+14):
      //neg
      break;
    case (256+15):
      //eq
      break;
    case (256+18):
      //drop
      break;
    case (256+20):
      //put
      break;
    case (256+21):
      //get
      break;
    case (256+22):
      //ifelse
      break;
    case (256+23):
      //random
      break;
    case (256+24):
      //mul
      break;
    case (256+26):
      //sqrt
      break;
    case (256+27):
      //dup
      break;
    case (256+28):
      //exch
      break;
    case (256+29):
      //index
      break;
    case (256+30):
      //roll
      break;
    case (256+34):
      //hflex
      //- dx1 dx2 dy2 dx3 dx4 dx5 dx6
      x_pos += nums[currNum];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+1];
      y_pos += nums[currNum+2];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+3];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+4];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+5];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+6];
      addPoint(x_pos, y_pos);
      currNum += 7;
      flex_depths.push_back(50);
      break;
    case (256+35):
      //flex
      //- dx1 dy1 dx2 dy2 dx3 dy3 dx4 dy4 dx5 dy5 dx6 dy6 fd
      x_pos += nums[currNum];//dx1
      y_pos += nums[currNum+1];//dy1
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+2];//dx2
      y_pos += nums[currNum+3];//dy2
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+4];//dx3
      y_pos += nums[currNum+5];//dy3
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+6];//dx4
      y_pos += nums[currNum+7];//dy4
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+8];//dx5
      y_pos += nums[currNum+9];//dy5
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+10];//dx6
      y_pos += nums[currNum+11];//dy6
      addPoint(x_pos, y_pos);
      flex_depths.push_back(nums[currNum+12]);//fd
      currNum += 13;
      break;
    case (256+36):
      //hflex1
      //- dx1 dy1 dx2 dy2 dx3 dx4 dx5 dy5 dx6
      x_pos += nums[currNum];//dx1
      y_pos += nums[currNum+1];//dy1
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+2];//dx2
      y_pos += nums[currNum+3];//dy2
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+4];//dx3
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+5];//dx4
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+6];//dx5
      y_pos += nums[currNum+7];//dy5
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+8];//dx6
      addPoint(x_pos, y_pos);
      flex_depths.push_back(50);
      currNum += 9;
      break;
    case (256+37):
      //flex1
      //- dx1 dy1 dx2 dy2 dx3 dy3 dx4 dy4 dx5 dy5 d6
      sumx = 0;
      sumy = 0;
      x_pos += nums[currNum];//dx1
      sumx += nums[currNum];
      y_pos += nums[currNum+1];//dy1
      sumy += nums[currNum+1];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+2];//dx2
      sumx += nums[currNum+2];
      y_pos += nums[currNum+3];//dy2
      sumy += nums[currNum+3];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+4];//dx3
      sumx += nums[currNum+4];
      y_pos += nums[currNum+5];//dy3
      sumy += nums[currNum+5];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+6];//dx4
      sumx += nums[currNum+6];
      y_pos += nums[currNum+7];//dy4
      sumy += nums[currNum+7];
      addPoint(x_pos, y_pos);
      x_pos += nums[currNum+8];//dx5
      sumx += nums[currNum+8];
      y_pos += nums[currNum+9];//dy5
      sumy += nums[currNum+9];
      addPoint(x_pos, y_pos);
      if (abs(sumx) > abs(sumy)){
        x_pos += nums[currNum+10];//dx6
        addPoint(x_pos, y_pos);
      } else {
        y_pos += nums[currNum+10];//dy6
        addPoint(x_pos, y_pos);
      }
      flex_depths.push_back(50);
      currNum += 11;
      break;
    default:
      break;
    }
  }
}

void CharString::printNums(int start, int end) const {
  std::cout << '(';
  for (auto num : nums){
    std::cout << "num" << ' ';
  }
  std::cout << ')';
}

void CharString::printOpers(bool withNums) const {
  if (withNums){
    for (auto oper : opers){
      std::cout << oper.name << " ";
      for (int i = 0; i < oper.numCount; i++){
        std::cout << nums[i] << " ";
      }
    }
  }
  else{
    for (auto oper : opers){
      std::cout << oper.name << " ";
    }
  }
  std::cout << std::endl;
}