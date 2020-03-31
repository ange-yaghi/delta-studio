#!python3

import sys
import package_build
import os

ARGUMENTS = [
    'architecture',
    'configuration',
    'vc_crt_path'
]

VARIABLES = [
    ('project_name', "delta-studios"),
    ('script_path', os.path.dirname(os.path.realpath(__file__))),
    ('root', "{script_path}/.."),
    ('dependencies', "{root}/dependencies"),
    ('dll_dir', "{dependencies}/runtime/{architecture}"),
    ('workspace', "{root}/workspace"),
    ('output', "{workspace}/build/delta"),
    ('vs_build_output', "{root}/project/{architecture}/{configuration}")
]

CLEAN_DIRECTORIES = [
    '{output}'
]

NEW_DIRECTORIES = [
    '{output}',
    '{output}/engines',
    '{output}/engines/basic',
    '{output}/physics',
    '{output}/lib',
    '{output}/lib/{architecture}',
    '{output}/lib/{architecture}/{configuration}',
    '{output}/runtime/{architecture}'
]

RESOURCES = [
    # Basic engine
    package_build.Resource('headers', '{root}/engines/basic/include', '{output}/engines/basic/include'),
    package_build.Resource('fonts', '{root}/engines/basic/fonts', '{output}/engines/basic/fonts'),
    package_build.Resource('shaders', '{root}/engines/basic/shaders', '{output}/engines/basic/shaders'),
    package_build.Resource('binaries', '{vs_build_output}/delta-basic-engine.lib', '{output}/lib/{architecture}/{configuration}', resource_type='file'),
    package_build.Resource('binaries', '{vs_build_output}/delta-basic-engine.pdb', '{output}/lib/{architecture}/{configuration}', resource_type='file', optional=True),

    # Physics engine
    package_build.Resource('headers', '{root}/physics/include', '{output}/physics/include'),
    package_build.Resource('binaries', '{vs_build_output}/delta-physics.lib', '{output}/lib/{architecture}/{configuration}', resource_type='file'),
    package_build.Resource('binaries', '{vs_build_output}/delta-physics.pdb', '{output}/lib/{architecture}/{configuration}', resource_type='file', optional=True),

    # Core
    package_build.Resource('headers', '{root}/include', '{output}/include'),
    package_build.Resource('runtime dependencies', '{dll_dir}/', '{output}/runtime/{architecture}', resource_type='dlls'),
    package_build.Resource('binaries', '{vs_build_output}/delta-core.lib', '{output}/lib/{architecture}/{configuration}', resource_type='file'),
    package_build.Resource('binaries', '{vs_build_output}/delta-core.pdb', '{output}/lib/{architecture}/{configuration}', resource_type='file', optional=True),
    package_build.Resource('visual studio redist', '{vc_crt_path}', '{output}/runtime/{architecture}', resource_type='dlls')
]

if __name__ == "__main__":
    package_build.run(sys.argv, ARGUMENTS, CLEAN_DIRECTORIES, NEW_DIRECTORIES, RESOURCES, VARIABLES)
