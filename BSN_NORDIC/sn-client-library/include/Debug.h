#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_
 
#include <iostream> 
#include <fstream>

#ifndef NDEBUG
  #ifndef DEBUG
  #define DEBUG()   std::cout
  #endif
  #ifndef INFO
  #define INFO()    std::cout
  #endif
  #ifndef ERROR
  #define ERROR()   std::cerr
  #endif
#else
  extern std::ofstream devNullStream;
  #define DEBUG()   devNullStream
  #define INFO()    devNullStream
  #define ERROR()   devNullStream
#endif

#endif //INCLUDE_DEBUG_H_
