bl_info = {
    "name": "Delta Studios Interchange Format",
    "author": "Ange Yaghi",
    "version": (1, 0, 0),
    "blender": (2, 80, 0),
    "location": "File > Import-Export",
    "description": "Export Delta Studios Asset File",
    "warning": "",
    "category": "Import-Export"}

if "bpy" in locals():
    import importlib
    if "delta_interchange_export" in locals():
        importlib.reload(delta_interchange_export)

    if "delta_interchange_animation_export" in locals():
        importlib.reload(delta_interchange_animation_export)

    if "utilities" in locals():
        importlib.reload(utilities)

    if "object_list" in locals():
        importlib.reload(object_list)


import bpy
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from bpy_extras.io_utils import (
        ImportHelper,
        ExportHelper,
        orientation_helper,
        path_reference_mode,
        axis_conversion,
        )
        

@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportDeltaAsset(bpy.types.Operator, ExportHelper):
    """Save a Delta Studios Scene File"""

    bl_idname = "export_scene.delta"
    bl_label = 'Export Delta Asset'
    bl_options = {'PRESET'}

    filename_ext = ".dia"
    filter_glob: StringProperty(
            default="*.dia",
            options={'HIDDEN'},
            )

    # context group
    use_selection: BoolProperty(
            name="Selection Only",
            description="Export selected objects only",
            default=True,
            )

    use_mesh_modifiers: BoolProperty(
            name="Apply Modifiers",
            description="Apply modifiers",
            default=True,
            )
    references_only: BoolProperty(
            name="References Only",
            description="Export only object transforms and their name without any mesh data",
            default=True,
            )

    check_extension = True

    def execute(self, context):
        from . import delta_interchange_export

        from mathutils import Matrix
        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "check_existing",
                                            "filter_glob",
                                            ))

        global_matrix = axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up,
                                         ).to_4x4()

        keywords["global_matrix"] = global_matrix
        return delta_interchange_export.write_scene_file(context, **keywords)


class ExportDeltaAnimation(bpy.types.Operator, ExportHelper):
    """Save a Delta Studios Animation File"""

    bl_idname = "export_animation.delta"
    bl_label = 'Export Delta Animation'
    bl_options = {'PRESET'}

    filename_ext = ".dimo"
    filter_glob: StringProperty(
            default="*.dimo",
            options={'HIDDEN'},
            )

    check_extension = True

    def execute(self, context):
        from . import delta_interchange_animation_export

        keywords = self.as_keywords(ignore=("filter_glob", "check_existing"))

        return delta_interchange_animation_export.write_animation_file(context, **keywords)


def menu_func_export(self, context):
    self.layout.operator(ExportDeltaAsset.bl_idname, text="Delta Interchange Asset (.dia)")


def menu_fun_export_animation(self, context):
    self.layout.operator(ExportDeltaAnimation.bl_idname, text="Delta Interchange Animation (.dimo)")


classes = (
    ExportDeltaAsset,
    ExportDeltaAnimation
)


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)
    bpy.types.TOPBAR_MT_file_export.append(menu_fun_export_animation)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
    bpy.types.TOPBAR_MT_file_export.remove(menu_fun_export_animation)

    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()
