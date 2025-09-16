#pragma once
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include <c4/format.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace CommandParser {
class CommandParser {
public:
  CommandParser();
  ~CommandParser();
  void ReadYaml(const char filename[]);
  c4::yml::Tree GetCommandTree();

private:
  ryml::Tree tree;
};
} // namespace CommandParser
