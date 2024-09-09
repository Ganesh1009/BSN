/** 
 * @file    : SNUtils.h
 * @author  : musahl
 * @date    : 17 Apr 2021
 * @version : 0.0
 * @brief   : Short description
 */
#ifndef SRC_SNUTILS_H_
#define SRC_SNUTILS_H_

#include <string>
#include <vector>

class SNUtils {
 public:
  SNUtils();
  virtual ~SNUtils();
  static std::string toHex(const std::string &s, bool upper_case = true);
  static std::string getRandomBinaryData(int length);
  static std::vector<std::string> wordsFromString(std::string sentence);
};

#endif /* SRC_SNUTILS_H_ */

