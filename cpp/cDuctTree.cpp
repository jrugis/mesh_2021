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
#include <fstream>
#include <iostream>
#include <string>

#include "cDuctTree.hpp"
#include "utils.hpp"
cDuctTree::cDuctTree(std::string fname)
{
  std::cout << "<DuctTree> reading mesh file: " + fname << std::endl;
  read_mesh_file(fname);
  print_info();
}

cDuctTree::~cDuctTree() {}

void cDuctTree::read_mesh_file(std::string mesh_name)
{
  std::string line;                    // file line buffer
  std::vector<std::string> tokens;     // tokenized line

  // open the duct file
  std::ifstream duct_file(mesh_name); 
  // get the duct point count
  while (getline(duct_file, line)) {
   	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	if (tokens[1]=="duct_node" ) break;
  }
  nodes_count = atoi(tokens[2].c_str());
  // get the duct segment count
  while (getline(duct_file, line)) {
   	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	if (tokens[1]=="duct_segment" ) break;
  }
  segments_count = atoi(tokens[2].c_str());
  // skip over the rest of the header
  while (getline(duct_file, line)) {
   	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	if (tokens[0]=="end_header" ) break;
  }
  // get duct nodes
  nodes.resize(nodes_count, Eigen::NoChange);
  for(int i=0; i<nodes_count; i++){
  	getline(duct_file, line);
  	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  	for(int j=0; j<3; j++) nodes(i,j) = std::stod(tokens[j]);
  }
  // get duct segments
  segments.resize(segments_count, Eigen::NoChange);
  for(int i=0; i<segments_count; i++){
  	getline(duct_file, line);
  	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  	for(int j=0; j<2; j++) segments(i,j) = std::stoi(tokens[j]);
  }
  duct_file.close();
}

void cDuctTree::get_dnd(Eigen::Vector3d p, int* seg, double* dist, double* pdist)
{
  Eigen::Vector3d w, v;
  double d, pd;
  *dist = 500.0; // large dummy initial distance
  for (int n = 0; n < segments_count; n++) {
    v = nodes.block<1, 3>(segments(n, 0), 0);
    w = nodes.block<1, 3>(segments(n, 1), 0);
    d = utils::get_distance(p, w, v, &pd);
	if (d < *dist){
	    *dist = d;      // distance to nearest duct segment
		*pdist = pd;    // distance along duct segment
		*seg = n;       // nearest duct segment index
	}
  }
}

void cDuctTree::print_info()
{
  std::cout << "<DuctTree> number of nodes: " << nodes_count << std::endl;
  std::cout << "<DuctTree> number of segments: " << segments_count << std::endl;
}