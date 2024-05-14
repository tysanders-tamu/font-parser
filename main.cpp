#define FMT_HEADER_ONLY

#include <iostream>
#include <fstream>
#include <vector>
#include <fmt/core.h>

using namespace std;

template <typename T>
void printV_HEX(vector<T> inVec){
    for (auto a: inVec){
      printf("%x", a);
    }
    cout << endl;
}

uint16_t readUInt16(vector<u_int8_t> &contents){
  uint16_t result;
  fmt::print("A: {}\n", static_cast<char>(contents.at(0)));
  fmt::print("B: {}\n", static_cast<char>(contents.at(1)));

  for (int i = 0; i < 8; i++){
    fmt::print("Aa: {}\n", *(contents.begin() + i));
  }

  copy(contents.begin(), contents.begin()+1, &result);
  //print out
  fmt::print("result: {}\n", result);
}

int main(){

  ifstream FILE("test.otf", ios::binary);
  char inbuffer[256];
  int buf_size = 256;
  char c;
  vector<u_int8_t> contents;

  if ( FILE.is_open() ){
    while ( FILE.good() ){
      c = FILE.get();
      contents.push_back(c);
    }
  }


  // fmt::print("sfntVersion: {}\n", sfntVersion_char);
  printV_HEX(contents);

}