/*
 * main.cpp
 *
 *	Created on: 14/09/2021
 *	Author: jrugis
 */

#include <filesystem>
//#include <fstream>
#include <iostream>
#include<string>
#include <time.h>
//#include <unistd.h>
#include <vector>

#include "cDuctTree.hpp"
#include "cCellMesh.hpp"

// the main program function for each mpi node
int main(int argc, char** args)
{
  struct timespec start, end;
  int duration;
  clock_gettime(CLOCK_REALTIME, &start);

  // read the duct tree file   
  cDuctTree* dtree = new cDuctTree("./meshes/_duct.ply");              

  // iterate through the cell mesh files outputing a copy of each one with additional data
  for (const auto &file : std::filesystem::directory_iterator("./meshes")){
	std::string fpath = std::filesystem::path(file.path());
	if(fpath.find("Cell_")==std::string::npos) continue;
    cCellMesh* cmesh = new cCellMesh(fpath, dtree);    // convert a cell mesh file
    cmesh->print_info();  
    delete cmesh;
  }
  delete dtree;

//  std::ofstream out;
//  out.open("bmsh.ply");

//  int nverts = 0;
//  int ntris = 0;
//  int ntets = 0;
//  for(int i=0; i<meshes.size(); i++){
//    meshes[i]->print_info();
//    nverts += meshes[i]->mesh_vals.vertices_count;
//	  ntris += meshes[i]->mesh_vals.surface_triangles_count;
//	  ntets += meshes[i]->mesh_vals.tetrahedrons_count;
//  }
//  out << "ply" << std::endl;
//  out << "format ascii 1.0" << std::endl;
//  out << "comment Mini-Gland mesh format 1.1" << std::endl;
//  out << "element acinii 1" << std::endl;
//  out << "property int ncells" << std::endl;
//  out << "property int icells" << std::endl;
//  out << "property int nlsegs" << std::endl;
//  out << "property int ilsegs" << std::endl;
//  out << "element duct 0" << std::endl;
//  out << "property int nicells" << std::endl;
//  out << "property int iicells" << std::endl;
//  out << "property int nscells" << std::endl;
//  out << "property int iscells" << std::endl;
//  out << "property int nlsegs" << std::endl;
//  out << "property int ilsegs" << std::endl;
//  out << "element lumen_node 73" << std::endl;
//  out << "property float x" << std::endl;
//  out << "property float y" << std::endl;
//  out << "property float z" << std::endl;
//  out << "property float radius" << std::endl;
//  out << "element lumen_segment 72" << std::endl;
//  out << "property int node_in" << std::endl;
//  out << "property int node_out" << std::endl;
//  out << "element cell 7" << std::endl;
//  out << "property int nverts" << std::endl;
//  out << "property int iverts" << std::endl;
//  out << "property int nfaces" << std::endl;
//  out << "property int ifaces" << std::endl;
//  out << "property int ntets" << std::endl;
//  out << "property int itets" << std::endl;
//  out << "element vertex " << std::to_string(nverts) << std::endl;
//  out << "property float x" << std::endl;
//  out << "property float y" << std::endl;
//  out << "property float z" << std::endl;
//  out << "element face " << std::to_string(ntris) << std::endl;
//  out << "property int v1" << std::endl;
//  out << "property int v2" << std::endl;
//  out << "property int v3" << std::endl;
//  out << "element tetrahedron " << std::to_string(ntets) << std::endl;
//  out << "property int v1" << std::endl;
//  out << "property int v2" << std::endl;
//  out << "property int v3" << std::endl;
//  out << "property int v4" << std::endl;
//  out << "end_header" << std::endl;
//  out << "7 0 72 0" << std::endl;             // single acinus
                                              // no duct 

//  std::ifstream infile("lumen_lines.txt");    // open lumen data file
//  std::string line;
//  std::getline(infile, line);
//  for(int i=0; i<73; i++){                    // 73 lumen nodes
//    std::getline(infile, line);
//    out << line << " 0.38" << std::endl;
//  }
//  std::getline(infile, line);
//  for(int i=0; i<72; i++){                    // 72 lumen segments
//    std::getline(infile, line);
//    out << line << std::endl;
//  }
//  infile.close();                            // close lumen data file
  
//  int vert_offset = 0;
//  int tri_offset = 0;
//  int tet_offset = 0;
//  for(int i=0; i<meshes.size(); i++){         // seven cells
//    out << std::to_string(meshes[i]->mesh_vals.vertices_count) << " ";
//	  out << std::to_string(vert_offset) << " ";
//    vert_offset += meshes[i]->mesh_vals.vertices_count;

//	  out << std::to_string(meshes[i]->mesh_vals.surface_triangles_count) << " ";
//	  out << std::to_string(tri_offset) << " ";
//    tri_offset += meshes[i]->mesh_vals.surface_triangles_count;

//	  out << std::to_string(meshes[i]->mesh_vals.tetrahedrons_count) << " ";
//	  out << std::to_string(tet_offset) << std::endl;
//    tet_offset += meshes[i]->mesh_vals.tetrahedrons_count;
//  }
  /*
    for(int i=0; i<meshes.size(); i++){         // seven cells - vertices
      for(int j=0; j<meshes[i]->mesh_vals.vertices_count; j++){
	    double p1 = meshes[i]->mesh_vals.vertices(j,0);
		double p2 = meshes[i]->mesh_vals.vertices(j,1);
		double p3 = meshes[i]->mesh_vals.vertices(j,2);
		out << boost::format("%1$0.3f %2$0.3f %3$0.3f\n") %p1 %p2 %p3;
	  }
  }
  for(int i=0; i<meshes.size(); i++){         // seven cells - surface triangles
  	for(int j=0; j<meshes[i]->mesh_vals.surface_triangles_count; j++){
  	  int v1 = meshes[i]->mesh_vals.surface_triangles(j,0);
  	  int v2 = meshes[i]->mesh_vals.surface_triangles(j,1);
  	  int v3 = meshes[i]->mesh_vals.surface_triangles(j,2);
  	  out << boost::format("%1% %2% %3%\n") %v1 %v2 %v3;
  	}
  }
  for(int i=0; i<meshes.size(); i++){         // seven cells - tetrahedra
  	for(int j=0; j<meshes[i]->mesh_vals.tetrahedrons_count; j++){
	  int v1 = meshes[i]->mesh_vals.tetrahedrons(j,0);
	  int v2 = meshes[i]->mesh_vals.tetrahedrons(j,1);
	  int v3 = meshes[i]->mesh_vals.tetrahedrons(j,2);
	  int v4 = meshes[i]->mesh_vals.tetrahedrons(j,3);
	  out << boost::format("%1% %2% %3% %4%\n") %v1 %v2 %v3 %v4;
  	}
  }
  */
//  out.close();
//  for(int i=0; i<meshes.size(); i++) delete meshes[i];

  clock_gettime(CLOCK_REALTIME, &end);
  duration = end.tv_sec - start.tv_sec;
  std::cout << "execution time: " << duration << "s" << std::endl;

  return 0;
}
