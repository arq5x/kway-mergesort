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
bool alphaAsc(const string &a, const string &b) { return a < b; }

int main(int argc, char* argv[]) {

    string inFile       = argv[1];
    int  bufferSize     = 100000;      // allow the sorter to use 100Kb (base 10) of memory for sorting.  
                                       // once full, it will dump to a temp file and grab another chunk.     
    bool compressOutput = false;       // not yet supported
    string tempPath     = "./";        // allows you to write the intermediate files anywhere you want.
    
    // sort the lines of a file lexicographically in ascending order (akin to UNIX sort, "sort FILE")
    KwayMergeSort<string> *sorter = new KwayMergeSort<string> (inFile, &cout, alphaAsc, bufferSize, compressOutput, tempPath);
    sorter->Sort();
}
