# -*- coding: utf-8 -*-
#
# Create hash table for ply files.
#
# J.rugis
# 03.05.2022
#
import glob as gb
import pyvista as pv
import numpy as np

#-------------------------------------------------------------------------------
# global constants
#-------------------------------------------------------------------------------

fname = "meshes/bsCells*.ply"
#-------------------------------------------------------------------------------
# function definitions
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#  MAIN PROGRAM
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

# get cell ply files sorted
flist = sorted(gb.glob(fname, recursive=False))
ncells = len(flist)          # total number of cells
bxmin = np.zeros(ncells) 

for n, fn in enumerate(flist):
  mesh = pv.read(fn)
  bxmin[n] = np.floor(1000*mesh.bounds[4]) # set to z min
  if (n%10 == 0): print()
  print(int(bxmin[n]), end=",")
print()
print()

#------------------------------------------------

