#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
using namespace std;

// local includes
#include "kwaymergesort.h"

// comparison functions for the ext. merge sort	
bool alphaAsc(const std::string &a, const std::string &b) { return a < b; }

int main(int argc, char* argv[]) {

  std::string inFile = "..\\..\\english.txt";
  // allow the sorter to use 100Kb (base 10) of memory for sorting.
  int  bufferSize = 100000;
  // once full, it will dump to a temp file and grab another chunk. 

  // not yet supported
  bool compressOutput = false;
  // allows you to write the intermediate files anywhere want.
  std::string tempPath = "..\\..\\temp\\";

  // sort the lines of a file lexicographically in ascending order
  KwayMergeSort<std::string> *sorter = new KwayMergeSort<std::string>(inFile,
                                                                      &std::cout,
                                                                      alphaAsc,
                                                                      bufferSize,
                                                                      compressOutput,
                                                                      tempPath);
  sorter->Sort();
}
