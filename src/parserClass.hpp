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

struct CFFHeader {
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint8_t headerSize;
    uint8_t offSize;
};

struct CFFIndex {
    uint16_t count;
    uint8_t offSize;
    //offset size varies based on offSize, so assume largest
    vector<uint32_t> offsets;
    vector<uint8_t> data;
};


class parserClass
{
  private:
    bool is_open;
    bool debug = false;
    ifstream file;
    string filename;
    //structs
    TableDirectory tableDirectory;
    TableRecords tableRecords;
    //CFF specific
    CFFHeader cffHeader;
    CFFIndex nameIndex;
    CFFIndex topDictIndex;
    CFFIndex stringIndex;
    CFFIndex globalSubrIndex;     
  public:
  // constructors
    parserClass(string file);
    parserClass(string file, bool debug);
    ~parserClass();
  //
    void print_table_records();
    void hex_to_ascii(uint32_t hex);
    void hex_to_ascii(uint8_t hex);
    void print_hex_data(uint32_t offset, uint32_t length);
    //! private functions
    private:
    void read_table_directory();
    void read_table_records();
    void standard_flow();
    void read_CFF_header();
    void read_CFF_indexes();
    void populate_CFF_Index(CFFIndex& index);
    //direct file manipilation
    uint8_t read_uint8_t();
    vector<uint8_t> read_index_data(uint32_t data_count);
    uint16_t read_uint16_t();
    uint32_t read_uint24_t();
    uint32_t read_uint32_t();

  
};