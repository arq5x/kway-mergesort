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

  std::string inFile = "..\\..\\numeric.txt";
  // allow the sorter to use 100Kb (base 10) of memory for sorting.
  int  bufferSize = 1024 * 16;
  // once full, it will dump to a temp file and grab another chunk. 

  // not yet supported
  bool compressOutput = false;
  // allows you to write the intermediate files anywhere want.
  std::string tempPath = "..\\..\\temp\\";

  // sort the lines of a file lexicographically in ascending order
  KwayMergeSort<uint32_t> *sorter = new KwayMergeSort<uint32_t>(inFile,
                                                                &cout,
                                                                Uint32_t,
                                                                bufferSize,
                                                                compressOutput,
                                                                tempPath);
  sorter->Sort();
}
