/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

// filename, set of line numbers
using Coverage = std::unordered_map<std::string, std::set<int>>;

Coverage read_coverage(const char* filename)
{
  Coverage coverage;
  std::ifstream logfile(filename);

  std::string line;
  boost::smatch match;
  while (std::getline(logfile, line))
  {
    if (!regex_match(line, match, boost::regex("^([^(]+)\\((\\d+)\\):  .*")))
    {
      continue;
    }

    coverage[match[1]].insert(boost::lexical_cast<int>(match[2]));
  }

  return coverage;
}

static bool keep_line(std::string const& line)
{
  if (line.empty())
  {
    return true;
  }

  // keep comments
  if (regex_search(line, boost::regex("^\\s*#")))
  {
    return true;
  }

  // keep lines containing just whitespace
  if (regex_search(line, boost::regex("^\\s*$")))
  {
    return true;
  }

  // TODO: keep block ends?
  if (regex_search(line, boost::regex("^\\s*end(foreach|function|if|macro|while)\\s*\\(")))
  {
    return true;
  }

  // keep lines not calling a command
  if (!regex_search(line, boost::regex("^\\s*[a-zA-Z0-9_]+\\s*\\(")))
  {
    return true;
  }

  return false;
}

void check_coverage(const char* directory, Coverage const& coverage)
{
  using boost::filesystem::recursive_directory_iterator;
  for (recursive_directory_iterator it(directory), end; it != end; ++it)
  {
    if (is_directory(it->path()) || it->path().extension() != ".cmake")
    {
      continue;
    }

    auto path = it->path().generic_string();
    auto found = coverage.find(path);
    if (found == coverage.end())
    {
      std::cout << "Removing unused file: " << path << std::endl;
      remove(it->path());
      continue;
    }

    // scope for tempfile and listsfile
    {
      std::ofstream tempfile(path + ".tmp");
      boost::filesystem::ifstream listsfile(it->path());

      std::string line;
      std::size_t number = 0;
      while (std::getline(listsfile, line))
      {
        ++number;
        if (found->second.count(number) || keep_line(line))
        {
          tempfile << line << std::endl;
        }
      }
    }

    remove(it->path());
    rename(path + ".tmp", it->path());
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
