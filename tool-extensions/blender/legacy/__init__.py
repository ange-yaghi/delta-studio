bl_info = {
    "name": "Delta Studios Scene Format",
    "author": "Ange Yaghi",
    "version": (1, 0, 0),
    "blender": (2, 80, 0),
    "location": "File > Import-Export",
    "description": "Export Delta Studios Asset File",
    "warning": "",
    "category": "Import-Export"}

if "bpy" in locals():
    import importlib
    if "delta_export" in locals():
        importlib.reload(delta_export)


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
    bl_label = 'Export Delta Scene'
    bl_options = {'PRESET'}

    filename_ext = ".ysc"
    filter_glob: StringProperty(
            default="*.ysc",
            options={'HIDDEN'},
            )

    # context group
    use_selection: BoolProperty(
            name="Selection Only",
            description="Export selected objects only",
            default=False,
            )

    use_mesh_modifiers: BoolProperty(
            name="Apply Modifiers",
            description="Apply modifiers",
            default=True,
            )
    object_references: BoolProperty(
            name="Use Asset References",
            description="Export only object transforms and their name",
            default=True,
            )

    check_extension = True

    def execute(self, context):
        from . import delta_export

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
        return delta_export.write_scene_file(context, **keywords)


def menu_func_export(self, context):
    self.layout.operator(ExportDeltaAsset.bl_idname, text="Delta Scene (.ysc)")


classes = (
    ExportDeltaAsset,
)


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()
