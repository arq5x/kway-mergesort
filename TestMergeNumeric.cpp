#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>

// local includes
#include "kwaymergesort.h"

// comparison functions for the ext. merge sort	
bool Uint32_t(const uint32_t &a, const uint32_t &b) { return a < b; }

int main(int argc, char* argv[]) {

  std::string inFile = "..\\..\\numeric_small.txt";
  // allow the sorter to use 100Kb (base 10) of memory for sorting.
  int  bufferSize = 100 * 1024;
  // once full, it will dump to a temp file and grab another chunk. 

  // not yet supported
  bool compressOutput = false;
  // allows you to write the intermediate files anywhere want.
  std::string tempPath = "..\\..\\temp\\";


  std::cout << "1. Passing \'&std::cout\' as \'ostream *out\'" << std::endl;
  {
    bufferSize = 20;
    std::cout << "1.1. Using a small maxBufferSize" << std::endl;
    {
      std::cout
        << "1.1.1. Using Sort method: merged output is "
        << "\tsent to ostream *out -> in this case, std::cout" << std::endl;
      {
        // sort the lines of a file lexicographically in ascending order
        KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                      &std::cout,
                                                                      Uint32_t,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
        // sorted data is sent to ostream *out -> in this case, std::cout
        sorter->Sort(); 
      }std::cout << std::endl;
      
      std::cout << "1.1.2. Using ranged-based for loop over KwayMergeSort:" << std::endl
        << "\tEach element of the output is obtained step by step" << std::endl
        << "\t\t-> Allows for a user/client operation over each one of the elements, " << std::endl
        << "\t\tinstead of processing them all together " << std::endl;
      {
        // sort the lines of a file lexicographically in ascending order
        KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                      &std::cout,
                                                                      Uint32_t,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
        // sorted data is sent to ostream *out -> in this case, std::cout
        for (const auto & value : *sorter) {
          //Perform operations over each value, ex.:
          //UserFunction(value);
        }
      }std::cout << std::endl;
    }std::cout << std::endl;

    bufferSize = 1024 * 16;
    std::cout << "1.2. Using a large maxBufferSize" << std::endl;
    {
      std::cout
        << "1.2.1. Using Sort method: merged output is "
        << "\tsent to ostream *out -> in this case, std::cout" << std::endl;
      {
        // sort the lines of a file lexicographically in ascending order
        KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                      &std::cout,
                                                                      Uint32_t,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
        // sorted data is sent to ostream *out -> in this case, std::cout
        sorter->Sort();
      }std::cout << std::endl;
      std::cout << "1.2.2. Using ranged-based for loop over KwayMergeSort:" << std::endl
        << "\tEach element of the output is obtained step by step" << std::endl
        << "\t\t-> Allows for a user/client operation over each one of the elements, " << std::endl
        << "\t\tinstead of processing them all together " << std::endl;
      {
        // sort the lines of a file lexicographically in ascending order
        KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                      &std::cout,
                                                                      Uint32_t,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
        // sorted data is sent to ostream *out -> in this case, std::cout
        for (const auto & value : *sorter) {
          //Perform operations over each value, ex.:
          //UserFunction(value);
        }
      }std::cout << std::endl;
    }std::cout << std::endl;
  }std::cout << std::endl;

  std::cout << "2. Passing \'nullptr' as \'ostream *out\'" << std::endl;
  {
    bufferSize = 20;
    std::cout << "2.1. Using a small maxBufferSize" << std::endl;
    {  
      std::cout << "2.1.2. Using ranged-based for loop over KwayMergeSort:" << std::endl
        << "\tEach element of the output is obtained step by step" << std::endl
        << "\t\t-> Allows for a user/client operation over each one of the elements, " << std::endl
        << "\t\tinstead of processing them all together " << std::endl;
      {
        // sort the lines of a file lexicographically in ascending order
        KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                      nullptr,
                                                                      Uint32_t,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
        // sorted data is not sent to ostream *out, since in this case, it is nullptr 
        // User must operate over each value and output the values if desires
        for (const auto & value : *sorter) {
          std::cout << value << std::endl;
        }
      }std::cout << std::endl;
    }std::cout << std::endl;

    bufferSize = 1024 * 16;
    std::cout << "2.2.1. Using a large maxBufferSize" << std::endl;
    {      
      std::cout << "2.2.2. Using ranged-based for loop over KwayMergeSort:" << std::endl
        << "\tEach element of the output is obtained step by step" << std::endl
        << "\t\t-> Allows for a user/client operation over each one of the elements, " << std::endl
        << "\t\tinstead of processing them all together " << std::endl;
      {
        // sort the lines of a file lexicographically in ascending order
        KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                      nullptr,
                                                                      Uint32_t,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
        // sorted data is not sent to ostream *out, since in this case, it is nullptr 
        // User must operate over each value and output the values if desires
        for (const auto & value : *sorter) {
          std::cout << value << std::endl;
        }
      }std::cout << std::endl;
    }std::cout << std::endl;
  }
}
