/*
 * cCellMesh.hpp
 *
 *	Created on: 14/09/2021
 *  Author: jrugis
 */

#ifndef CCELLMESH_H_
#define CCELLMESH_H_

#include <Eigen/Dense>
#include <string>

#include "global_defs.hpp"

class cDuctTree;

class cCellMesh {
  public:
  cCellMesh(const std::string mesh_name, const cDuctTree* dtree);
  ~cCellMesh();
  void print_info();

  int ctype;                   // this cell type (ACINUS, INTERCALATED or STRIATED)
  int vertices_count;
  int surface_triangles_count;
  int tetrahedrons_count;
  MatrixN3d vertices;          // 3x coordinate
  MatrixN3i surface_triangles; // 3x vertex indices
  MatrixN4i tetrahedrons;      // 4x vertex indices
  MatrixN1i n_di;              // nearest duct segment index (per node)
  MatrixN1d n_dfnd;            // distance from nearest duct segment (per node)
  MatrixN1d n_dad;             // distance along duct segment from in to out (per node)

//  MatrixNCi common_triangles;        // this triangle, other cell, other triangle
//  MatrixNCi common_apical_triangles; // this triangle, other cell, other triangle
//  MatrixN1i apical_triangles;        // surface triangle indicies
//  MatrixN1i basal_triangles;         // surface triangle indicies
//  MatrixN1d e_dfa;                   // distance from apical (per element)
//  MatrixN1d e_dfb;                   // distance from basal (per element)
//  int common_triangles_count, apical_triangles_count, basal_triangles_count;

  private:
//  cCell_calcium* parent;
//  void calc_common();
//  void calc_dfa();
//  void calc_apical_basal();
//  void calc_dfb();
  void read_mesh_file(std::string mesh_name);
  void write_mesh_file();
  void calc_nd(const cDuctTree* dtree);
//  void identify_common_apical_triangles();
};

#endif /* CCELLMESH_H_ */
