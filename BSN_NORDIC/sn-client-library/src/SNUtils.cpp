/** 
 * @file    : SNUtils.cpp
 * @author  : musahl
 * @date    : 17 Apr 2021
 * @version : 0.0
 * @brief   : Short description
 */
#include <SNUtils.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <vector>

SNUtils::SNUtils() {
  // TODO Auto-generated constructor stub

}

SNUtils::~SNUtils() {
  // TODO Auto-generated destructor stub
}

// C++11
class RandomNumberBetween
{
public:
    RandomNumberBetween(int low, int high)
    : random_engine_{std::random_device{}()}
    , distribution_{low, high}
    {
    }
    int operator()()
    {
        return distribution_(random_engine_);
    }
private:
    std::mt19937 random_engine_;
    std::uniform_int_distribution<int> distribution_;
};

std::string SNUtils::toHex(const std::string &s, bool upper_case ) {
  std::ostringstream ret;

  for (std::string::size_type i = 0; i < s.length(); ++i)
      ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << (uint32_t)(s[i]&0xFF) << " ";

  return ret.str();
}

std::string SNUtils::getRandomBinaryData(int length) {

  std::string dataToReturn("");
  std::generate_n(std::back_inserter(dataToReturn), length, RandomNumberBetween(0, 100));

  return dataToReturn;
}

std::vector<std::string> SNUtils::wordsFromString(std::string sentence) {
  std::vector < std::string > wordsToReturn;

  std::istringstream iss(sentence);
  std::string word;
  while(iss >> word) {
      /* do stuff with words */
    wordsToReturn.push_back(word);
  }
  return wordsToReturn;
}
