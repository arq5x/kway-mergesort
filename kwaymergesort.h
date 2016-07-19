/****************************************************************************
   kwaymergesort.h (c) 2016 David Nogueira

   Derivated from Aaron Quinlan (https://github.com/arq5x/kway-mergesort)
****************************************************************************/
#ifndef KWAYMERGESORT_H
#define KWAYMERGESORT_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
#include <iterator>
#include <cstdio>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#include <libgen.h> //for basename()
#endif

namespace {
#ifdef _WIN32
#include <stdio.h>
#include <locale.h>
#include <assert.h>

#if defined _WIN32 || defined __WIN32__ || defined __EMX__ || defined __DJGPP__
/* Win32, OS/2, DOS */
# define HAS_DEVICE(P) \
((((P)[0] >= 'A' && (P)[0] <= 'Z') || ((P)[0] >= 'a' && (P)[0] <= 'z')) \
&& (P)[1] == ':')
# define FILESYSTEM_PREFIX_LEN(P) (HAS_DEVICE (P) ? 2 : 0)
# define ISSLASH(C) ((C) == '/' || (C) == '\\')
#endif

#ifndef FILESYSTEM_PREFIX_LEN
# define FILESYSTEM_PREFIX_LEN(Filename) 0
#endif

#ifndef ISSLASH
# define ISSLASH(C) ((C) == '/')
#endif

char *
basename(char const *name) {
  char const *base = name += FILESYSTEM_PREFIX_LEN(name);
  int all_slashes = 1;
  char const *p;

  for (p = name; *p; p++) {
    if (ISSLASH(*p))
      base = p + 1;
    else
      all_slashes = 0;
  }

  /* If NAME is all slashes, arrange to return `/'. */
  if (*base == '\0' && ISSLASH(*name) && all_slashes)
    --base;

  /* Make sure the last byte is not a slash. */
  assert(all_slashes || !ISSLASH(*(p - 1)));

  return (char *)base;
}
#endif
}


using namespace std;

bool isRegularFile(const string& filename);
// STLized version of basename()
// (because POSIX basename() modifies the input string pointer)
// Additionally: removes any extension the basename might have.
std::string stl_basename(const std::string &path) {
  string result;

  char* path_dup = strdup(path.c_str());
  char* basename_part = basename(path_dup);
  result = basename_part;
  free(path_dup);

  size_t pos = result.find_last_of('.');
  if (pos != string::npos)
    result = result.substr(0, pos);

  return result;
}


template <class T>
class MERGE_DATA {

public:
  // data
  T data;
  istream *stream;
  bool(*compFunc)(const T &a, const T &b);

  // constructor
  MERGE_DATA(const T &data,
             istream *stream,
             bool(*compFunc)(const T &a, const T &b))
    :
    data(data),
    stream(stream),
    compFunc(compFunc) {}

  // comparison operator for maps keyed on this structure
  bool operator < (const MERGE_DATA &a) const {
    if (compFunc)
      return compFunc(data, a.data);
    else
      return data < a.data;
  }
};


//************************************************
// DECLARATION
// Class methods and elements
//************************************************
template <class T> class KwayMergeSort;


template <class T>
class KwayMergeSortIterator
  :  public std::iterator <std::forward_iterator_tag, T, ptrdiff_t, const T*, const T&> {
public:
  friend KwayMergeSort<T>;

  // Construction/Destruction //////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  KwayMergeSortIterator() :
    _owner(nullptr),
    _eof(false) {};
  KwayMergeSortIterator(KwayMergeSort<T> * owner,
                        T elem,
                        bool eof = false) :
    _owner(owner),
    _elem(elem),
    _eof(eof) {};
  ~KwayMergeSortIterator() {};


  KwayMergeSortIterator(const KwayMergeSortIterator &other) :
    KwayMergeSortIterator() {
    this->operator=(other);
  }

  KwayMergeSortIterator& KwayMergeSortIterator::operator=(const KwayMergeSortIterator & other) {
    _owner = other._owner;
    _elem = other._elem;
    _eof = other._elem;
    return *this;
  }
  // Forward immutable standard iterator interface ///////////////////////
  //////////////////////////////////////////////////////////////////////////////
  bool operator==(const KwayMergeSortIterator &other) const {
    if (other._eof == true && _eof == true)
      return true;
    else if (_eof == true)
      return false;
    else if (other._eof == true)
      return false;
    else {//both _eof false
      if (_elem == other.e_elem)
        return true;
      else
        return false;
    }
  };
  bool operator!=(const KwayMergeSortIterator &other) const {
    if (other._eof == true && _eof == true)
      return false;
    else if (_eof == true)
      return true;
    else if (other._eof == true)
      return true;
    else {//both _eof false
      if (_elem == other._elem)
        return  false;
      else
        return true;
    }
  };

  KwayMergeSortIterator& operator++() {
    if (_owner->_outQueue.empty()) {
      _eof = true;
      _owner->CloseTempFiles();
      return *this;
    } else {
      _elem = _owner->MergeStepByStep();
      return *this;
    }
  };

  KwayMergeSortIterator  operator++(int) {
    KwayMergeSortIterator tmp = *this;
    ++*this;
    return tmp;
  };

  const T& operator*() const {
    return _elem;
  };

  T value() { return _elem; };

protected:
  KwayMergeSort<T> * _owner;
  T _elem;
  bool _eof;
};




template <class T>
class KwayMergeSort {

public:
  friend KwayMergeSortIterator<T>;
  typedef KwayMergeSortIterator<T> iterator;

  // constructor, using custom comparison function
  KwayMergeSort(const string &inFile,
                ostream *out = nullptr,
                bool(*compareFunction)(const T &a, const T &b) = NULL,
                int  maxBufferSize = 1000000,
                bool compressOutput = false,
                string tempPath = "");

  // constructor, using T's overloaded < operator.  Must be defined.
  KwayMergeSort(const string &inFile,
                ostream *out = nullptr,
                int  maxBufferSize = 1000000,
                bool compressOutput = false,
                string tempPath = "");

  // destructor
  ~KwayMergeSort(void);
  // Sort the data
  void Sort();
  // change the buffer size
  void SetBufferSize(int bufferSize);
  // change the sort criteria
  void SetComparison(bool(*compareFunction)(const T &a, const T &b));

  KwayMergeSortIterator<T> begin();
  KwayMergeSortIterator<T> end();

protected:
  // drives the creation of sorted sub-files stored on disk.
  void DivideAndSort();
  // drives the merging of the sorted temp files.
  // final, sorted and merged output is written to "out".
  void Merge();
  
  void InitializeMergeStep();
  T MergeStepByStep();


  void WriteToTempFile(const vector<T> &lines);
  void OpenTempFiles();
  void CloseTempFiles();

protected:
  string _inFile;
  bool(*_compareFunction)(const T &a, const T &b);
  string _tempPath;
  vector<string>    _vTempFileNames;
  vector<ifstream*>  _vTempFiles;
  unsigned int _maxBufferSize;
  unsigned int _runCounter;
  bool _compressOutput;
  bool _tempFileUsed;
  ostream *_out;

  // priority queue for the buffer.
  std::multiset < MERGE_DATA<T> > _outQueue;
  T _line;

};



//************************************************
// IMPLEMENTATION
// Class methods and elements
//************************************************

// constructor
template <class T>
KwayMergeSort<T>::KwayMergeSort(const string &inFile,
                                ostream *out,
                                bool(*compareFunction)(const T &a, const T &b),
                                int maxBufferSize,
                                bool compressOutput,
                                string tempPath)
  : _inFile(inFile)
  , _out(out)
  , _compareFunction(compareFunction)
  , _tempPath(tempPath)
  , _maxBufferSize(maxBufferSize)
  , _runCounter(0)
  , _compressOutput(compressOutput) {}

// constructor
template <class T>
KwayMergeSort<T>::KwayMergeSort(const string &inFile,
                                ostream *out,
                                int maxBufferSize,
                                bool compressOutput,
                                string tempPath)
  : _inFile(inFile)
  , _out(out)
  , _compareFunction(NULL)
  , _tempPath(tempPath)
  , _maxBufferSize(maxBufferSize)
  , _runCounter(0)
  , _compressOutput(compressOutput) {}

// destructor
template <class T>
KwayMergeSort<T>::~KwayMergeSort(void) {}

// API for sorting.  
template <class T>
void KwayMergeSort<T>::Sort() {
  DivideAndSort();
  Merge();
}

// change the buffer size used for sorting
template <class T>
void KwayMergeSort<T>::SetBufferSize(int bufferSize) {
  _maxBufferSize = bufferSize;
}

// change the sorting criteria
template <class T>
void KwayMergeSort<T>::SetComparison(bool(*compareFunction)(const T &a, const T &b)) {
  _compareFunction = compareFunction;
}


template <class T>
void KwayMergeSort<T>::DivideAndSort() {

  istream *input = new ifstream(_inFile.c_str(), ios::in);
  // gzipped
  // if ((isGzipFile(_inFile) == true) && (isRegularFile(_inFile) == true)) {
  //     delete input;
  //     input = new igzstream(_inFile.c_str(), ios::in);
  // }

  // bail unless the file is legit
  if (input->good() == false) {
    cerr << "Error: The requested input file (" << _inFile
      << ") could not be opened. Exiting!" << endl;
    exit(1);
  }
  vector<T> lineBuffer;
  lineBuffer.reserve(100000);
  unsigned int totalBytes = 0;  // track the number of bytes consumed so far.

  // track whether or not we actually had to use a temp
  // file based on the memory that was allocated
  _tempFileUsed = false;

  // keep reading until there is no more input data
  T line;
  while (*input >> line) {

    // add the current line to the buffer and track the memory used.
    lineBuffer.push_back(line);
    totalBytes += sizeof(line);  // buggy?

    // sort the buffer and write to a temp file if we have filled up our quota
    if (totalBytes > _maxBufferSize) {
      if (_compareFunction != NULL)
        sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
      else
        sort(lineBuffer.begin(), lineBuffer.end());
      // write the sorted data to a temp file
      WriteToTempFile(lineBuffer);
      // clear the buffer for the next run
      lineBuffer.clear();
      _tempFileUsed = true;
      totalBytes = 0;
    }
  }

  // handle the run (if any) from the last chunk of the input file.
  if (lineBuffer.empty() == false) {
    // write the last "chunk" to the tempfile if
    // a temp file had to be used (i.e., we exceeded the memory)
    if (_tempFileUsed == true) {
      if (_compareFunction != NULL)
        sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
      else
        sort(lineBuffer.begin(), lineBuffer.end());
      // write the sorted data to a temp file
      WriteToTempFile(lineBuffer);
    }
    // otherwise, the entire file fit in the memory given,
    // so we can just dump to the output.
    else {
      if (_compareFunction != NULL)
        sort(lineBuffer.begin(), lineBuffer.end(), *_compareFunction);
      else
        sort(lineBuffer.begin(), lineBuffer.end());
      for (size_t i = 0; i < lineBuffer.size(); ++i) {
        if (_out) {
          *_out << lineBuffer[i] << endl;
        }
      }
    }
  }
  static_cast<ifstream*>(input)->close();
}


template <class T>
void KwayMergeSort<T>::WriteToTempFile(const vector<T> &lineBuffer) {
  // name the current tempfile
  stringstream tempFileSS;
  if (_tempPath.size() == 0)
    tempFileSS << _inFile << "." << _runCounter;
  else
    tempFileSS << _tempPath << stl_basename(_inFile) << "." << _runCounter;
  string tempFileName = tempFileSS.str();

  // do we want a regular or a gzipped tempfile?
  ofstream *output;
  //if (_compressOutput == true)
      //output = new ogzstream(tempFileName.c_str(), ios::out);
  //else
  output = new ofstream(tempFileName.c_str(), ios::out);

  // write the contents of the current buffer to the temp file
  for (size_t i = 0; i < lineBuffer.size(); ++i) {
    *output << lineBuffer[i] << endl;
  }

  // update the tempFile number and add the tempFile to the list of tempFiles
  ++_runCounter;
  output->close();
  delete output;
  _vTempFileNames.push_back(tempFileName);
}


//---------------------------------------------------------
// MergeDriver()
//
// Merge the sorted temp files.
// uses a priority queue, with the values being a pair of
// the record from the file, and the stream from which the record came.
// SEE: http://stackoverflow.com/questions/2290518/c-n-way-merge-for-external-sort,
// post from Eric Lippert.
//----------------------------------------------------------
template <class T>
void KwayMergeSort<T>::Merge() {

  // we can skip this step if there are no temp files to
  // merge.  That is, the entire inout file fit in memory
  // and thus we just dumped to stdout.
  if (_tempFileUsed == false)
    return;

  // open the sorted temp files up for merging.
  // loads ifstream pointers into _vTempFiles
  OpenTempFiles();

  // extract the first line from each temp file
  InitializeMergeStep();

  // keep working until the queue is empty
  while (_outQueue.empty() == false) {
    MergeStepByStep();
  }
  // clean up the temp files.
  CloseTempFiles();
}

template <class T>
void KwayMergeSort<T>::InitializeMergeStep() {
  // extract the first line from each temp file
  for (size_t i = 0; i < _vTempFiles.size(); ++i) {
    *_vTempFiles[i] >> _line;
    _outQueue.insert
    (MERGE_DATA<T>(_line, _vTempFiles[i], _compareFunction));
  }
}

template <class T>
T KwayMergeSort<T>::MergeStepByStep() {
  // grab the lowest element, print it, then ditch it.
  MERGE_DATA<T> lowest = *(_outQueue.begin());
  // write the entry from the top of the queue
  if (_out) {
    *_out << lowest.data << endl;
  }
  // remove this record from the queue
  _outQueue.erase(_outQueue.begin());
  // add the next line from the lowest stream (above) to the queue
  // as long as it's not EOF.
  *(lowest.stream) >> _line;
  if (*(lowest.stream))
    _outQueue.insert
    (MERGE_DATA<T>(_line, lowest.stream, _compareFunction));

  return lowest.data;
}


template <class T>
KwayMergeSortIterator<T> KwayMergeSort<T>::begin() {

  DivideAndSort();

  // we can skip this step if there are no temp files to
  // merge.  That is, the entire inout file fit in memory
  // and thus we just dumped to stdout.
  if (_tempFileUsed == false)
    return end();

  // open the sorted temp files up for merging.
  // loads ifstream pointers into _vTempFiles
  OpenTempFiles();

  // extract the first line from each temp file
  InitializeMergeStep();
    
  return KwayMergeSortIterator<T>(this, MergeStepByStep());
}

template <class T>
KwayMergeSortIterator<T> KwayMergeSort<T>::end() {

  return KwayMergeSortIterator<T>(this, {}, true);
}

template <class T>
void KwayMergeSort<T>::OpenTempFiles() {
  for (size_t i = 0; i < _vTempFileNames.size(); ++i) {

    ifstream *file;

    // not gzipped
    // if ((isGzipFile(_vTempFileNames[i]) == false) && (isRegularFile(_vTempFileNames[i]) == true)) {
    if (isRegularFile(_vTempFileNames[i]) == true) {
      file = new ifstream(_vTempFileNames[i].c_str(), ios::in);
    }
    // gzipped
    //else if ((isGzipFile(_vTempFileNames[i]) == true) && (isRegularFile(_vTempFileNames[i]) == true)) {
    //    file = new igzstream(_vTempFileNames[i].c_str(), ios::in);
    //}

    if (file->good() == true) {
      // add a pointer to the opened temp file to the list
      _vTempFiles.push_back(file);
    } else {
      cerr << "Unable to open temp file (" << _vTempFileNames[i]
        << ").  I suspect a limit on number of open file handles.  Exiting."
        << endl;
      exit(1);
    }
  }
}


template <class T>
void KwayMergeSort<T>::CloseTempFiles() {
  // delete the pointers to the temp files.
  for (size_t i = 0; i < _vTempFiles.size(); ++i) {
    _vTempFiles[i]->close();
    delete _vTempFiles[i];
  }
  // delete the temp files from the file system.
  for (size_t i = 0; i < _vTempFileNames.size(); ++i) {
    remove(_vTempFileNames[i].c_str());  // remove = UNIX "rm"
  }
}


/*
   returns TRUE if the file is a regular file:
     not a pipe/device.

   This implies that the file can be opened/closed/seek'd multiple times without losing information
 */
bool isRegularFile(const string& filename) {
  struct stat buf;
  int i;

  i = stat(filename.c_str(), &buf);
  if (i != 0) {
    cerr << "Error: can't determine file type of '" << filename << "': " << strerror(errno) << endl;
    exit(1);
  }
  //  if (S_ISREG(buf.st_mode))
  return true;

  return false;
}


/*
   returns TRUE if the file has a GZIP header.
   Should only be run on regular files.
 */
bool isGzipFile(const string& filename) {
  //see http://www.gzip.org/zlib/rfc-gzip.html#file-format
  struct {
    unsigned char id1;
    unsigned char id2;
    unsigned char cm;
  } gzip_header;
  ifstream f(filename.c_str(), ios::in | ios::binary);
  if (!f)
    return false;

  if (!f.read((char*)&gzip_header, sizeof(gzip_header)))
    return false;

  if (gzip_header.id1 == 0x1f
      &&
      gzip_header.id2 == 0x8b
      &&
      gzip_header.cm == 8)
    return true;

  return false;
}





#endif /* KWAYMERGESORT_H */

