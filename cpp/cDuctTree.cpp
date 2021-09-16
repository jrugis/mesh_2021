/*
 * cDuctTree.cpp
 *
 *	Created on: 14/09/2021
 *	Author: jrugis
 */

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/tokenizer.hpp>
#include <string>

#include "cDuctTree.hpp"
#include "utils.hpp"
cDuctTree::cDuctTree(std::string fname)
{
  get_segments(fname);
}

cDuctTree::~cDuctTree() {}

void cDuctTree::get_segments(std::string fname)
{
  std::ifstream duct_file(fname.c_str(), std::ios::in); // open the duct file
  std::string line;                                                             // file line buffer
  std::vector<std::string> tokens;                                              // tokenized line
  /*
  // check the file is open
  if (not lumen_file.is_open()) { utils::fatal_error("lumen file " + file_name + " could not be opened", *out); }
  // get the lumen tree points
  getline(lumen_file, line);
  boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  points_count = std::stoi(tokens[0]);
  points.resize(points_count, Eigen::NoChange);
  for (int n = 0; n < points_count; n++) {
    getline(lumen_file, line);
    boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
    for (int i = 0; i < 3; i++) { points(n, i) = std::stof(tokens[i]); }
  }
  // get the lumen tree segments
  getline(lumen_file, line);
  boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  segments_count = std::stoi(tokens[0]);
  segments.resize(segments_count, Eigen::NoChange);
  for (int n = 0; n < segments_count; n++) {
    getline(lumen_file, line);
    boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
    for (int i = 0; i < 2; i++) {
      segments(n, i) = std::stoi(tokens[i]) - 1; // change to zero Treed indexing
    }
  }
  */
  duct_file.close();
}

double cDuctTree::get_dnl(const Eigen::Vector3d p)
{
  double d = 100.0; // large dummy initial distance
  Eigen::Vector3d w, v;
  for (int n = 0; n < segments_count; n++) {
    v = points.block<1, 3>(segments(n, 0), 0);
    w = points.block<1, 3>(segments(n, 1), 0);
    //d = std::min(d, utils::get_distance(p, w, v));
  }
  return (d);
}

void cDuctTree::print_info()
{
  *out << "<DuctTree> number of points: " << points_count << std::endl;
  *out << "<DuctTree> number of segments: " << segments_count << std::endl;
}