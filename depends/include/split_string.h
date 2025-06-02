/**
 * @file split_string.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-06-02-05-30
 * @author Nemausa
 *
 */
#ifndef SPLIT_STRING_H_
#define SPLIT_STRING_H_

class SplitString {
 public:
  void Reset(char* buffer);
  char* Next(char delim);
  char* Next(const char* delim);

 private:
  char* ptr_;
};

#endif  // SPLIT_STRING_H_
