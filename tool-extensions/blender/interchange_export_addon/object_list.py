import mathutils

class ObjectType(object):
    GEOMETRY = 0x0
    BONE = 0x1
    GROUP = 0x2
    PLANE = 0x3
    INSTANCE = 0x4
    EMPTY = 0x5
    ARMATURE = 0x6
    UNDEFINED = -1


class Object(object):
    def __init__(self, obj):
        self.obj = obj
        self.index = -1
        self.instance_index = -1
        self.empty = False
        self.parent_index = -1
        self.object_type = ObjectType.UNDEFINED
        self.global_matrix = None


class Armature(object):
    def __init__(self, obj):
        self.obj = obj
        self.bone_map = {}


class ObjectList(object):
    def __init__(self):
        self.object_list = []
        self.armatures = []

    def get_object_count(self):
        return len(self.object_list)

    def expand_object(self, obj, parent_transform):
        if obj.is_instancer:
            new_object = self.add_empty(obj)
            new_object.global_matrix = parent_transform @ obj.matrix_world

            if obj.instance_type == 'COLLECTION':
                parent_map = {}
                new_sub_objects = []

                for sub_obj in obj.instance_collection.objects:
                    if sub_obj.is_instancer:
                        new_instance = self.expand_object(sub_obj, new_object.global_matrix)
                        new_instance.parent_index = new_object.index

                        index = self.get_index(sub_obj)
                        parent_map[index] = new_instance.index
                    else:
                        index = self.get_index(sub_obj)
                        ref = self.get(index)

                        new_instance = self.add_instance(sub_obj, ref.index)
                        new_instance.parent_index = new_object.index
                        new_instance.global_matrix = new_object.global_matrix @ ref.global_matrix

                        parent_map[ref.index] = new_instance.index

                    new_sub_objects.append(new_instance)

                for sub_obj in new_sub_objects:
                    if sub_obj.obj.parent is not None:
                        index = self.get_index(sub_obj.obj.parent)
                        mapped_index = parent_map[index]

                        sub_obj.parent_index = mapped_index
        else:
            if obj.type == 'EMPTY':
                new_object = self.add_empty(obj)
            elif obj.type == 'ARMATURE':
                new_object = self.add_armature(obj)
            else:
                new_object = self.add_object(obj)

            new_object.global_matrix = parent_transform @ obj.matrix_world
        
        return new_object


    def add_referenced_geometry(self, obj):
        if obj.is_instancer:
            if obj.instance_type == 'COLLECTION':
                for sub_obj in obj.instance_collection.objects:
                    if not sub_obj.is_instancer:
                        index = self.get_index(sub_obj)
                        if index is None:
                            new_object = self.add_object(sub_obj)
                            new_object.global_matrix = sub_obj.matrix_world
                    else:
                        self.add_referenced_geometry(sub_obj)


    def generate_object_list(self, objects):
        for obj in objects:
            self.add_referenced_geometry(obj)

        for obj in objects:
            self.expand_object(obj, mathutils.Matrix.Identity(4))

        for obj in self.object_list:
            if obj.obj.parent is not None and obj.parent_index == -1:
                obj.parent_index = self.resolve_parent(obj)

        return self

    def add_object(self, obj):
        n = len(self.object_list)
        new_object = Object(obj)
        new_object.index = n
        new_object.instance_index = -1
        new_object.object_type = ObjectType.GEOMETRY

        self.object_list.append(new_object)

        return new_object

    def get_armature(self, armature):
        for arm in self.armatures:
            if armature == arm.obj:
                return arm

        return None

    def resolve_parent(self, obj):
        if obj.obj.parent_type == 'BONE':
            return self.get_armature(obj.obj.parent).bone_map[obj.obj.parent_bone].index
        else:
            return self.get_index(obj.obj)

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

        self.object_list.append(new_object)

        return new_object

    def add_empty(self, obj):
        n = len(self.object_list)
        new_object = Object(obj)
        new_object.index = n
        new_object.instance_index = -1
        new_object.empty = True
        new_object.object_type = ObjectType.EMPTY

        self.object_list.append(new_object)

        return new_object

    def resolve_bone_transform(self, bone):
        if bone.global_matrix is not None:
            return bone.global_matrix

        bone.global_matrix = self.resolve_bone_transform(self.get(bone.parent_index)) @ bone.obj.matrix_local
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
            self.object_list.append(new_bone)

            new_armature.bone_map[bone.name] = new_bone

            n += 1

        for name, bone in new_armature.bone_map.items():
            parent = bone.obj.parent

            if parent is None:
                bone.parent_index = top_level.index
            else:
                parent_bone_name = parent.name
                bone.parent_index = new_armature.bone_map[parent_bone_name].index

        for name, bone in new_armature.bone_map.items():
            self.resolve_bone_transform(bone)

        return top_level
