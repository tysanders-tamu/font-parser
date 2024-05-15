#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <fmt/core.h>
using namespace std;

// Define a struct to represent the table directory
struct TableDirectory {
    uint32_t sfntVersion;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
};


  //TABLE RECORDS
struct TableRecord {
    uint32_t tag;
    uint32_t checkSum;
    uint32_t offset;  // offset TYPE
    uint32_t length;
};

struct TableRecords {
    vector<TableRecord> records;
};


  //CMAP Specific
struct encodingRecord {
    uint16_t platformID;
    uint16_t encodingID;
    uint32_t offset;  // offset TYPE
};

struct cmapHeader {
    uint16_t version;
    uint16_t numTables;
    vector<encodingRecord> encodingRecords;
};


class parserClass
{
  private:
  public:
    bool is_open;
    string filename;
    ifstream file;

    //structs
    TableDirectory tableDirectory;
    TableRecords tableRecords;
  public:
  // constructors
    parserClass(string file);
    ~parserClass();
  //
    void print_table_records();
    void hex_to_ascii(uint32_t hex);
  //! private functions
  private:
    void read_table_directory();
    void read_table_records();
    //direct file manipilation
    uint16_t read_uint16_t();
    uint32_t read_uint32_t();

  
};