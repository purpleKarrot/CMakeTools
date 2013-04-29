/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include <set>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

using Coverage = std::unordered_map<std::string, std::set<int>>;

Coverage read_coverage(const char* filename)
  {
  Coverage coverage;
  std::ifstream logfile(filename);

  std::string line;
  boost::smatch match;
  while (std::getline(logfile, line))
    {
    if (!regex_match(line, boost::regex("^([^(]+)\\((\\d+)\\):  "), match))
      {
      continue;
      }
    coverage[match[1]].insert(std::stoi(match[2]));
    }
  return coverage;
  }

void check_coverage(const char* directory, Coverage const& coverage)
  {
  using boost::filesystem::recursive_directory_iterator;
  for (recursive_directory_iterator it(directory), end; it != end; ++it)
    {
    auto path = it->path().generic_string();
    auto found = coverage.find(path);
    if (found == coverage.end())
      {
      std::cout << path << ":  Unused file." << std::endl;
      continue;
      }
    std::string line;
    std::size_t number = 0;
    while (std::getline(logfile, line))
      {
      ++number;
      if (line.empty())
        {
        continue;
        }
      if (!found->count(number))
        {
        continue;
        }
      if (regex_search(line, boost::regex("^\\s*#")))
        {
        continue;
        }
      if (regex_search(line, boost::regex("^\\s*end(foreach|function|if|macro|while)\\s*\\(")))
        {
        continue;
        }
      std::cout << path << "(" << number << "):  " << line << std::endl;
      }
    }
  }

int main(int argc, char* argv[])
  {
  if (argc < 3)
    {
    std::cerr << "Insufficient parameters are insufficient!" << std::endl;
    return -1;
    }
  const auto coverage = read_coverage(argv[1]);
  check_coverage(argv[2], coverage);
  return 0;
  }
