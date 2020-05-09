from . object_list import ObjectList
from . utilities import write_32_bit_unsigned, write_32_bit_signed, write_32_bit_float, write_string, Vector2, Vector3, Quaternion
import bpy
import re
import mathutils
import math

def write_id_header(f):
    MAJOR_VERSION = 0x00
    MINOR_VERSION = 0x01
    MAGIC_NUMBER = 0xFEA4AA
    EDITOR_ID = 0x3
    COMPILATION_STATUS_RAW = int('0x1', base=16)
    
    write_32_bit_unsigned(MAGIC_NUMBER, f)
    write_32_bit_unsigned(MAJOR_VERSION, f)
    write_32_bit_unsigned(MINOR_VERSION, f)
    write_32_bit_unsigned(EDITOR_ID, f)
    write_32_bit_unsigned(COMPILATION_STATUS_RAW, f)


class FileHeader(object):
    def __init__(self, action_count):
        self.action_count = action_count

    def write(self, f):
        write_32_bit_unsigned(self.action_count, f)


class Action(object):
    def __init__(self, name, curves):
        self.name = name
        self.curves = curves
        
    def write(self, f):
        write_string(self.name, f, 256)
        write_32_bit_unsigned(len(self.curves), f)

        for curve in self.curves:
            curve.write(f)


class InterpolationMode:
    Bezier = 0x0
    Linear = 0x1


class CurveType:
    # Full-data types
    LocationVec = 0x1
    RotationQuat = 0x2

    # Individual channel types
    LocationX = 0x3
    LocationY = 0x4
    LocationZ = 0x5

    RotationQuatW = 0x6
    RotationQuatX = 0x7
    RotationQuatY = 0x8
    RotationQuatZ = 0x9

    Undefined = 0x0


class Curve(object):
    def __init__(self, curve_type):
        self.target = ""
        self.keyframe_count = 0
        self.curve_type = curve_type

        self.keyframes = []
        
    def write(self, f):
        write_string(self.target, f, 256)
        write_32_bit_unsigned(self.keyframe_count, f)
        write_32_bit_unsigned(self.curve_type, f)

        for keyframe in self.keyframes:
            keyframe.write(f)


class Keyframe(object):
    def __init__(self, timestamp, value):
        self.timestamp = timestamp
        self.value = value

        self.interpolation_mode = 0x0

        self.l_handle_x = 0.0
        self.l_handle_y = 0.0

        self.r_handle_x = 0.0
        self.r_handle_y = 0.0

    def write(self, f):
        write_32_bit_float(self.timestamp, f)
        write_32_bit_float(self.value, f)

        write_32_bit_unsigned(self.interpolation_mode, f)

        write_32_bit_float(self.l_handle_x, f)
        write_32_bit_float(self.l_handle_y, f)

        write_32_bit_float(self.r_handle_x, f)
        write_32_bit_float(self.r_handle_y, f)


def write_animation_file(context, filepath):
    objects = context.selected_objects

    armature = objects[0]

    object_list = ObjectList()
    object_list.generate_object_list([armature])

    all_actions = bpy.data.actions

    # There's probably a better way of doing this
    # without regex
    pattern = re.compile(r"pose\.bones\[\"(.*)\"\].(.*)")

    actions = []
    for action in all_actions:
        action_name = action.name

        curves = []
        for fcurve in action.fcurves:
            data_path = fcurve.data_path
            property_index = fcurve.array_index

            result = pattern.match(data_path)
            if result is None:
                continue

            bone_name = result.group(1)
            target = result.group(2)

            new_curve = None
            if target == "rotation_quaternion":
                if property_index == 0:
                    new_curve = Curve(CurveType.RotationQuatW)
                elif property_index == 1:
                    new_curve = Curve(CurveType.RotationQuatX)
                elif property_index == 2:
                    new_curve = Curve(CurveType.RotationQuatY)
                elif property_index == 3:
                    new_curve = Curve(CurveType.RotationQuatZ)
            elif target == "location":
                if property_index == 0:
                    new_curve = Curve(CurveType.LocationX)
                elif property_index == 1:
                    new_curve = Curve(CurveType.LocationY)
                elif property_index == 2:
                    new_curve = Curve(CurveType.LocationZ)

            if new_curve is None:
                continue

            for keyframe in fcurve.keyframe_points:
                f_key = Keyframe(keyframe.co.x, keyframe.co.y)

                if keyframe.interpolation == "LINEAR":
                    f_key.interpolation_mode = InterpolationMode.Linear
                elif keyframe.interpolation == "BEZIER":
                    f_key.interpolation_mode = InterpolationMode.Bezier
                else:
                    f_key.interpolation_mode = InterpolationMode.Linear

                f_key.l_handle_x = keyframe.handle_left.x
                f_key.l_handle_y = keyframe.handle_left.y

                f_key.r_handle_x = keyframe.handle_right.x
                f_key.r_handle_y = keyframe.handle_right.y

                new_curve.keyframes.append(f_key)

            new_curve.target = bone_name
            new_curve.keyframe_count = len(new_curve.keyframes)
            curves.append(new_curve)

        if len(curves) > 0:
            new_action = Action(action_name, curves)
            actions.append(new_action)
            
    with open(filepath, 'wb') as f:
        write_id_header(f)

        file_header = FileHeader(len(actions))
        file_header.write(f)

        for action in actions:
            action.write(f)
        
    return {'FINISHED'}
