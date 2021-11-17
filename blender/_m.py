# -*- coding: utf-8 -*-
#
# Blender ontersction mini-gland mesh creation
#
# J.rugis
# 05.09.2021
#

#-------------------------------------------------------------------------------
# setup and useage
#-------------------------------------------------------------------------------
# ---- mac terminal ----
# cd Desktop/nesi00119/mesh_2021
# /Applications/Blender.app/Contents/MacOS/Blender
#
# ---- blender / python console----
# import importlib
# import _m as m
# importlib.reload(_m)
#
# ...

import bpy
import glob
import mathutils
import os 

#-------------------------------------------------------------------------------
# class (structure) definitions
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# global constants
#-------------------------------------------------------------------------------

C_FILE = "_centers_"                      # cell locations file PREFIX
INNER = "_inner_NEW.ply"                  # inner duct mesh file name
OUTER = "_outer_NEW.ply"                  # outer ...
ACINUS_OUTER = "_acinus_outer_NEW.ply"
ACINUS = "_acinus_X.ply"

DUCT_COLOR = (0.08, 0.8, 0.08, 1.0)  # inner duct color
C_RADIUS = 3.2                       # seed cell radius

# cell type dictionary
cell_types = {  
  "acinii" : {"COLOR":(0.8,   0.8,   0.8,   1.0), "PRESSURE":1.1, "STIFFNESS":0.07, "RADIUS":4.5},
  "a1"     : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.1, "STIFFNESS":0.11, "RADIUS":3.2},
  "a2"     : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.1, "STIFFNESS":0.11, "RADIUS":3.2},
  "a3"     : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.1, "STIFFNESS":0.11, "RADIUS":3.2},
  "a4"     : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.1, "STIFFNESS":0.11, "RADIUS":3.2},
  "a5"     : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.1, "STIFFNESS":0.11, "RADIUS":3.2},
  "a6"     : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.1, "STIFFNESS":0.11, "RADIUS":3.2},
  "iCells" : {"COLOR":(1.000, 0.100, 0.120, 1.0), "PRESSURE":1.2, "STIFFNESS":0.11, "RADIUS":2.5},
  "sCells" : {"COLOR":(1.000, 0.200, 0.240, 1.0), "PRESSURE":1.2, "STIFFNESS":0.11, "RADIUS":3.2}    
}


#-------------------------------------------------------------------------------
# function definitions
#-------------------------------------------------------------------------------

# initialize the mini_gland
def create_proto_cell(cname):
  bpy.ops.mesh.primitive_ico_sphere_add(subdivisions = 5, radius = cell_types[cname]["RADIUS"], location = (0.0, 0.0, 0.0))
  mat = bpy.data.materials.new(name="mat"+cname)
  bpy.context.object.data.materials.append(mat) # add material to object
  bpy.ops.object.modifier_add(type = 'CLOTH')
  bpy.context.object.modifiers["Cloth"].settings.use_internal_springs = False
  bpy.context.object.modifiers["Cloth"].settings.use_pressure = True
  bpy.context.object.modifiers["Cloth"].settings.tension_stiffness = 0.02
  bpy.context.object.modifiers["Cloth"].collision_settings.distance_min = 0.1
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.cloth_friction = 0
  return

def init_mini_gland():
  bpy.context.scene.gravity = (0,0,0) # turn gravity off
  # create the mini-gland collections  
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "Duct"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "Acinus_outer"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "acinii"))
  return


# load duct inner and outer from files
def load_duct_inner():
  bpy.ops.import_mesh.ply(filepath = INNER)
  bpy.context.object.name = "inner.001"    # duplicate names will auto increment
  mat = bpy.data.materials.new(name="mat.001")
  mat.diffuse_color = DUCT_COLOR
  bpy.context.object.data.materials.append(mat) # add material to object
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.thickness_outer = 1.0
  return

def load_duct_outer():
  bpy.ops.import_mesh.ply(filepath = OUTER)
  bpy.context.object.name = "outer.001"    # duplicate names will auto increment
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.thickness_outer = 1.0
  return

# load acinii overall outer constraint from file
def load_acinii():
  bpy.ops.import_mesh.ply(filepath = ACINUS_OUTER)
  bpy.context.object.name = "acinus_outer.001"    # duplicate names will auto increment
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.thickness_outer = 1.0
  return

# load a single acinus outer constraint from file
def load_acinus(aname):
  bpy.ops.import_mesh.ply(filepath = ACINUS.replace("X", aname))
  bpy.context.object.name = "acinus_" + aname    # duplicate names will auto increment
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.thickness_outer = 1.0
  bpy.context.object.collision.use_culling = False
  return

# save mesh object locations to a file
def save_locations(cname):
  cfile = open(C_FILE+cname+".txt", 'w')
  for obj in bpy.data.collections[cname].all_objects:
    obj.select_set(True)
    cfile.write("{:.2f} {:.2f} {:.2f}\n".format(obj.location.x,obj.location.y,obj.location.z))
    obj.select_set(False)
  cfile.close()
  return

# create new seed cells at locations from file
def load_cells(cname):
  create_proto_cell(cname) # new prototype cell
  mat = bpy.data.materials.new(name="mat.001")
  mat.diffuse_color = cell_types[cname]["COLOR"]

  with open(C_FILE+cname+".txt", 'r') as cfile:
    lines = cfile.readlines()
  for line in lines:
    bpy.ops.object.duplicate()                 # duplicate
    bpy.context.object.name = cname+".001"     # duplicate names will auto increment
    bpy.context.object.data.materials[0] = mat #assign material to object
    bpy.context.object.location = mathutils.Vector(tuple(map(float, line.split(' '))))
    bpy.context.object.modifiers["Cloth"].settings.uniform_pressure_force = cell_types[cname]["PRESSURE"]
    bpy.context.object.modifiers["Cloth"].settings.compression_stiffness = cell_types[cname]["STIFFNESS"]
  bpy.data.objects.remove(bpy.data.objects['Icosphere']) # remove the prototype cell
  return

## save the cell meshes as ply files
def save_cell_meshes(cname):
  for obj in bpy.data.collections[cname].all_objects:
    obj.select_set(True)
    bpy.ops.export_mesh.ply(filepath="./meshes/b"+obj.name.replace('.', '_')+".ply", use_ascii=True,\
      use_normals=False, use_uv_coords=False, use_colors=False, use_selection=True)
    obj.select_set(False)
  return

# load mesh files
def load_cell_meshes(cname):
  mat = bpy.data.materials.new(name="mat."+cname)
  mat.diffuse_color = cell_types[cname]["COLOR"]
  for fname in glob.glob("./meshes/b"+cname+"*.ply"):
    bpy.ops.import_mesh.ply(filepath = fname)
    bpy.context.object.name = cname+".001"    # duplicate names will auto increment
    bpy.context.object.data.materials.append(mat) # add material to object
  return

# smooth mesh objects
def smoooth_meshes(cname):
  for obj in bpy.data.collections[cname].all_objects:
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_add(type='SMOOTH')
    if cname == "iCells":
      bpy.context.object.modifiers["Smooth"].factor = 0.8
      bpy.context.object.modifiers["Smooth"].iterations = 5
    else :
      bpy.context.object.modifiers["Smooth"].factor = 1.2
      bpy.context.object.modifiers["Smooth"].iterations = 6
    obj.select_set(False)
  return



