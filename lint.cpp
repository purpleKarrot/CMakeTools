/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include <iostream>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace fs = boost::filesystem;

struct Rule
  {
  boost::regex regex;
  std::string info;
  };

int max_length = 80;
std::vector<Rule> rules;

void read_rules(const char* filename)
  {
  std::string regex, info;
    {
    rules.emplace_back(boost::regex(regex), info);
    }
  }

//const Rule rules[] =
//  {
//    {boost::regex("\t"), "tabs shall not be used!"},
//    {boost::regex("\\s$"), "lines shall not end with whitespace!"},
//    {boost::regex("^\\s*[A-Z0-9_]+\\("), "commands shall be all lowercase!"},
//    {boost::regex("^\\s*end(foreach|function|if|macro|while)\\([^)]+\\)"), "block end shall be empty!"},
//    {boost::regex("^\\s*else\\([^)]+\\)"), "'else' shall be empty!"},
//  };

void check_line(std::string const& line, fs::path const& path, std::size_t nr)
  {
  BOOST_FOREACH(Rule const& rule, rules)
    {
    if (boost::regex_search(line, rule.regex))
      {
      std::cout
        << path.string()
        << '(' << nr << ") : "
        << rule.info
        << std::endl
        ;
      }
    }
  if (line.size() > max_length)
    {
    std::cout
      << path.string()
      << '(' << nr << ") : line is wider than " << max_length << " characters!"
      << std::endl
      ;
    }
  }

void check_file(fs::path const& path, Rules const& rules)
  {
  std::string line;
  std::size_t number = 1;
  fs::ifstream file(path);
  while (std::getline(file, line))
    {
    if (line[0] != '#')
      {
      check_line(line, rules, path.string() + '(' + number + ')');
      }
    ++number;
    }
  }

bool is_cmake_file(fs::path const& path)
  {
  if (path.filename() == "CMakeLists.txt")
    {
    return true;
    }
  if (path.extension() == ".cmake")
    {
    return true;
    }
  return false;
  }

int main(int argc, char* argv[])
  {
  if (argc < 3)
    {
    std::cerr << "Insufficient parameters are insufficient!" << std::endl;
    return -1;
    }
  read_rules(argv[1]);
  using boost::filesystem::recursive_directory_iterator;
  for (recursive_directory_iterator it(argv[2]), end; it != end; ++it)
    {
    if (!is_cmake_file(it->path()))
      {
      continue;
      }
    check_file(it->path(), rules);
    }
  }

