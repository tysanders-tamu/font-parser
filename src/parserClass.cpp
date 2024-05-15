#include "parserClass.hpp"

parserClass::parserClass(string filename) : filename(filename)
{
  file = ifstream(filename, ios::binary);
  is_open = file.is_open();

  //std flow
  read_table_directory();
  read_table_records();
}


parserClass::~parserClass()
{
  if (is_open) file.close();
}

void parserClass::read_table_directory(){
  if (!is_open) {
    cerr << "cannot use unopened file!" << endl;
    return;
  }

  tableDirectory.sfntVersion   = read_uint32_t();
  //16 bit
  tableDirectory.numTables     = read_uint16_t();
  tableDirectory.searchRange   = read_uint16_t();
  tableDirectory.entrySelector = read_uint16_t();
  tableDirectory.rangeShift    = read_uint16_t();
}

void parserClass::read_table_records(){
  if (!is_open) {
    cerr << "cannot use unopened file!" << endl;
    return;
  }

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
    fmt::print("Tag: {}\n", str);
}




uint16_t parserClass::read_uint16_t(){
  uint16_t value;
  file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
  return htons(value);
}

uint32_t parserClass::read_uint32_t(){
  uint32_t value;
  file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
  return htonl(value);
}