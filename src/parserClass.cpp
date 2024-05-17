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
      printf("%c ", c);
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

void parserClass::populate_CFF_Index(CFFIndex& index){
  index.count   = read_uint16_t();
  //offsize can be 1, 2, 3, or 4 (num of bytes per offset)
  //? plus abs offset from header?
  index.offSize = read_uint8_t();

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

  file.seekg(cffHeader.headerSize, ios_base::cur);
  //name, topDict, string, globalSubr
  populate_CFF_Index(nameIndex);
  populate_CFF_Index(topDictIndex);
  populate_CFF_Index(stringIndex);
  populate_CFF_Index(globalSubrIndex);
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