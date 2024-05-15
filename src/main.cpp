#include <iostream>
#include <fstream>
#include <vector>
#include <fmt/core.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <GLFW/glfw3.h>
#include "parserClass.hpp"

using namespace std;

// hex to ascii printout
void hex_to_ascii(uint32_t hex) {
    char str[11];
    sprintf(str, "%c%c%c%c", hex >> 24, hex >> 16, hex >> 8, hex);
    fmt::print("Tag: {}\n", str);
}

// Function to read a 32-bit unsigned integer from the file
uint32_t readUint32(ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return htonl(value);
}

// Function to read a 16-bit unsigned integer from the file
uint16_t readUint16(ifstream& file) {
    uint16_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    return htons(value);
}

// Function to read the table directory from the file
TableDirectory readTableDirectory(ifstream& file) {
    TableDirectory dir;
    dir.sfntVersion = readUint32(file);
    dir.numTables = readUint16(file);
    dir.searchRange = readUint16(file);
    dir.entrySelector = readUint16(file);
    dir.rangeShift = readUint16(file);
    return dir;
}

TableRecords readTableRecords(ifstream& file, const TableDirectory& dir) {
    TableRecords records;
    //seek forward size of TableDirectory
    file.seekg(sizeof(TableDirectory));

    for (int i = 0; i < dir.numTables; i++) {
        TableRecord record;
        record.tag = readUint32(file);
        record.checkSum = readUint32(file);
        record.offset = readUint32(file);
        record.length = readUint32(file);
        //push into vector
        records.records.push_back(record);
    }
    return records;
}

cmapHeader readCmapHeader(ifstream& file, uint32_t offset) {
    file.seekg(0); //offset from beginning of file
    file.seekg(offset);
    cmapHeader header;
    header.version = readUint16(file);
    header.numTables = readUint16(file);
    for (int i = 0; i < header.numTables; i++) {
        encodingRecord record;
        record.platformID = readUint16(file);
        record.encodingID = readUint16(file);
        record.offset = readUint32(file);
        header.encodingRecords.push_back(record);
    }
    return header;
}

// Function to print the first 256 bytes of the file as hexadecimal
void printFile256(ifstream& file) {
    file.seekg(0); // Seek to the beginning of the file
    unsigned char c;
    for (int i = 0; i < 256; i++) {
        file.read(reinterpret_cast<char*>(&c), sizeof(unsigned char));
        printf("%02x ", c);
    }
    cout << endl;
}

void printTableDirectory(const TableDirectory& dir) {
    fmt::print("SFNT version: {:#x}\n", dir.sfntVersion);
    fmt::print("Num tables: {}\n", dir.numTables);
    fmt::print("Search range: {}\n", dir.searchRange);
    fmt::print("Entry selector: {}\n", dir.entrySelector);
    fmt::print("Range shift: {}\n", dir.rangeShift);
}

void printTableRecords(const TableRecords& records) {
    for (const auto& record : records.records) {
        fmt::print("-------------------------\n");
        hex_to_ascii(record.tag);
        fmt::print("tag hex: {:#x}\n", record.tag);
        fmt::print("CheckSum: {:#x}\n", record.checkSum);
        fmt::print("Offset: {:#x}\n", record.offset);
        fmt::print("Length: {:#x}\n", record.length);
    }
    fmt::print("-------------------------\n");
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  //substantiate class
  string filename = "test.otf";
  parserClass parser(filename);
  parser.print_table_records();
  cout << parser.tableDirectory.sfntVersion << endl;
  cout << parser.tableDirectory.numTables << endl;
  cout << parser.tableDirectory.searchRange << endl;
  cout << parser.tableDirectory.entrySelector << endl;
  cout << parser.tableDirectory.rangeShift << endl;
  
    // ifstream file("Helvetica.otf", ios::binary);
    // if (!file.is_open()) {
    //     cerr << "Error opening file!" << endl;
    //     return 1;
    // }

    // // Read the table directory
    // TableDirectory dir = readTableDirectory(file);
    // TableRecords records = readTableRecords(file, dir);

    // // Print the SFNT version and the number of tables
    // printTableDirectory(dir);
    // printTableRecords(records);

    // //get cmap
    // //get record entry for cmap
    // TableRecord cmapRecord;
    // //find relevant entry
    // for (const auto& record : records.records) {
    //     if (record.tag == 0x636d6170) {
    //         cmapRecord = record;
    //         break;
    //     }
    // }

    // cmapHeader cmapHeader = readCmapHeader(file, cmapRecord.offset);
    // cout << "Cmap Header: " << cmapHeader.version << endl;
    // cout << "Cmap Header: " << cmapHeader.numTables << endl;
    // for (int i = 0; i < cmapHeader.numTables; i++) {
    //     fmt::print("-------------------------\n");  
    //     cout << "Platform ID: " << cmapHeader.encodingRecords[i].platformID << endl;
    //     cout << "Encoding ID: " << cmapHeader.encodingRecords[i].encodingID << endl;
    //     cout << "Offset: " << cmapHeader.encodingRecords[i].offset << endl;
    // }

    // file.close();


    // // glfw testing
    // if (!glfwInit())
    //   {
    //     fmt::print("Failed to initialize GLFW\n");
    //   }

    
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    

    // GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    // if (!window)
    //   {
    //     fmt::print("Failed to create window\n");
    //     glfwTerminate();
    //   }


    // // /////////////////////////////////
    // // // OpenGL setup
    // // glViewport(0, 0, 640, 480);
    // // /////////////////////////////////

    // glfwSetKeyCallback(window, key_callback);
    // glfwMakeContextCurrent(window);

    // // while (!glfwWindowShouldClose(window))
    // //   {
    // //       glfwPollEvents();
    // //   }

    // glfwDestroyWindow(window);
    // glfwTerminate();
    // return 0;
}



// w? {hs* vs* cm* hm* mt subpath}? {mt subpath}* endchar