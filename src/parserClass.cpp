#include "parserClass.hpp"

parserClass::parserClass(string filename) : filename(filename){standard_flow();}
parserClass::parserClass(string filename, bool debug) : filename(filename), debug(debug){standard_flow();}


void parserClass::standard_flow(){
  file = ifstream(filename, ios::binary);
  is_open = file.is_open();

  if (!is_open) {
    cerr << "Error opening file!" << endl;
    return;
  }


  read_table_directory();
  read_table_records();
  read_CFF_header();
  read_CFF_indexes();
    //operate on indexs
  decode_dict_data(topDictIndex.data);

  //skip down to charstrings
  //find charstring offset in decode_dict_data
  int charstring_offset = 0;
  for (auto group : decoded_dict_data){
    if (group.first == 17){
      charstring_offset = group.second[0];
      break;
    }
  }
  
  file.seekg(charstring_offset, ios::ios_base::beg);

  //read charstring data
  fmt::print(fg(fmt::color::green), "==========charStringIndex==========\n");
  populate_CFF_Index(charStringIndex);



}

parserClass::~parserClass()
{
  if (is_open) file.close();
}

/*
* Print hex data is non-destructive and it will not change the file position
*/
void parserClass::print_hex_data(uint32_t offset, uint32_t length) {
  int _filepos = file.tellg();
  file.seekg(0, ios_base::beg);
  file.seekg(offset, ios_base::beg);

  unsigned char c;
  for (int i = 0; i < length; i++) {
      file.read(reinterpret_cast<char*>(&c), sizeof(unsigned char));
      fmt::print("{:x} ", c);
  }
  cout << endl;

  file.seekg(_filepos, ios_base::beg);
}

void parserClass::read_table_directory(){

  tableDirectory.sfntVersion   = read_uint32_t();
  //16 bit
  tableDirectory.numTables     = read_uint16_t();
  tableDirectory.searchRange   = read_uint16_t();
  tableDirectory.entrySelector = read_uint16_t();
  tableDirectory.rangeShift    = read_uint16_t();
}

void parserClass::read_table_records(){

  file.seekg(0);
  file.seekg(sizeof(TableDirectory));

  for (int i = 0; i < tableDirectory.numTables; i++){
    TableRecord record;
    record.tag      = read_uint32_t();
    record.checkSum = read_uint32_t();
    record.offset   = read_uint32_t();
    record.length   = read_uint32_t();
      //push into vector
    tableRecords.records.push_back(record);
  }
}

//CFF specific
/////////////////////////////////////////////////////
void parserClass::read_CFF_header(){

  file.seekg(0);
  //find offset
  for (auto record : tableRecords.records){
    if (record.tag == 0x43464620){
      file.seekg(record.offset);
      break;
    }
  }

  //read header Card8 == uint8_t
  cffHeader.majorVersion = read_uint8_t();
  cffHeader.minorVersion = read_uint8_t();
  cffHeader.headerSize   = read_uint8_t();
  cffHeader.offSize      = read_uint8_t();

  if (debug){
    fmt::print("-------CFF Header-----------\n");
    fmt::print("majorVersion: {}\n", cffHeader.majorVersion);
    fmt::print("minorVersion: {}\n", cffHeader.minorVersion);
    fmt::print("headerSize: {}\n", cffHeader.headerSize);
    fmt::print("offSize: {}\n", cffHeader.offSize);
    fmt::print("----------------------------\n");
  }
  
}

//helper to decode_dict_data
int opsize_to_opamt(uint8_t opsize){
  if (opsize >= 32 && opsize <= 246){return 1;}
  if (opsize >= 247 && opsize <= 254){return 2;}
  if (opsize == 28){return 3;}
  if (opsize == 29){return 5;}
  //if not a valid first num
  return 0;
}

int decode_operand(int b0){
  return (b0-139);
}

int decode_operand(int b0, int b1){
  if (b0 >= 247 && b0 <= 250){
    return ((b0-247)*256 + b1 + 108);
  }

  if (b0 >= 251 && b0 <= 254){
    return (-(b0-247)*256 - b1 - 108);
  }
}

//b0 == 28
int decode_operand(int b0, int b1, int b2){
  return (b1 << 8|b2);
}

//b1 == 29
int decode_operand(int b0, int b1, int b2, int b3, int b4){
  return (b1 << 24|b2 << 16|b3 << 8|b4);
}

//Dict data
void parserClass::decode_dict_data(vector<uint8_t> data){
  vector<int> _unit_calculated_values;

  int i = 0;
  while(1){
    //read first value in array, get size, read that much
    switch (opsize_to_opamt(data[i]))
    {
      case 1:
        _unit_calculated_values.push_back(decode_operand(data[i]));
        i += 1;
        break;
      case 2:
        _unit_calculated_values.push_back(decode_operand(data[i], data[i+1]));
        i += 2;
        break;
      case 3:
        _unit_calculated_values.push_back(decode_operand(data[i], data[i+1], data[i+2]));
        i += 3;
        break;
      case 5:
        _unit_calculated_values.push_back(decode_operand(data[i], data[i+1], data[i+2], data[i+3], data[i+4]));
        i += 5;
        break;
    }

    //after reading data, check for operators, if none, read another chunk
    if (data[i] > 31 || data[i] == 28 || data[i] == 29) continue;
    pair<int, vector<int>> _unit_instruction;
    //use two byte operator?
    if (data[i] == 12){
      i++;
      //construct pair
      _unit_instruction.first = data[i] + 256;
    }

    //if not a two byte
    _unit_instruction.first = data[i];
    _unit_instruction.second = _unit_calculated_values;

    //push the pair into decoded_dict_data vector
    decoded_dict_data.push_back(_unit_instruction);
    i++;
    //empty out operand vector
    _unit_calculated_values = {};
    //simple break instruction
    if (i >= data.size()){
      break;
    }
  }

  //debug print outof decoded_dict_data
  if (debug){
    fmt::print("-------data grouping of dict:-------\n");
    for (auto group : decoded_dict_data){
      //pre process
      if (group.first >= 256){
        fmt::print(fg(fmt::color::orange), "{} ", cff_operators_twobyte[group.first - 256]);
      } else {
        fmt::print(fg(fmt::color::orange), "{} ", cff_operators_onebyte[group.first]);
      }
      for (auto value : group.second){
        fmt::print(fg(fmt::color::azure), "{} ", value);
      }
    }
    fmt::print("\n------------------------------------\n");
  }

}

void parserClass::populate_CFF_Index(CFFIndex& index){
  index.count   = read_uint16_t();
  //offsize can be 1, 2, 3, or 4 (num of bytes per offset)
  //? plus abs offset from header?
  index.offSize = read_uint8_t();

  if (debug){
    fmt::print("count: {}\n", index.count);
    fmt::print("offSize: {}\n", index.offSize);
  }

  //get offset array
  for (int i = 0; i < index.count + 1; i++){
    switch (index.offSize)
    {
    case 1:
      index.offsets.push_back(read_uint8_t());
      break;
    case 2:
      index.offsets.push_back(read_uint16_t());
      break;
    case 3:
      index.offsets.push_back(read_uint24_t());
      break;
    case 4:
      index.offsets.push_back(read_uint32_t());
      break;
    }
  }

  vector<uint8_t> single_read_data;
  int start_post = (int)file.tellg() - 1;

  //store data in index
  for (int i = 0; i < index.count; i++){
    file.seekg(start_post + index.offsets[i], ios_base::beg);
    single_read_data = read_index_data(index.offsets[i + 1] - index.offsets[i]);
    index.data.insert(index.data.end(), single_read_data.begin(), single_read_data.end());
  }

  //TODO: add a way to track size of each element without having to query the offset arr
  if (debug) {
    fmt::print("-------index data:-------\n");
    for (int i = 0; i < index.data.size(); i++){
      //print out chunk together
        fmt::print("{:#c}", index.data[i]);
    }
    fmt::print("\n-------------------------\n");
  }
}

void parserClass::read_CFF_indexes(){

  file.seekg(0);
  //find offset
  for (auto record : tableRecords.records){
    if (record.tag == 0x43464620){
      file.seekg(record.offset);
      break;
    }
  }

///////////////////// end of cff specific //////////////////////

  file.seekg(cffHeader.headerSize, ios_base::cur);
  int bottom_of_cff_header_abs = file.tellg();
  //name, topDict, string, globalSubr
  fmt::print(fg(fmt::color::green), "=============nameIndex============\n");
  populate_CFF_Index(nameIndex);
  // fmt::print("file location: " + to_string(file.tellg()) + "\n");
  fmt::print(fg(fmt::color::green), "===========topDictIndex===========\n");
  populate_CFF_Index(topDictIndex);
    fmt::print("nameIndex.count: {}\n", nameIndex.count);

  if (debug){
    for (int i = 0; i < nameIndex.count + 1; i++){
      fmt::print("dict.offsets[{}]: {:#x}\n", i, topDictIndex.offsets[i]);
    }
  }

  fmt::print(fg(fmt::color::green), "============stringIndex===========\n");
  populate_CFF_Index(stringIndex);
  //! curr causing segfault
  fmt::print(fg(fmt::color::green),"==========globalSubrIndex==========\n");
  populate_CFF_Index(globalSubrIndex);

  //decode dict data
  fmt::print(fg(fmt::color::green), "=============dict data============\n");
}

////////////////////////////////////////////////////////////

void parserClass::print_table_records(){
  for (const auto& record : tableRecords.records) {
    fmt::print("-------------------------\n");
    hex_to_ascii(record.tag);
    fmt::print("tag hex: {:#x}\n", record.tag);
    fmt::print("CheckSum: {:#x}\n", record.checkSum);
    fmt::print("Offset: {:#x}\n", record.offset);
    fmt::print("Length: {:#x}\n", record.length);
  }
  fmt::print("-------------------------\n");
}

void parserClass::hex_to_ascii(uint32_t hex) {
    char str[11];
    sprintf(str, "%c%c%c%c", hex >> 24, hex >> 16, hex >> 8, hex);
    fmt::print(str);
}

void parserClass::hex_to_ascii(uint8_t hex) {
    char str[11];
    sprintf(str, "%c%c", hex >> 8, hex);
    fmt::print(str);
}


uint8_t parserClass::read_uint8_t(){
  uint8_t value;
  file.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));
  return value;
}

vector<uint8_t> parserClass::read_index_data(uint32_t data_count){
  vector<uint8_t> data;
  for (int i = 0; i < data_count; i++){
    data.push_back(read_uint8_t());
  }
  return data;
}

uint16_t parserClass::read_uint16_t(){
  uint16_t value;
  file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
  return htons(value);
}

//TODO: TEST
uint32_t parserClass::read_uint24_t(){
  uint32_t value;
  file.read(reinterpret_cast<char*>(&value), 3);
  return htonl(value);
}

uint32_t parserClass::read_uint32_t(){
  uint32_t value;
  file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
  return htonl(value);
}