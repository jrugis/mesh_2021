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
# import _mesh_creation as m
# importlib.reload(_mesh_creation)
#
# m.init_mini_glnad()
# m.load_duct()   
# m.load_cells("striated")
# ...

import bpy
import mathutils
import os 

#-------------------------------------------------------------------------------
# class (structure) definitions
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# global constants
#-------------------------------------------------------------------------------

C_FILE = "_centers_"                  # cell locations file
INNER_DUCT = "_inner.ply"            # inner duct mesh file name
OUTTER_DUCT = "_outer.ply"           # outter duct mesh file name
DUCT_COLOR = (0.08, 0.8, 0.08, 1.0)  # inner duct color
C_RADIUS = 3.2                       # seed cell radius

# cell type dictionary
cell_types = {  
  "acinar"       : {"COLOR":(1.000, 0.055, 0.060, 1.0), "PRESSURE":1.8, "STIFFNESS":0.20, "RADIUS":3.2},
  "iCells" : {"COLOR":(1.000, 0.100, 0.120, 1.0), "PRESSURE":1.2, "STIFFNESS":0.11, "RADIUS":2.5},
  "sCells"     : {"COLOR":(1.000, 0.200, 0.240, 1.0), "PRESSURE":1.2, "STIFFNESS":0.11, "RADIUS":3.2}    
}


#-------------------------------------------------------------------------------
# function definitions
#-------------------------------------------------------------------------------

# create prototype cell
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
  #bpy.data.collections[cname].objects.link(bpy.context.object)
  #bpy.context.collection.objects.unlink(bpy.context.object) # unlink from main collection
  return

# initialize the mini_gland
def init_mini_gland():
  #bpy.context.scene.use_gravity = False
  bpy.context.scene.gravity = (0,0,0) # turn gravity off
  ## delete the default collection and objects
  collection = bpy.data.collections.get('Collection')
  for obj in collection.objects:
    bpy.data.objects.remove(obj, do_unlink=True)
  bpy.data.collections.remove(collection)
  # create the mini-gland collections  
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "sCells"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "iCells"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "a1Cells"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "a2Cells"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "Duct"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "V_Duct"))
  bpy.context.scene.collection.children.link(bpy.data.collections.new(name = "Acinii"))
  return

# load seed cells from file
def load_cells(cname):
  # delete existing cells
  #for obj in bpy.data.collections["Duct"].all_objects: obj.select_set(False)
  bpy.ops.object.select_all(action='DESELECT')
  for obj in bpy.data.collections[cname].all_objects: obj.select_set(True)
  bpy.ops.object.delete()
  # create new cells at locations from file
  create_proto_cell(cname) # new prototype cell
  mat = bpy.data.materials.new(name="mat.001")
  mat.diffuse_color = cell_types[cname]["COLOR"]

  with open(C_FILE+cname+".txt", 'r') as cfile:
    lines = cfile.readlines()
  for line in lines:
    bpy.ops.object.duplicate()              # duplicate
    bpy.context.object.name = cname+".001"    # duplicate names will auto increment
    bpy.context.object.data.materials[0] = mat #assign material to object
    bpy.context.object.location = mathutils.Vector(tuple(map(float, line.split(' '))))
    bpy.context.object.modifiers["Cloth"].settings.uniform_pressure_force = cell_types[cname]["PRESSURE"]
    bpy.context.object.modifiers["Cloth"].settings.compression_stiffness = cell_types[cname]["STIFFNESS"]
  bpy.data.objects.remove(bpy.data.objects['Icosphere']) # remove the prototype cell
  return

# load duct inner and outer from files
def load_duct():
  for obj in bpy.data.collections["Duct"].all_objects: obj.select_set(True)
  for obj in bpy.data.collections["Cells"].all_objects: obj.select_set(False)
  bpy.ops.object.delete()                     # delete existing duct

  bpy.ops.import_mesh.ply(filepath = OUTTER_DUCT)
  bpy.context.object.name = "outer.001"    # duplicate names will auto increment
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.thickness_outer = 1.0
  bpy.data.collections["Duct"].objects.link(bpy.context.object)
  bpy.context.collection.objects.unlink(bpy.context.object) # unlink from main collection

  bpy.ops.import_mesh.ply(filepath = INNER_DUCT)
  bpy.context.object.name = "inner.001"    # duplicate names will auto increment
  mat = bpy.data.materials.new(name="mat.001")
  mat.diffuse_color = DUCT_COLOR
  bpy.context.object.data.materials.append(mat) # add material to object
  bpy.ops.object.modifier_add(type = 'COLLISION')
  bpy.context.object.modifiers["Collision"].settings.thickness_outer = 1.0
  bpy.data.collections["Duct"].objects.link(bpy.context.object)
  bpy.context.collection.objects.unlink(bpy.context.object) # unlink from main collection
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
 
## save the cell meshes as ply files
def save_cell_meshes(cname):
  os.system("rm " + cname + "*.ply")
  #for obj in bpy.data.collections["Duct"].all_objects: obj.select_set(False)
  #for obj in bpy.data.collections["Cells"].all_objects: obj.select_set(False)
  bpy.ops.object.select_all(action='DESELECT')
  for obj in bpy.data.collections[cname].all_objects:
    obj.select_set(True)
    bpy.ops.export_mesh.ply(filepath="./meshes/b"+obj.name.replace('.', '_')+".ply", use_ascii=True,\
      use_normals=False, use_uv_coords=False, use_colors=False, use_selection=True)
    obj.select_set(False)
  return

## animate (to apply physics) 
#bpy.context.scene.frame_current = 1
#for frame in range(C_FRAMES):
#  bpy.context.view_layer.update()
#  bpy.context.scene.frame_current += 1

