/** 
 * @file    : Debug.cpp
 * @author  : musahl
 * @date    : 22 Feb 2021
 * @version : 0.0
 * @brief   : Short description
 */ 


#include <iostream>
#include <fstream>

#ifndef NDEBUG
//NOTHING TO DO HERE
#else
//provide devnull pipeline for DEBUG / INFO and ERROR
std::ofstream devNullStream("dev/null");
#endif
