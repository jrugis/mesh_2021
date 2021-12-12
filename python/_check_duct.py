# -*- coding: utf-8 -*-
#
# Check duct ply file.
#
# J.rugis
# 10.12.2021
#
import numpy as np
from mayavi.mlab import *

#-------------------------------------------------------------------------------
# global constants
#-------------------------------------------------------------------------------

#DUCT_IN = "_duct.ply"
DUCT_IN = "_lumen_a1.ply"

#-------------------------------------------------------------------------------
# class definitions
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# function definitions
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#  MAIN PROGRAM
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

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
dnodes = np.zeros((ndnodes,3))  # node coordinates
for i in range(ndnodes):            # duct node data
  tokens = dfile.readline().split()
  for j in range(3):
    dnodes[i,j] = float(tokens[j])
dsegs = np.zeros((ndsegs,2))   # duct segment indices
for i in range(ndsegs):            # duct segment data
  tokens = dfile.readline().split()
  for j in range(2):
    dsegs[i,j] = int(tokens[j])
dfile.close()

# Data for a three-dimensional line
#in_pts = np.array([dnodes[int(n)] for n in dsegs[:,0]])

for i in range(ndsegs):
  seg = dsegs[i]
  #print(int(seg[0]+1),int(seg[1]+1))
  xpts = [dnodes[int(seg[0]),0], dnodes[int(seg[1]),0]]
  ypts = [dnodes[int(seg[0]),1], dnodes[int(seg[1]),1]]
  zpts = [dnodes[int(seg[0]),2], dnodes[int(seg[1]),2]]
  #print("c",i+1,"n",int(seg[0])+1, dnodes[int(seg[0])])
  #print(seg[1], dnodes[int(seg[1])])
  #points3d(dnodes[int(seg[0]),0], dnodes[int(seg[0]),1],dnodes[int(seg[0]),2])
  plot3d(xpts, ypts, zpts, tube_radius=0.5)

show()

#------------------------------------------------

