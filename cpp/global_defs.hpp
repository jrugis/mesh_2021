/*
 * global_defs.hpp
 *
 *	Created on: 14/09/2021
 *			Author: jrugis
 */

#ifndef DEFS_H_
#define DEFS_H_

#include <Eigen/Dense>
//#include <Eigen/Sparse>
//#include <string>

//************************************************************************
// model sizes
//	NOTE: a single acinus with lumen (for now)
//#define CELLS_COUNT 7

//************************************************************************
// MPI defs, macros

//#define ACINUS_CELL_TAG 400
//#define CELL_CELL_TAG 401
//#define LUMEN_CELL_TAG 402

//************************************************************************
// cell to cell connectivity
//  this_triangle, other_cell, other_triangle
//enum cell_conn { tTri, oCell, oTri, CCONNCOUNT };

//************************************************************************
// some convenience typedefs
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> MatrixN1d;
typedef Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajorBit> MatrixN3d;
//typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixNNd;
//typedef Eigen::SparseMatrix<double> SparceMatrixd;

typedef Eigen::Matrix<int, Eigen::Dynamic, 1> MatrixN1i;
typedef Eigen::Matrix<int, Eigen::Dynamic, 2, Eigen::RowMajorBit> MatrixN2i;
typedef Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajorBit> MatrixN3i;
typedef Eigen::Matrix<int, Eigen::Dynamic, 4, Eigen::RowMajorBit> MatrixN4i;
//typedef Eigen::Matrix<int, Eigen::Dynamic, CCONNCOUNT, Eigen::RowMajorBit> MatrixNCi;
//typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> MatrixNNi;

//typedef Eigen::Vector3d Vector3d;
//typedef Eigen::Vector3i Vector3i;
//typedef Eigen::Vector4i Vector4i;

//************************************************************************
// cell types
enum cell_types { ACINUS, INTERCALATED, STRIATED, UNKNOWN };

//************************************************************************
//************************************************************************

#endif /* DEFS_H_ */
