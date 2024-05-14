#define FMT_HEADER_ONLY

#include <iostream>
#include <fstream>
#include <vector>
#include <fmt/core.h>
#include <climits>

using namespace std;

template <typename T>
void printV_HEX(vector<T> inVec){
    for (auto a: inVec){
      printf("%x", a);
    }
    cout << endl;
}

//replace
template <typename T>
T swap_endian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

struct tableDirectory
{
  uint32_t sfntVersion;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;
};

uint32_t readBigEndianUint32(ifstream& file){
  uint32_t value;
  file.read( reinterpret_cast<char*>(&value), sizeof(uint32_t));
  if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__){
    value = swap_endian<uint32_t>(value);
  }
  return value;
}

uint16_t readBigEndianUint16(ifstream& file){
  uint16_t value;
  file.read( reinterpret_cast<char*>(&value), sizeof(uint16_t));
  if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__){
    reverse(reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(uint16_t));
  }
  return value;
}

void printFile256(ifstream& file){
  unsigned char c;
  for (int i = 0; i < 256; i++){
    file.read(reinterpret_cast<char*>(&c), sizeof(unsigned char));
    printf("%02x ", c);
  }
  cout << endl;
}


tableDirectory readTableDirectory(ifstream& file){
  file.seekg(0);
  tableDirectory dir;
  dir.sfntVersion = readBigEndianUint32(file);
  dir.numTables = readBigEndianUint16(file);
}

int main(){

  ifstream file("test.otf", ios::binary);
  if (!file.is_open()) {
      cerr << "Error opening file!" << endl;
      return 1;
  }
  

  // tableDirectory dir = readTableDirectory(file);

  // cout << "SFNT version " << hex << dir.sfntVersion << endl;
  // cout << "Num tables " << dir.numTables << endl;

  // fmt::print("sfntVersion: {}\n", sfntVersion_char);
  printFile256(file);

  file.close();

}