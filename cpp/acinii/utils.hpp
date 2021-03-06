/*
 * utils.hpp
 *
 *  Created on: 2/8/2021
 *      Author: jrugis
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <fstream>
#include <iostream>
#include <string>

#include "global_defs.hpp"

namespace utils {
  //void calc_tri_centers(MatrixN3d& centers, const MatrixN3d& vertices, const MatrixN3i& triangles);
  void fatal_error(const std::string msg);
  //void get_parameters(const std::string file_id, int ptype, int cell_num, double* p, std::ofstream& out);
  double get_distance(Vector3d p, Vector3d v, Vector3d w, double* pd);
  //void read_mesh(const std::string file_name, sMeshVals& mesh_vals);
  //void save_matrix(const std::string file_name, int bytes, char* data);
} // namespace utils

#endif /* UTILS_H_ */
