#include <iostream>
#include <fstream>
#include <vector>
#include <fmt/core.h>
#include <arpa/inet.h>

using namespace std;

// Define a struct to represent the table directory
struct TableDirectory {
    uint32_t sfntVersion;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
};

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

int main() {
    ifstream file("test.otf", ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Read the table directory
    TableDirectory dir = readTableDirectory(file);

    // Print the SFNT version and the number of tables
    printTableDirectory(dir);

    // Print the first 256 bytes of the file as hexadecimal
    printFile256(file);

    file.close();
    return 0;
}
