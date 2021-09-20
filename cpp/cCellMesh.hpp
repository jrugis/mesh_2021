/*
 * cCellMesh.hpp
 *
 *	Created on: 14/09/2021
 *  Author: jrugis
 */

#ifndef CCELLMESH_H_
#define CCELLMESH_H_

#define APICAL_D 5 
#define BASAL_D 16 

#include <Eigen/Dense>
#include <string>

#include "global_defs.hpp"

class cDuctTree;

class cCellMesh {
  public:
  cCellMesh(const std::string mesh_name);
  ~cCellMesh();
  int ctype;                   // this cell type (ACINUS, INTERCALATED or STRIATED)
  int vertices_count;
  int surface_triangles_count;
  int tetrahedrons_count;
  MatrixN3d vertices;          // 3x coordinate
  MatrixN3i surface_triangles; // 3x vertex indices
  MatrixN4i tetrahedrons;      // 4x vertex indices
  MatrixN1i t_di;              // nearest duct segment index (per node)
  MatrixN1d t_dfnd;            // distance to nearest duct segment (per node)
  MatrixN1d t_dad;             // distance along duct segment from in to out (per node)
  MatrixN1i tri_types;         // triangle type: APICAL, BASOLATERAL, BASAL
  void calc_nd(cDuctTree* dtree);
  void write_mesh_file(std::string mesh_name);

  private:
  void print_info();
  void read_mesh_file(std::string mesh_name);
};

#endif /* CCELLMESH_H_ */
