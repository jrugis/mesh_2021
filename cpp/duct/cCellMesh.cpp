/*
 * cCellMesh.cpp
 *
 *	Created on: 14/09/2021
 *	Author: jrugis
 */

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/tokenizer.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "cCellMesh.hpp"
#include "cDuctTree.hpp"
#include "utils.hpp"

cCellMesh::cCellMesh(const std::string mesh_name)
{
  read_mesh_file(mesh_name);
  print_info();  
}

cCellMesh::~cCellMesh() {}

void cCellMesh::read_mesh_file(std::string mesh_name)
{
  std::cout << "<CellMesh> reading mesh file: " + mesh_name << std::endl;
  char ct = mesh_name.at(10);
  if(ct=='a') ctype = ACINUS;
  else if(ct=='i') ctype = INTERCALATED;
  else if(ct=='s') ctype = STRIATED;
  else ctype = UNKNOWN;
  
  std::string line;                    // file line buffer
  std::vector<std::string> tokens;     // tokenized line

  // open the mesh file
  std::ifstream mesh_file(mesh_name); 
  // get the total mesh vertex count
  while (getline(mesh_file, line)) {
   	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	if (tokens[1]=="vertex" ) break;
  }
  vertices_count = atoi(tokens[2].c_str());
  // get the total face count
  while (getline(mesh_file, line)) {
   	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	if (tokens[1]=="face" ) break;
  }
  surface_triangles_count = atoi(tokens[2].c_str());
  // get the total tetrahedron count
  if(ctype == ACINUS ){
    while (getline(mesh_file, line)) {
      boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	  if (tokens[1]=="tetrahedron" ) break;
    }
    tetrahedrons_count = atoi(tokens[2].c_str());
  }
  else tetrahedrons_count= 0;
  // skip over the rest of the header
  while (getline(mesh_file, line)) {
   	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
   	if (tokens[0]=="end_header" ) break;
  }
  // get vertices for this cell
  vertices.resize(vertices_count, Eigen::NoChange);
  for(int i=0; i<vertices_count; i++){
  	getline(mesh_file, line);
  	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  	for(int j=0; j<3; j++) vertices(i,j) = std::stod(tokens[j]);
  }
  // get faces for this cell
  surface_triangles.resize(surface_triangles_count, Eigen::NoChange);
  for(int i=0; i<surface_triangles_count; i++){
  	getline(mesh_file, line);
  	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  	for(int j=0; j<3; j++) surface_triangles(i,j) = std::stoi(tokens[j+1]);
  }
  // get tetrahedra for this cell
  tetrahedrons.resize(tetrahedrons_count, Eigen::NoChange);
  for(int i=0; i<tetrahedrons_count; i++){
  	getline(mesh_file, line);
  	boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
  	for(int j=0; j<4; j++) tetrahedrons(i,j) = std::stoi(tokens[j+1]);
  }
  mesh_file.close();
}

void cCellMesh::write_mesh_file(std::string mesh_name)
{
  std::cout << "<CellMesh> writing mesh file: " + mesh_name << std::endl;

  // create the mesh file
  std::ofstream mesh_file(mesh_name); 
  mesh_file << "ply\n";
  mesh_file << "format ascii 1.0\n";
  mesh_file << "comment created by ply2ply\n";
  mesh_file << "element vertex " << vertices_count << "\n";
  mesh_file << "property float x\n";
  mesh_file << "property float y\n";
  mesh_file << "property float z\n";
  mesh_file << "element face " << surface_triangles_count << "\n";
  mesh_file << "property list uchar uint vertex_indices\n";
  mesh_file << "property int tri_type\n";  // for matlab simulation
  //mesh_file << "property int intensity\n";  // for paraview visualisation
  mesh_file << "property int duct_idx\n";
  mesh_file << "property float dist_from_duct\n";
  mesh_file << "property float dist_along_duct\n";
  mesh_file << "end_header\n";
  for(int i=0; i<vertices_count; i++){
  	mesh_file << vertices(i,0) << " ";
  	mesh_file << vertices(i,1) << " ";
  	mesh_file << vertices(i,2) << "\n";
  }
  for(int i=0; i<surface_triangles_count; i++){
  	mesh_file << "3 ";
  	for(int j=0; j<3; j++) mesh_file << surface_triangles(i,j) << " ";
	mesh_file << tri_types(i) << " " << t_di(i) << " ";
	mesh_file << t_dnd(i) << " " << t_dad(i) << "\n";
  }
  mesh_file.close();
}

void cCellMesh::print_info()
{
  std::cout << "<CellMesh> vertices_count: " << vertices_count << std::endl;
  std::cout << "<CellMesh> surface_triangles_count: " << surface_triangles_count << std::endl;
  std::cout << "<CellMesh> tetrahedrons_count: " << tetrahedrons_count << std::endl;
}

void cCellMesh::calc_nd(cDuctTree* dtree) // triangle to duct measurements 
{
  std::cout << "<CellMesh> Calculating duct distances and face types..." << std::endl;
  t_di.resize(surface_triangles_count, Eigen::NoChange);       // nearest duct segment index
  t_dnd.resize(surface_triangles_count, Eigen::NoChange);      // distance to the nearest duct segment
  t_dad.resize(surface_triangles_count, Eigen::NoChange);      // distance along nearest duct segment
  tri_types.resize(surface_triangles_count, Eigen::NoChange);
  int napical = 0;
  int nbasal = 0;
  int nbasolateral = 0;
  double apical_d = (ctype==INTERCALATED ? I_APICAL_D : S_APICAL_D);   // intercalated or striated?
  double basal_d = (ctype==INTERCALATED ? I_BASAL_D : S_BASAL_D);
  for (int n = 0; n < surface_triangles_count; n++){
    Vector3d v1 = vertices.row(surface_triangles(n,0));
    Vector3d v2 = vertices.row(surface_triangles(n,1));
    Vector3d v3 = vertices.row(surface_triangles(n,2));
    Vector3d cp = (v1 + v2 + v3) / 3.0;        // use center point of triangle for distance calculations
	dtree->get_dnd(cp, &t_di(n), &t_dnd(n), &t_dad(n));
	double rad = dtree->get_radius(t_di(n), t_dad(n));
	
	if(t_dnd(n) < (rad + apical_d)) { tri_types(n) = APICAL; napical++; }
	else if(t_dnd(n) > (rad + basal_d)) { tri_types(n) = BASAL; nbasal++; }
    else { tri_types(n) = BASOLATERAL; nbasolateral++; }
  }
  std::cout << "<CellMesh> apical:" << napical << "  basal:" << nbasal << "  basolateral:" << nbasolateral << std::endl;
  if(napical == 0) std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
  if(nbasal == 0) std::cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" << std::endl;
}

/*
void cCellMesh::calc_dfa()
{
  // calculate the distance from nodes to the lumen
  cLumenTree lumen(parent->out);
  n_dfa.resize(mesh_vals.vertices_count, Eigen::NoChange);
  for (int n = 0; n < mesh_vals.vertices_count; n++) { n_dfa(n) = lumen.get_dnl(mesh_vals.vertices.row(n)); }
  // calculate the distance from elements to the lumen
  e_dfa.resize(mesh_vals.tetrahedrons_count, Eigen::NoChange);
  for (int n = 0; n < mesh_vals.tetrahedrons_count; n++) {
    e_dfa(n) = (n_dfa(mesh_vals.tetrahedrons(n, 0)) + n_dfa(mesh_vals.tetrahedrons(n, 1)) +
                n_dfa(mesh_vals.tetrahedrons(n, 2)) + n_dfa(mesh_vals.tetrahedrons(n, 3))) /
               4.0;
  }
}

// determine the cell-to-cell connetivity data (this_triamgle, other_cell, other_triangle)
//	 do this by comparing surface triangle centers on this cell to those on every other cell
void cCellMesh::calc_common()
{
  MatrixN3d this_centers;  // this cell surface triangle centers
  MatrixN3d other_centers; // other cell surface triangle centers
  sMeshVals other_mesh;    // other mesh values

  utils::calc_tri_centers(this_centers, mesh_vals.vertices, mesh_vals.surface_triangles);
  int ci = id.find_last_of('c') + 1;               // to split the mesh id string, find the position of the last "c"
  int this_cell = atoi(id.substr(ci).c_str()) - 1; // extract this cell index from its mesh id
  common_triangles.resize(mesh_vals.surface_triangles_count, Eigen::NoChange); // overkill size for now
  common_triangles_count = 0;
  for (int other_cell = 0; other_cell < CELLS_COUNT; other_cell++) { // check against all the other cell meshes...
    if (other_cell == this_cell) continue;                           // don't check against itself
    std::string other_id = id.substr(0, ci) + std::to_string(other_cell + 1); // the other mesh id
    utils::read_mesh(other_id, other_mesh, parent->out);
    utils::calc_tri_centers(other_centers, other_mesh.vertices, other_mesh.surface_triangles);
    for (int this_tri = 0; this_tri < mesh_vals.surface_triangles_count; this_tri++) {
      for (int other_tri = 0; other_tri < other_mesh.surface_triangles_count; other_tri++) {
        Eigen::Vector3d p1 = this_centers.row(this_tri);
        Eigen::Vector3d p2 = other_centers.row(other_tri);
        if (p1 == p2) {
          common_triangles.row(common_triangles_count++) = Eigen::Vector3i(this_tri, other_cell, other_tri);
        }
      }
    }
  }
  common_triangles.conservativeResize(common_triangles_count, Eigen::NoChange); // actual size
}

void cCellMesh::calc_apical_basal()
{
  // determine the apical and the apical keep-out surface triangle indices
  MatrixN1i apical_keepout;                                                  // keep-out triangle indicies
  apical_keepout.resize(mesh_vals.surface_triangles_count, Eigen::NoChange); // overkill size for now
  int apical_keepout_count = 0;
  apical_triangles.resize(mesh_vals.surface_triangles_count, Eigen::NoChange); // overkill size for now
  apical_triangles_count = 0;
  for (int n = 0; n < mesh_vals.surface_triangles_count; n++) {
    double d = // triangle distance from apical
      (n_dfa(mesh_vals.surface_triangles(n, 0)) + n_dfa(mesh_vals.surface_triangles(n, 1)) +
       n_dfa(mesh_vals.surface_triangles(n, 2))) / 3.0;
    if (d < parent->p[APICALds]) apical_triangles(apical_triangles_count++) = n;
    if (d < (parent->p[APICALdl])) apical_keepout(apical_keepout_count++) = n;
  }
  apical_triangles.conservativeResize(apical_triangles_count, 1); // actual triangles count
  apical_keepout.conservativeResize(apical_keepout_count, 1);     // actual triangles count

  // save a list of the apical node indices (only used by post-processing plotting)
  MatrixN1i apical_nodes;                                         // apical node indices
  apical_nodes.resize(mesh_vals.vertices_count, Eigen::NoChange); // overkill size for now
  apical_nodes.setZero(mesh_vals.vertices_count);
  for (int n = 0; n < apical_triangles_count; n++) {
    for (int m = 0; m < 3; m++) {
      apical_nodes(mesh_vals.surface_triangles(apical_triangles(n), m)) = 1; // flag as apical
    }
  }
  int apical_nodes_count = 0;
  for (int n = 0; n < mesh_vals.vertices_count; n++) { // convert (in-place) what's left to a list of indices
    if (apical_nodes(n)) { apical_nodes(apical_nodes_count++) = n; }
  }
  apical_nodes.conservativeResize(apical_nodes_count, Eigen::NoChange); // actual size
  utils::save_matrix("apical_nodes_" + id + ".bin", apical_nodes_count * sizeof(int),
                     reinterpret_cast<char*>(apical_nodes.data()));

  // determine the basal triangle indices by considering all surface triangles
  //	 then eliminating the common triangles and the triangles that are too close to the lumen
  basal_triangles.resize(mesh_vals.surface_triangles_count, Eigen::NoChange); // overkill size for now
  basal_triangles.setOnes(mesh_vals.surface_triangles_count);
  for (int n = 0; n < common_triangles_count; n++) { // eliminate the common triangles
    basal_triangles(common_triangles(n, 0)) = 0;
  }
  for (int n = 0; n < apical_keepout_count; n++) { // eliminate the apical keepout triangles
    basal_triangles(apical_keepout(n)) = 0;
  }
  basal_triangles_count = 0;
  for (int n = 0; n < mesh_vals.surface_triangles_count; n++) { // convert (in-place) what's left to a list of indices
    if (basal_triangles(n)) { basal_triangles(basal_triangles_count++) = n; }
  }
  basal_triangles.conservativeResize(basal_triangles_count, Eigen::NoChange); // actual size

  // save a list of the basal node indices (only used by post-processing plotting)
  MatrixN1i basal_nodes;                                         // basal node indices
  basal_nodes.resize(mesh_vals.vertices_count, Eigen::NoChange); // overkill size for now
  basal_nodes.setZero(mesh_vals.vertices_count);
  for (int n = 0; n < basal_triangles_count; n++) {
    for (int m = 0; m < 3; m++) {
      basal_nodes(mesh_vals.surface_triangles(basal_triangles(n), m)) = 1; // flag as basal
    }
  }
  int basal_nodes_count = 0;
  for (int n = 0; n < mesh_vals.vertices_count; n++) { // convert (in-place) what's left to a list of indices
    if (basal_nodes(n)) { basal_nodes(basal_nodes_count++) = n; }
  }
  basal_nodes.conservativeResize(basal_nodes_count, Eigen::NoChange); // actual size
  utils::save_matrix("basal_nodes_" + id + ".bin", basal_nodes_count * sizeof(int), reinterpret_cast<char*>(basal_nodes.data()));
}

// calculate the distance from elements to basal surface
//	 use the average of the element-vertex to nearest-basal-triangle-vertex distances
void cCellMesh::calc_dfb()
{
  // get the basal vertices
  MatrixN1i basal_verts;                                         // the basal triangle vertices
  basal_verts.resize(mesh_vals.vertices_count, Eigen::NoChange); // overkill size for now
  basal_verts.setZero(mesh_vals.vertices_count);
  for (int n = 0; n < basal_triangles_count; n++) {
    Eigen::Vector3i vi = Eigen::Vector3i(mesh_vals.surface_triangles.row(basal_triangles(n)));
    for (int i = 0; i < 3; i++) basal_verts(vi(i)) = 1; // flag vertex as basal
  }
  int basal_verts_count = 0;
  for (int n = 0; n < mesh_vals.vertices_count; n++) { // convert flags to a list of basal indices
    if (basal_verts(n)) basal_verts(basal_verts_count++) = n;
  }
  basal_verts.conservativeResize(basal_verts_count, Eigen::NoChange); // actual size
  // calculate the (per node) node to nearest basal node distance
  MatrixN1d n_dfb;
  n_dfb.resize(mesh_vals.vertices_count, Eigen::NoChange);
  for (int n = 0; n < mesh_vals.vertices_count; n++) {
    Eigen::Vector3d p1 = Eigen::Vector3d(mesh_vals.vertices.row(n));
    n_dfb(n) = 1000.0; // large dummy value
    for (int m = 0; m < basal_verts_count; m++) {
      Eigen::Vector3d p2 = Eigen::Vector3d(mesh_vals.vertices.row(basal_verts(m)));
      double d = (p1 - p2).norm();
      if (d < n_dfb(n)) n_dfb(n) = d;
    }
  }
  // for each tet calculate e_dnb as the average it's vertex dnb's
  e_dfb.resize(mesh_vals.tetrahedrons_count, Eigen::NoChange);
  for (int n = 0; n < mesh_vals.tetrahedrons_count; n++) {
    e_dfb(n) = (n_dfb(mesh_vals.tetrahedrons(n, 0)) + n_dfb(mesh_vals.tetrahedrons(n, 1)) +
                n_dfb(mesh_vals.tetrahedrons(n, 2)) + n_dfb(mesh_vals.tetrahedrons(n, 3))) /
               4.0;
  }
}

void cCellMesh::identify_common_apical_triangles()
{
  parent->out << "<CellMesh> building common apical triangles list" << std::endl;
  // NOTE: this could be a feature of the mesh (i.e. included in the mesh file)

  common_apical_triangles.resize(apical_triangles_count, Eigen::NoChange);

  // mask of which triangles are apical
  std::vector<int> is_apical(mesh_vals.surface_triangles_count, 0);
  std::vector<int> is_apical_index(mesh_vals.surface_triangles_count, 0);
  for (int i = 0; i < apical_triangles_count; i++) {
    int this_tri = apical_triangles(i);
    is_apical[this_tri] = 1;
    is_apical_index[this_tri] = i;
  }

  // record which apical triangles have been included from the common triangles list,
  // so can add any remaining apical triangles to the list
  std::vector<int> apical_mask(apical_triangles_count, 0);

  // start with common triangles that are also apical triangles
  int count = 0;
  for (int i = 0; i < common_triangles_count; i++) {
    int this_tri = common_triangles(i, tTri);
    if (is_apical[this_tri]) {
      int apical_index = is_apical_index[this_tri];
      apical_mask[apical_index] = 1;
      for (int j = 0; j < CCONNCOUNT; j++) { common_apical_triangles(count, j) = common_triangles(i, j); }
      count++;
    }
  }
  parent->out << "<CellMesh> apical triangles common with other cells: " << count << std::endl;

  // now add on the apical triangles not in the common triangles list
  for (int i = 0; i < apical_triangles_count; i++) {
    if (not apical_mask[i]) {
      common_apical_triangles(count, tTri) = apical_triangles(i);
      common_apical_triangles(count, oCell) = parent->cell_number - 1; // needs to be zero-indexed
      common_apical_triangles(count, oTri) = apical_triangles(i);
      count++;
    }
  }
}
*/

