# -*- coding: utf-8 -*-
#
# Combine initial ply files into single custom ply file.
#
# J.rugis
# 08.09.2021
#
import glob as gb
import pyvista as pv
import numpy as np
from typing import NamedTuple

#-------------------------------------------------------------------------------
# global constants
#-------------------------------------------------------------------------------

OUT = "_mini_gland.ply"
DUCT_IN = "_duct.ply"

#-------------------------------------------------------------------------------
# class definitions
#-------------------------------------------------------------------------------

# duct segment structure
class sDSeg(NamedTuple): 
    node_in: int
    node_out: int

#-------------------------------------------------------------------------------
# function definitions
#-------------------------------------------------------------------------------

# get face type (apical, basal, basolateral) based on distance from duct center line
# NOTE: for now, duct needs to be centered on z-axis (simplifies calculations!)
def get_ftype(verts, id, od): # face vertices, duct inner diameter, duct outer diamter
  #c = np.mean(verts, axis=0)
  #d = np.sqrt(c[0]**2 + c[1]**2)
  #if((d - id/2.0) < 1.5): return('A')    # apical
  #elif((od/2.0 - d) < 1.0): return('B')  # basal
  #else : return('L')                     # basolateral
  return('A')
  
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#  MAIN PROGRAM
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

#------------------------------------------------
# get cell ply files sorted list and summary data
#------------------------------------------------
flist = sorted(gb.glob("Cell*.ply", recursive=False))
ncells = len(flist)          # total number of cells
tnverts = 0                  # total number of vertices
tnfaces = 0                  # total number of faces
for fname in flist:
  mesh = pv.read(fname)
  tnverts += mesh.n_points
  tnfaces += mesh.n_faces

#------------------------------------------------
# get duct data from the duct ply file
#------------------------------------------------
dfile = open(DUCT_IN, "r")
for ln in dfile:
  if ln.startswith("element duct_node"): break
ndnodes = int(ln.split()[-1])       # number of duct nodes
for ln in dfile:
  if ln.startswith("element duct_segment"): break
ndsegs = int(ln.split()[-1])        # number of duct segments
for ln in dfile:
  if ln.startswith("end_header"): break
dnodes = []  # node coordinates & radius
for i in range(ndnodes):            # duct node data
  tokens = dfile.readline().split()
  dnodes.append([float(s) for s in tokens])
dsegs = []   # duct segment indices
dstypes = [] # duct segment cell types
for i in range(ndsegs):            # duct segment data
  tokens = dfile.readline().split()
  dsegs.append(sDSeg(int(tokens[0]), int(tokens[1])))
  dstypes.append(tokens[2])
dfile.close()

#------------------------------------------------
# write out a single consolidated custom ply file
#------------------------------------------------
pfile = open(OUT, "w")

# ---write out the file header
pfile.write("ply\n")
pfile.write("format ascii 1.0\n")

pfile.write("element duct_node " + str(ndnodes) + "\n")
pfile.write("property float x\n")
pfile.write("property float y\n")
pfile.write("property float z\n")
pfile.write("property float radius\n")

pfile.write("element duct_segment " + str(ndsegs) + "\n")
pfile.write("property int node_in\n")
pfile.write("property int node_out\n")
pfile.write("property uchar cell_type\n")

pfile.write("element cell " + str(ncells) + "\n")
pfile.write("property int nverts\n")
pfile.write("property int iverts\n")
pfile.write("property int nfaces\n")
pfile.write("property int ifaces\n")
pfile.write("property int ntets\n")
pfile.write("property int itets\n")
pfile.write("property uchar cell_type\n")

pfile.write("element vertex " + str(tnverts) + "\n")
pfile.write("property float x\n")
pfile.write("property float y\n")
pfile.write("property float z\n")

pfile.write("element face " + str(tnfaces) + "\n")
pfile.write("property int v1\n")
pfile.write("property int v2\n")
pfile.write("property int v3\n")
pfile.write("property uchar region_type\n")
pfile.write("property int nearest_duct\n")
pfile.write("property float duct_point\n")

pfile.write("element tetrahedron 0\n")
pfile.write("property int v1\n")
pfile.write("property int v2\n")
pfile.write("property int v3\n")
pfile.write("property int v4\n")
pfile.write("end_header\n")

# --- write out the duct data
pfile.write("0 0 ")                
pfile.write(str(len(flist)) + " ") # only striated duct cells for now
pfile.write("0 ")                 
pfile.write(str(ndsegs) + " ")     # only duct lumen for now
pfile.write("0 \n")

# --- write out the duct node data
for n in dnodes:
  pfile.write("{:.2f} {:.2f} {:.2f} {:.2f}\n".format(*n))
# --- write out the lumen segment data
for i, s in enumerate(dsegs):
  # -- segment details
  pfile.write("{:d} {:d} {}\n".format(s.node_in, s.node_out, dstypes[i]))

# --- write out the cell data
pi_offset = 0  # point index offset
fi_offset = 0  #  face   "     " 
for fname in flist:
  pfile.write(str(mesh.n_points) + " ")
  pfile.write(str(pi_offset) + " ")
  pi_offset += mesh.n_points
  # -- number faces in this cell
  pfile.write(str(mesh.n_faces) + " ")
  pfile.write(str(fi_offset) + " ")
  fi_offset += mesh.n_faces
  # -- number of tetrahedrons in this cell
  pfile.write("0 0 ")
  # -- cell type
  pfile.write(fname.split('.')[0][-1] + "\n") # get cell type from file name

# --- write out the vertex data (all cells)
for fname in flist:
  mesh = pv.read(fname)
  for p in mesh.points:
    pfile.write("{:.2f} {:.2f} {:.2f}\n".format(*p))

# --- write out oriented face data (all cells)
#bad_cell = False  # cell mesh integrity check
pi_offset = 0     # point index offset
for i, fname in enumerate(flist):
  #n_apical = 0    # number of apical faces for this cell
  mesh = pv.read(fname)
  for pi in mesh.faces.reshape((-1,4))[:, 1:]:  # three indices per row
    # -- vertex indices
    pfile.write("{:d} {:d} {:d} \n".format(*(pi+pi_offset)))
    ## -- face type
    #d = dsegs[0]
    #ftype = get_ftype(mesh.points[pi], d.inner_diameter, d.outer_diameter)
    #if(ftype == 0): n_apical += 1
    ##pfile.write(str(ftype) + "\n")
  pi_offset += mesh.n_points
  #if(n_apical == 0): bad_cell = True # check if cell mesh has no apical faces

# --- write out oriented tetrahedron data (all cells)
# NOTE: no tets for now

# --- done
pfile.close()
#if(bad_cell):
#  exit(50)  # special error return code if any bad cells

#------------------------------------------------

