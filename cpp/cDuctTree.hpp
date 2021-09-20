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
  void get_dnd(Eigen::Vector3d p, int* seg, double* dist, double* pdist);

  private:
  int nodes_count, segments_count;  // the number of nodes and segments in the lumen tree
  MatrixN3d nodes;                  // 3x coordinate
  MatrixN2i segments;               // line segment indices, 2x points
  MatrixN1d radius;                 // duct radius (per node)
  void read_mesh_file(std::string mesh_name);
  void print_info();
};

#endif /* CDUCTTREE_ */
