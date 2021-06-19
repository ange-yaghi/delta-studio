import mathutils
import math
import bpy
from math import radians
from . object_list import LightData, ObjectList, Object, ObjectType
from . utilities import write_32_bit_float, write_32_bit_unsigned, write_32_bit_signed, write_string, Vector2, Vector3, Quaternion
        
        
def write_id_header(f):
    MAJOR_VERSION = 0x0
    MINOR_VERSION = 0x1
    MAGIC_NUMBER = 0xFEA4A
    EDITOR_ID = 0x3
    COMPILATION_STATUS_RAW = int('0x1', base=16)
    
    write_32_bit_unsigned(MAGIC_NUMBER, f)
    write_32_bit_unsigned(MAJOR_VERSION, f)
    write_32_bit_unsigned(MINOR_VERSION, f)
    write_32_bit_unsigned(EDITOR_ID, f)
    write_32_bit_unsigned(COMPILATION_STATUS_RAW, f)
        

class SceneHeader(object):
    def __init__(self):
        self.object_count = 0
        
    def write(self, f):        
        write_32_bit_unsigned(self.object_count, f)
        
        
class ObjectTransformation(object):
    def __init__(self):
        self.position = Vector3()
        self.orientation_euler = Vector3()
        self.orientation = Quaternion()
        self.scale = Vector3()
        
    def write(self, f):
        self.position.write(f)
        self.orientation_euler.write(f)
        self.orientation.write(f)
        self.scale.write(f)
        

class ObjectInformationHeader(object):
    def __init__(self):
        self.name = ""
        self.material_name = ""
        self.model_index = -1
        self.parent_index = -1
        self.instance_index = -1
        self.object_type = -1
        
    def write(self, f):
        write_string(self.name, f, 256)
        write_string(self.material_name, f, 256)

        write_32_bit_signed(self.model_index, f)
        write_32_bit_signed(self.parent_index, f)
        write_32_bit_signed(self.instance_index, f)
        write_32_bit_signed(self.object_type, f)


class LightInformationHeader(object):
    def __init__(self, light_data: LightData):
        self.light_type = light_data.light_type
        self.intensity = light_data.intensity
        self.color = light_data.color
        self.cutoff_distance = light_data.cutoff_distance
        self.distance = light_data.distance

        # Spot information
        self.spot_angular_size = light_data.spot_angular_size
        self.spot_fade = light_data.spot_fade

    def write(self, f):
        write_32_bit_unsigned(self.light_type, f)
        write_32_bit_float(self.intensity, f)
        write_32_bit_float(self.cutoff_distance, f)
        write_32_bit_float(self.distance, f)
        self.color.write(f)

        write_32_bit_float(self.spot_angular_size, f)
        write_32_bit_float(self.spot_fade, f)

        
class GeometryInformation(object):
    def __init__(self):
        self.num_uv_channels = 0
        self.num_vertices = 0
        self.num_normals = 0
        self.num_tangents = 0
        self.num_faces = 0
        
    def write(self, f):
        write_32_bit_unsigned(self.num_uv_channels, f)
        write_32_bit_unsigned(self.num_vertices, f)
        write_32_bit_unsigned(self.num_normals, f)
        write_32_bit_unsigned(self.num_tangents, f)
        write_32_bit_unsigned(self.num_faces, f)
        
        
class FaceIndexSet(object):
    def __init__(self):
        self.u = 0
        self.v = 0
        self.w = 0
        
    def set(self, v_index, index):
        if v_index == 0:
            self.u = index
        elif v_index == 1:
            self.v = index
        elif v_index == 2:
            self.w = index
        
    def write(self, f):
        write_32_bit_unsigned(self.u, f)
        write_32_bit_unsigned(self.v, f)
        write_32_bit_unsigned(self.w, f)
        

class UVChannel(object):
    def __init__(self):
        self.uv_count = 0

    def write(self, f):
        write_32_bit_unsigned(self.uv_count, f)

        
def veckey2d(vec):
    return round(vec[0], 4), round(vec[1], 4)


def veckey3d(v):
    return round(v.x, 4), round(v.y, 4), round(v.z, 4)


def triangulate_mesh(me):
    import bmesh
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()
    
    
def write_object_mesh(object_record: Object, object_list, global_transform, apply_modifiers, depsgraph, f):
    parent_index = object_record.parent_index
    parent = None
    if parent_index != -1:
        parent = object_list.get(parent_index)

    obj_transform = object_record.global_matrix
    if parent is not None:
        inverse_parent = parent.global_matrix.copy()
        inverse_parent.invert()
        obj_transform = inverse_parent @ object_record.global_matrix

    obji_header = ObjectInformationHeader()
    obji_header.name = object_record.name

    if (object_record.object_type in (ObjectType.GEOMETRY, ObjectType.INSTANCE)):
        obji_header.material_name = object_record.material.name if object_record.material is not None else ''
    else:
        obji_header.material_name = ''
    
    obji_header.model_index = object_record.index
    obji_header.parent_index = object_record.parent_index
    obji_header.instance_index = object_record.instance_index
    obji_header.object_type = object_record.object_type
    obji_header.write(f)
    
    final_transform = obj_transform

    # Fill in object transformation header
    euler = final_transform.to_euler()
    quat = final_transform.to_quaternion()
    translation = final_transform.to_translation()
    scale = final_transform.to_scale()
    
    objt_header = ObjectTransformation()
    objt_header.position = Vector3.from_bvec(translation)
    objt_header.orientation_euler = Vector3.from_bvec(euler)
    objt_header.orientation = Quaternion.from_bquat(quat)
    objt_header.scale = Vector3.from_bvec(scale)
    objt_header.write(f)

    if object_record.object_type == ObjectType.LIGHT:
        LightInformationHeader(object_record.light_data).write(f)

    if object_record.object_type != ObjectType.GEOMETRY:
        return
    
    try:
        obj = object_record.obj
        obj_copy = obj.evaluated_get(depsgraph) if apply_modifiers else obj.original
        me = obj_copy.to_mesh()
    except:
        return
    
    # Triangulate mesh
    triangulate_mesh(me)
    
    me_verts = me.vertices[:]
    face_index_pairs = [(face, index) for index, face in enumerate(me.polygons)]
    
    # Extract normals
    me.calc_normals_split()
    loops = me.loops

    no_key = no_val = None
    normals_to_idx = {}
    no_get = normals_to_idx.get
    loops_to_normals = [0] * len(loops)
    all_normals = []
    no_unique_count = 0
    for face, f_index in face_index_pairs:
        for l_idx in face.loop_indices:
            no_key = veckey3d(loops[l_idx].normal)
            no_val = no_get(no_key)
            if no_val is None:
                no_val = normals_to_idx[no_key] = no_unique_count
                all_normals.append(loops[l_idx].normal)
                no_unique_count += 1
            loops_to_normals[l_idx] = no_val
    del normals_to_idx, no_get, no_key, no_val

    # Calculate tangents
    me.calc_tangents()

    tan_key = tan_val = None
    tangent_to_idx = {}
    tan_get = tangent_to_idx.get
    loops_to_tangents = [0] * len(loops)
    all_tangents = []
    no_unique_count = 0
    for face, f_index in face_index_pairs:
        for l_idx in face.loop_indices:
            tan_key = veckey3d(loops[l_idx].tangent)
            tan_val = tan_get(tan_key)
            if tan_val is None:
                tan_val = tangent_to_idx[tan_key] = no_unique_count
                all_tangents.append(loops[l_idx].tangent)
                no_unique_count += 1
            loops_to_tangents[l_idx] = tan_val
    del tangent_to_idx, tan_get, tan_key, tan_val

    # Populate geometry header
    geometry_header = GeometryInformation()
    geometry_header.num_uv_channels = len(me.uv_layers)
    geometry_header.num_vertices = len(me_verts)
    geometry_header.num_faces = len(face_index_pairs)
    geometry_header.num_normals = len(all_normals)
    geometry_header.num_tangents = len(all_tangents)
    geometry_header.write(f)
    
    # Write vertices
    for vert in me_verts:
        vec = Vector3(vert.co[0], vert.co[1], vert.co[2])
        vec.write(f)
                
    # Extract UVs
    uv_channel_data = []
    uv_face_mapping = []
    
    uv = f_index = uv_index = uv_key = uv_val = uv_ls = None
    
    uv_dict = {}
    uv_get = uv_dict.get
    for uv_channel in me.uv_layers:
        uv_face_mapping_channel = [None] * len(face_index_pairs)
        
        channel_data = []
        uv_channel_data.append(channel_data)
        
        uv_dict = {}
        uv_get = uv_dict.get
        for face, f_index in face_index_pairs:
            uv_ls = uv_face_mapping_channel[f_index] = []
            for uv_index, l_index in enumerate(face.loop_indices):
                uv = uv_channel.data[l_index].uv
                
                # Perform a lookup
                uv_key = veckey2d(uv)
                uv_val = uv_get(uv_key)
                if uv_val is None:
                    uv_val = uv_dict[uv_key] = len(channel_data)
                    channel_data.append(uv[:])
                    
                uv_ls.append(uv_val)
                    
        uv_face_mapping.append(uv_face_mapping_channel)

        del uv_dict, uv, f_index, uv_index, uv_ls, uv_get, uv_key, uv_val
        # Only need uv_unique_count and uv_face_mapping
        
    # Write UVs
    for channel_data in uv_channel_data:
        h = UVChannel()
        h.uv_count = len(channel_data)
        h.write(f)
        
        for uv_pair in channel_data:
            uv = Vector2(uv_pair[0], uv_pair[1])
            uv.write(f)

    # Write normals
    for normal in all_normals:
        vec = Vector3(normal.x, normal.y, normal.z)
        vec.write(f)

    # Write tangents
    for tangent in all_tangents:
        vec = Vector3(tangent.x, tangent.y, tangent.z)
        vec.write(f)
        
    # Write faces
    for face, f_index in face_index_pairs:
        f_v = [(vi, me_verts[v_idx], l_idx)
            for vi, (v_idx, l_idx) in enumerate(zip(face.vertices, face.loop_indices))]
        
        # Vertex indices
        s = FaceIndexSet()
        for vi, v, li in f_v:
            s.set(vi, v.index)
        s.write(f)

        # Normal indices
        s = FaceIndexSet()
        for vi, v, li in f_v:
            s.set(vi, loops_to_normals[li])
        s.write(f)

        # Tangent indices
        s = FaceIndexSet()
        for vi, v, li in f_v:
            s.set(vi, loops_to_tangents[li])
        s.write(f)
        
        for uv_channel in uv_face_mapping:
            uv_set = FaceIndexSet()
            
            for vi, v, li in f_v:
                uv_index = uv_channel[f_index][vi]
                uv_set.set(vi, uv_index)       
            uv_set.write(f)
        
    # Clean up mesh
    obj_copy.to_mesh_clear()


def write_scene_file(context, filepath, *, 
        use_selection, 
        use_mesh_modifiers, 
        references_only,
        global_matrix):
            
    depsgraph = context.evaluated_depsgraph_get()
    scene = context.scene
    
    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')
    
    if use_selection:
        objects = context.selected_objects
    else:
        objects = scene.objects

    object_list = ObjectList()
    object_list.generate_object_list(objects)
            
    with open(filepath, 'wb') as f:
        write_id_header(f)
        
        scene_header = SceneHeader()
        scene_header.object_count = object_list.get_object_count()
        scene_header.write(f)
        
        for obj in object_list.object_list:
            write_object_mesh(obj, object_list, global_matrix, use_mesh_modifiers, depsgraph, f)
        
    return {'FINISHED'}
