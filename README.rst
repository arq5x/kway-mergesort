:Project: kway-mergesort
:Version: 0.1.0
:Authors: - Aaron Quinlan, CPHG, University of Virginia
:Website: http://cphg.virginia.edu/quinlan
:Contact: arq5x@virginia.edu


.. contents::

======================================================================
kway-mergesort - A lightweight, templated C++ API for external sorting
======================================================================

Often times, we run into cases where we need to sort extremely large files from within a piece of software. While it is certainly possible to make system calls to the UNIX "sort" command, it is much more flexible (and convenient) to have a nice API that will handle all of the external sorting within the constraints of the memory available.

There's a huge literature in Computer Science on this topic, and the most general approach to sorting files that are too big to fit in memory is the "merge sort" (e.g., http://www.cprogramming.com/tutorial/computersciencetheory/mergesort.html). 

A more advanced approach is to do what is known as a k-way, memory-assisted merge sort.  Despite the mouthful, it is pretty straight forward.  Let's assume you have a 1 terabyte file of strings, one per line.  Your computer only has 16Gb of RAM.  You want to sort the 1 Tb file as quickly as possible.  Kway merge sort would tackle this as follows

1. Read 16Gb worth of data.
2. Sort it.
3. Write the sorted 16Gb of data to a temp file.
4. Repeat steps 1-3 for the rest of the file until it now exists as a large set of 16Gb sorted temp files.
5. Merge the sorted temp files such that the final result is sorted 1 Tb file.

The **kway-mergesort** library is a C++ API that simplifies this logic into a single method.  It is templated so that one can sort a file of any object type.  For example, you could use it to sort a file of strings (raw text lines) or with almost as much ease, you could use it to sort a file of structured data, so long as you have an appropriate STRUCT that overloads the "<<" and ">>" operators.

You can even define your own comparison functions and pass them as parameters to the sorter.


A basic example with plain old strings
=======================================

In the example below, we are sorting an input file that is passed in from the command line and writing the sorted output to stdout (cout). In this example, we have defined a custom comparison function (alphaAsc) that drives how the file is sorted.  The KwayMergeSort class is templated, so we must define what type of object we are sorting.  In this case, we keep it simple and just treat each line of the input file as a string.  Hence the "*KwayMergeSort<string> *sorter = new KwayMergeSort<string>*".  We also tell it to only use 100Kb (base 10) of memory for sorting, and we ask it to write it's intermediate files to the present working directory.  The sorter will cleanup all of it's intermediate files when it has finished sorting.

Once we have defined how we want the sorting to be done, we instantiate a new instance of the "kway" class:

::

  KwayMergeSort<string> *sorter = new KwayMergeSort<string> (inFile, &cout, alphaAsc, bufferSize, compressOutput, tempPath);

This tells the class to sort "inFile", and write to "cout" (this could also be an ofstream).  We also tell it to use our custom sorting function (alphaAsc) and only "bufferSize" worth of memory.

Once we have instantiated the class, we ask it to sort the input and write the output to cout using:

::

  sorter->Sort();

Here's the entire listing.

::

  #include <iostream>
  #include <string>
  #include "kwaymergesort.h"
  
  // custom comparison functions for the ext. merge sort	
  bool alphaAsc(const string &a, const string &b) { return a < b; }
  
  int main(int argc, char* argv[]) {
  
      string inFile       = argv[1];
      int  bufferSize     = 100000;      // allow the sorter to use 100Kb (base 10) of memory for sorting.  
                                         // once full, it will dump to a temp file and grab another chunk.     
      bool compressOutput = false;       // not yet supported
      string tempPath     = "./";        // allows you to write the intermediate files anywhere you want.
      
      // sort the lines of a file lexicographically in ascending order (akin to UNIX sort, "sort FILE")
      KwayMergeSort<string> *sorter = new KwayMergeSort<string> (inFile, 
                                                                 &cout, 
                                                                 alphaAsc, 
                                                                 bufferSize, 
                                                                 compressOutput, 
                                                                 tempPath);
      sorter->Sort();
  } 

The included program "TestMerge.cpp" contains the above code and can be used to sort the provided text file of a large set of shuffled words from the english language.

::

  $ head english.txt 
  serialisation
  meant
  pause
  ablates
  sideband
  tensing
  bun
  instruments
  pearly
  tasteful

  $ g++ TestMerge.cpp -o testsort

  $ ./testsort english.txt | head
  aardvark
  aardwolf
  aaron
  aback
  abacus
  abaft
  abalone
  abandon
  abandoned
  abandonment

  $ ./testsort english.txt | tail
  zoologists
  zoology
  zoom
  zoomed
  zooming
  zooms
  zooplankton
  zoos
  zulu
  zulus


A more detailed example with a custom STRUCT and overloading ">>" and "<<"
===========================================================================

The following example illustrates how the same kway-mergesort class can be used to sort more complex data.  Here we will create a custom struct for a simple BED record.  The listing below defines a BED struct with three fields: chrom, start, and end. Now as mentioned, in order for the kway-mergesort class to sort the data, you must define overload the C++ >> and << operators so that the sorting class can know how to read and write BED data.  Accordingly, the BED struct includes custom definitions of << and >>.  Lastly, we need a custom comparison function to drive how the input file is sorted.  There are two options:

1. Overload the < operator in your struct / class.  An example of this is provided where the < operator sorts by chrom, then start.
2. Define a function (not class/struct method) for a custom sort.  An example of this is provided in the "bySize" function.

Once the STRUCT or CLASS and the optional comparison function are setup, the only difference in this example w.r.t. the above string example, is that we tell the template we are dealing with a BED type (hence the KwayMergeSort<BED>) and we provide the new comparison function.

In the example below, we first sort the file using the overloaded < operator.  Note that we instantiate the class without passing a custom function; this tells the sorter to use the < operator defined for BED.  We will get a compilation error if we don't provide a definition for the < operator and omit a comparison function when creating a class.

We then change the sort criteria to use the "bySize" function.  This change is made with the SetComparison() method.  

Lastly, we create a new class using the "bySize" function as a custom sort criteria.

::

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


