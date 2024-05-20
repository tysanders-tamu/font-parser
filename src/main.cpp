#include <iostream>
#include <fstream>
#include <vector>

#include <fmt/core.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <GLFW/glfw3.h>
#include "parserClass.hpp"

using namespace std;

int main() {
  //substantiate class
  string filename = "test2.otf";
  parserClass parser(filename, true);

  cout << "\n";
  // parser.print_table_records();
  
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