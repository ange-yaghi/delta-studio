import mathutils

from . utilities import Vector3

class ObjectType(object):
    GEOMETRY = 0x0
    BONE = 0x1
    GROUP = 0x2
    PLANE = 0x3
    INSTANCE = 0x4
    EMPTY = 0x5
    ARMATURE = 0x6
    LIGHT = 0x7
    UNDEFINED = -1


class Object(object):
    def __init__(self, obj):
        self.obj = obj
        self.name = ""
        self.material = None
        self.index = -1
        self.instance_index = -1
        self.empty = False
        self.parent_index = -1
        self.object_type = ObjectType.UNDEFINED
        self.global_matrix = None
        self.light_data = None


class LightType(object):
    SPOT = 0
    POINT = 1
    SUN = 2


class LightData(object):
    def __init__(self):
        self.light_type = None
        self.intensity = Vector3()
        self.color = None
        self.distance = None
        self.cutoff_distance = None

        # Spot shape
        self.spot_angular_size = 0
        self.spot_fade = 0


class Armature(object):
    def __init__(self, obj):
        self.obj = obj
        self.bone_map = {}


class ObjectList(object):
    def __init__(self):
        self.collections = dict()
        self.object_list = []
        self.armatures = []

    def get_object_count(self):
        return len(self.object_list)

    def expand_collection(self, collection):
        if collection.name in self.collections:
            return self.collections[collection.name]
        
        collection_object = self.add_collection(collection.name)

        for sub_obj in collection.objects:
            new_instance = self.expand_object(sub_obj, collection_object.global_matrix)
            new_instance.parent_index = collection_object.index

        return collection_object

    def expand_object(self, obj, parent_transform):
        if obj.is_instancer:
            if obj.instance_type == 'COLLECTION':
                collection = self.expand_collection(obj.instance_collection)
                new_object = self.add_instance(obj, collection.index)
            else:
                new_object = self.add_empty(obj)
            
            new_object.global_matrix = parent_transform @ obj.matrix_world
        else:
            if obj.type == 'EMPTY':
                new_object = self.add_empty(obj)
            elif obj.type == 'ARMATURE':
                new_object = self.add_armature(obj)
            elif obj.type == 'LIGHT':
                new_object = self.add_light(obj)
            else:
                new_object = self.add_object(obj)

            new_object.global_matrix = parent_transform @ obj.matrix_world
        
        return new_object

    def generate_object_list(self, objects):
        for obj in objects:
            self.expand_object(obj, mathutils.Matrix.Identity(4))

        for obj in self.object_list:
            if obj.obj is not None:
                if obj.obj.parent is not None and obj.parent_index == -1:
                    obj.parent_index = self.resolve_parent(obj)

        return self

    def get_armature(self, armature):
        for arm in self.armatures:
            if armature == arm.obj:
                return arm

        return None

    def resolve_parent(self, obj):
        if obj.obj.parent_type == 'BONE':
            return self.get_armature(obj.obj.parent).bone_map[obj.obj.parent_bone].index
        else:
            return self.get_index(obj.obj.parent)

    def get_index(self, obj):
        for o in self.object_list:
            if o.obj == obj:
                return o.index
        
        return None

    def get(self, index):
        return self.object_list[index]

    def add_instance(self, obj, instance_index):
        n = len(self.object_list)
        new_object = Object(obj)
        new_object.index = n
        new_object.instance_index = instance_index
        new_object.object_type = ObjectType.INSTANCE
        new_object.name = obj.name

        self.object_list.append(new_object)

        return new_object

    def add_object(self, obj):
        n = len(self.object_list)
        new_object = Object(obj)
        new_object.index = n
        new_object.instance_index = -1
        new_object.material = obj.active_material
        new_object.name = obj.name

        if (obj.type == 'MESH'):
            new_object.object_type = ObjectType.GEOMETRY
        elif (obj.type == 'EMPTY'):
            new_object.object_type = ObjectType.EMPTY

        self.object_list.append(new_object)

        return new_object

    def add_empty(self, obj):
        n = len(self.object_list)
        new_object = Object(obj)
        new_object.index = n
        new_object.instance_index = -1
        new_object.empty = True
        new_object.object_type = ObjectType.EMPTY
        new_object.name = obj.name

        self.object_list.append(new_object)

        return new_object

    def add_collection(self, name):
        new_object = Object(None)
        new_object.index = len(self.object_list)
        new_object.material = None
        new_object.name = name
        new_object.instance_index = -1
        new_object.empty = True
        new_object.object_type = ObjectType.EMPTY
        new_object.global_matrix = mathutils.Matrix()

        self.collections[name] = new_object
        self.object_list.append(new_object)

        return new_object        

    def add_light(self, obj):
        new_object = Object(obj)
        new_object.index = len(self.object_list)
        new_object.instance_index = -1
        new_object.empty = False
        new_object.object_type = ObjectType.LIGHT
        new_object.name = obj.name

        new_object.light_data = LightData()
        new_object.light_data.intensity = obj.data.energy
        new_object.light_data.color = Vector3(obj.data.color.r, obj.data.color.g, obj.data.color.b)
        new_object.light_data.cutoff_distance = obj.data.cutoff_distance
        new_object.light_data.distance = obj.data.distance

        if obj.data.type == 'SPOT':
            new_object.light_data.light_type = LightType.SPOT
            new_object.light_data.spot_angular_size = obj.data.spot_size
            new_object.light_data.spot_fade = obj.data.spot_blend
        elif obj.data.type == 'POINT':
            new_object.light_data.light_type = LightType.POINT
        elif obj.data.type == 'SUN':
            new_object.light_data.light_type = LightType.SUN

        self.object_list.append(new_object)

        return new_object

    def resolve_bone_transform(self, bone, armature_transform):
        if bone.global_matrix is not None:
            return bone.global_matrix

        bone.global_matrix = armature_transform @ bone.obj.matrix_local
        return bone.global_matrix
    
    def add_armature(self, obj):
        n = len(self.object_list)
        data = obj.data

        top_level = self.add_empty(obj)
        top_level.global_matrix = obj.matrix_world
        n += 1

        new_armature = Armature(obj)
        self.armatures.append(new_armature)
        for bone in data.bones:
            new_bone = Object(bone)
            new_bone.index = n
            new_bone.instance_index = -1
            new_bone.empty = False
            new_bone.object_type = ObjectType.BONE
            new_bone.name = bone.name
            self.object_list.append(new_bone)

            new_armature.bone_map[bone.name] = new_bone

            n += 1

        for _, bone in new_armature.bone_map.items():
            parent = bone.obj.parent

            if parent is None:
                bone.parent_index = top_level.index
            else:
                parent_bone_name = parent.name
                bone.parent_index = new_armature.bone_map[parent_bone_name].index

        for _, bone in new_armature.bone_map.items():
            self.resolve_bone_transform(bone, top_level.global_matrix)

        return top_level
