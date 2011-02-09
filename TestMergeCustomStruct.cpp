#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
using namespace std;

// local includes
#include "kwaymergesort.h"

// a basic struct for a BED entry.
struct BED {
    string chrom;
    unsigned int start;
    unsigned int end;
    
    bool operator < (const BED &b) const
    {
        if      (chrom < b.chrom)  return true;
        else if (chrom > b.chrom)  return false;
        // we get here when chroms are the same. now sort on starts
        if      (start < b.start)  return true;
        else if (start >= b.start) return false;
    }
    
    // overload the << operator for writing a BED struct
    friend ostream& operator<<(ostream &os, const BED &b) 
    {
        os  << b.chrom  << "\t" 
            << b.start  << "\t" 
            << b.end;
        return os;
    }
    // overload the >> operator for reading into a BED struct    
    friend istream& operator>>(istream &is, BED &b) 
    {
        is  >> b.chrom 
            >> b.start  
            >> b.end;
        return is;
    }    
};


// comparison function for sorting by chromosome, then by start.
bool bySize(BED const &a, BED const &b) {
    return (a.end - a.start) < (b.end - b.start);
}


int main(int argc, char* argv[]) {

    string inFile       = argv[1];
    int  bufferSize     = 100000;      // allow the sorter to use 100Kb (base 10) of memory for sorting.  
                                       // once full, it will dump to a temp file and grab another chunk.     
    bool compressOutput = false;       // not yet supported
    string tempPath     = "./";        // allows you to write the intermediate files anywhere you want.
    
    // sort a BED file by chrom then start
    KwayMergeSort<BED> *bed_sorter = new KwayMergeSort<BED> (inFile, 
                                                            &cout, 
                                                            bufferSize, 
                                                            compressOutput, 
                                                            tempPath);
                                                            
    cout << "First sort by chrom, then start using the overloaded \"<\" operator\n";
    bed_sorter->Sort();
    cout << "Now, sort by size using a custom function (bySize)\n";
    bed_sorter->SetComparison(bySize);
    bed_sorter->Sort();
    

    // sort a BED file by chrom then start
    KwayMergeSort<BED> *bed_sorter_custom = new KwayMergeSort<BED> (inFile, 
                                                                    &cout,
                                                                    bySize, 
                                                                    bufferSize, 
                                                                    compressOutput, 
                                                                    tempPath);
    cout << "Now create a new class with bySize() as the custom sort function\n";
    bed_sorter_custom->Sort();
}