#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <bitset>
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

//exact sizing is irrelevant as we are not using the struct to read
struct TopDict {
  string version;
  string Notice;
  string Copyright;
  string FullName;
  string FamilyName;
  string Weight;
  bool isFixedPitch = 0;
  int ItalicAngle = 0;
  int UnderlinePosition = -100;
  int UnderlineThickness = 50;
  int PaintType = 0;
  int CharstringType = 2;
  vector<double> FontMatrix = {0.001, 0, 0, 0.001, 0, 0};
  int uniqueID;
  vector<int> XUID;
  int charset = 0; //offset
  int Encoding = 0; //offset
  int CharString; //offset to charstrings
  int Private[2] = {0, 0}; //private dict size and offset
  int SyntheticBase;
  string PostScript;
  string BaseFontName;
  string BaseFontBlend; 
};

// CFF 1-byte operators
const string cff_operators_onebyte[31] = {
  "version",
  "Notice",
  "FullName",
  "FamilyName",
  "Weight",
  "FontBBox",
  "BlueValues",
  "OtherBlues",
  "FamilyBlues",
  "FamilyOtherBlues",
  // 10
  "StdHW",
  "StdVW",
  "escape", // first byte of a two byte operator
  "UniqueID",
  "XUID",
  "charset",
  "Encoding",
  "CharStrings",
  "Private",
  "Subrs",
  // 20
  "defaultWidthX",
  "nominalWidthX",
  "", // reserved
  "", // reserved
  "", // reserved
  "", // reserved
  "", // reserved
  "", // reserved
  "", // shortint
  "", // longint
  // 30
  "BCD"
};

//CFF 2-byte operators (all start with 12, so we drop it)
const string cff_operators_twobyte[39] = {
  // 0
  "Copyright",
  "isFixedPitch",
  "ItalicAngle",
  "UnderlinePosition",
  "UnderlineThickness",
  "PaintType",
  "CharstringType",
  "FontMatrix",
  "StrokeWidth",
  "BlueScale",
  // 10
  "BlueShift",
  "BlueFuzz",
  "StemSnapH",
  "StemSnapV",
  "ForceBold",
  "", // reserved
  "", // reserved
  "LanguageGroup",
  "ExpansionFactor",
  "initialRandomSeed",
  // 20
  "SyntheticBase",
  "PostScript",
  "BaseFontName",
  "BaseFontBlend",
  "", // reserved
  "", // reserved
  "", // reserved
  "", // reserved
  "", // reserved
  "", // reserved
  // 30
  "ROS",
  "CIDFontVersion",
  "CIDFontRevision",
  "CIDFontType",
  "CIDCount",
  "UIDBase",
  "FDArray",
  "FDSelect",
  "FontName"
};


class parserClass
{
  //objects
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
    CFFIndex charStringIndex;    
    //Dict objects
    vector<pair<int, vector<int>>> decoded_dict_data;
    TopDict top_dict;

  //methods
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
    void populate_CFF_Index(CFFIndex& index, bool do_hex);
    //direct file manipilation
    uint8_t read_uint8_t();
    vector<uint8_t> read_index_data(uint32_t data_count);
    uint16_t read_uint16_t();
    uint32_t read_uint24_t();
    uint32_t read_uint32_t();
    void decode_dict_data(vector<uint8_t> data);

};