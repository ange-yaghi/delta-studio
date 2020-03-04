def write_32_bit_unsigned(i, f):
    b = i.to_bytes(4, byteorder='little', signed=False)
    f.write(b)
    
    
def write_8_bit_unsigned(i ,f):
    b = i.to_bytes(1, byteorder='little', signed=False)
    f.write(b)
    
    
def write_8_bit_bool(b, f):
    write_8_bit_unsigned(BitBool.convert(b), f)
        

def write_32_bit_bool(b, f):
    write_32_bit_unsigned(BitBool.convert(b), f)
    

def write_32_bit_float(fl, f):
    b = f.to_bytes(4, byteorder='little')
    f.write(b)
    
    
def write_padding(bytes, f):
    PADDING = 0xFE
    
    for i in range(bytes):
        write_8_bit_unsigned(PADDING, f)


class ObjectType(object):
    GEOMETRY = 0x0
    BONE = 0x1
    GROUP = 0x2
    PLANE = 0x3
    INSTANCE = 0x4
    UNDEFINED = -1


class BitBool(object):
    TRUE = 0x1
    FALSE = 0x0
    
    @staticmethod
    def convert(b):
        if b:
            return TRUE
        else:
            return FALSE
        
        
class ObjectInformationHeader(object):
    def __init__(self):
        self.model_index = 0
        self.parent_index = 0
        self.parent_instance = 0
        self.object_type = ObjectType.UNDEFINED
        self.uses_bones = False
        self.skeleton_index = 0


class Vector3(object):
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
        
    def write(self, f):
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)
        write_32_bit_float(self.z, f)
        
        
class Vector4(object):
    def __init__(self, x, y, z, w=1.0):
        self.x = x;
        self.y = y;
        self.z = z;
        self.w = w;
        
    def write(self, f):
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)
        write_32_bit_float(self.z, f)
        write_32_bit_float(self.w, f)
        
        
class Quaternion(object):
    def __init__(self, w, x, y, z):
        self.w = w;
        self.x = x;
        self.y = y;
        self.z = z;
        
    def write(self, f):
        write_32_bit_float(self.x, f)
        write_32_bit_float(self.y, f)
        write_32_bit_float(self.z, f)
        write_32_bit_float(self.w, f)     
        

class SceneHeader(object):
    def __init__(self):
        self.animation_data = False
        self.bone_data = False
        self.texture_coordinate_data = False
        self.material_data = False
        self.smoothing_data_check = False
        self.object_count = 0
        
    def write(self, f):
        write_8_bit_bool(self.animation_data, f)
        write_8_bit_bool(bone_data, f)
        write_8_bit_bool(texture_coordinate_data, f)
        write_8_bit_bool(material_data, f)
        write_8_bit_bool(smoothing_data_check, f)
        write_padding(3)
        
        write_32_bit_unsigned(object_count, f)
        
        
class ObjectTransformation(object):
    def __init__(self):
        self.position = Vector4()
        self.orientation_euler = Vector3()
        self.orientation = Quaternion()
        self.scale = Vector3()
        
    def write(self, f):
        self.position.write(f)
        self.orientation_euler.write(f)
        self.orientation.write(f)
        self.scale.write(f)

    
def write_header(f):
    FILE_VERSION = 0x5
    MAGIC_NUMBER = 0x50F1AA
    EDITOR_ID = 0x3
    COMPILATION_STATUS_RAW = 0x1
    
    write_32_bit_unsigned(MAGIC_NUMBER, f)
    write_32_bit_unsigned(FILE_VERSION, f)
    write_32_bit_unsigned(EDITOR_ID, f)
    write_32_bit_unsigned(COMPILATION_STATUS_RAW, f)


def write_scene_file(context, filepath, *, 
        use_selection, 
        use_mesh_modifiers, 
        object_references,
        global_matrix):
            
    scene = context.scene
    
    if use_selection:
        objects = context.selected_objects
    else:
        objects = scene.objects
            
    with open(filepath, 'wb') as f:
        write_header(f)
        write_scene_header(10, f)
        
    return {'FINISHED'}
