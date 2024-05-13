#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

template <typename T>
void printV_HEX(vector<T> inVec){
    for (auto a: inVec){
      printf("%x", a);
    }
}

int main(){

  ifstream FILE("test.otf", ios::binary);
  char inbuffer[256];
  int buf_size = 256;
  char c;
  vector<uint32_t> contents;
  int offset = 0;

  if ( FILE.is_open() ){
    while ( FILE.good() ){
      c = FILE.get();
      contents.push_back(c);
    }
  }

  //collect sfnt version
  vector<uint32_t> sfntVersion(contents.begin(), contents.begin() + 4);
  offset += 4;

  printV_HEX(contents);

}