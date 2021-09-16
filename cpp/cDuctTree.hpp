/*
 * cDuctTree.hpp
 *
 *	Created on: 14/09/2021
 *			Author: jrugis
 */

#ifndef CDUCTTREE_H_
#define CDUCTTREE_H_

#include <string>

#include "global_defs.hpp"

class cDuctTree {
  public:
  cDuctTree(std::string fname);
  ~cDuctTree();

  double get_dnl(const Eigen::Vector3d p);

  private:
  std::string id;
  std::ofstream* out;
  int points_count, segments_count; // the number of points and segments in the lumen tree
  MatrixN3d points;                 // 3x coordinate
  MatrixN2i segments;               // line segment indices, 2x points

  void get_segments(std::string fname);
  void print_info();
};

#endif /* CDUCTTREE_ */
