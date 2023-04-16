#ifndef FILE_H
#define FILE_H
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

class Directory 
{
  public:
    std::vector<std::string> paths{};
    Directory(std::string pathToDir);
};
#endif
