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
  double get_dnl(Eigen::Vector3d p);
  double get_radius(int seg, double pdist);

  private:
  int nodes_count, segments_count;  // the number of nodes and segments in the lumen tree
  MatrixN3d nodes;                  // 3x coordinate
  MatrixN1d radii;                  // duct radius (per node)
  MatrixN2i segments;               // line segment indices, 2x points
  MatrixN1i seg_type;               // segment type (0 = duct, 1 = first acinus, 2 = second acinus, ...)
  void read_mesh_file(std::string mesh_name);
  void print_info();
};

#endif /* CDUCTTREE_ */
