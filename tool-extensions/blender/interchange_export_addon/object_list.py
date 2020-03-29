class ObjectType(object):
    GEOMETRY = 0x0
    BONE = 0x1
    GROUP = 0x2
    PLANE = 0x3
    INSTANCE = 0x4
    EMPTY = 0x5
    UNDEFINED = -1


class Object(object):
    def __init__(self, obj):
        self.obj = obj
        self.index = -1
        self.instance_index = -1
        self.empty = False
        self.parent_index = -1
        self.object_type = ObjectType.UNDEFINED


class ObjectList(object):
    def __init__(self):
        self.object_list = []

    def add_object(self, obj):
        n = len(self.object_list)
        new_object = Object(obj)
        new_object.index = n
        new_object.instance_index = -1
        new_object.object_type = ObjectType.GEOMETRY

        self.object_list.append(new_object)

        return new_object

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