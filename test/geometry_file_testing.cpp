#include <pch.h>

#include "../include/yds_interchange_file_0_0.h"
#include "../include/yds_geometry_export_file.h"

#include <fstream>

TEST(GeometryFile, SanityCheck) {
    ysInterchangeFile0_0 f;
}

TEST(GeometryFile, LoadCheck) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("fake_file.fake");
    EXPECT_EQ(r, ysError::CouldNotOpenFile);

    f.Close();
}

TEST(GeometryFile, LoadValidCheck) {
    std::fstream testFile("test_fake_file", std::ios::out);

    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/empty_file.dia");
    EXPECT_EQ(r, ysError::None);

    EXPECT_EQ(f.GetMajorVersion(), 0);
    EXPECT_EQ(f.GetMinorVersion(), 0);

    f.Close();
}

TEST(GeometryFile, LoadInvalidheck) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/invalid_file.dia");
    EXPECT_EQ(r, ysError::InvalidFileType);

    f.Close();
}

TEST(GeometryFile, LoadCube) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/cube.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    EXPECT_EQ(r, ysError::None);

    EXPECT_EQ(obj.MaterialName, "Material");
    EXPECT_EQ(obj.Name, "Cube");

    EXPECT_EQ(obj.NormalIndices.size(), 12);
    EXPECT_EQ(obj.Normals.size(), 8);
    EXPECT_EQ(obj.TangentIndices.size(), 12);
    EXPECT_EQ(obj.Tangents.size(), 24);
    EXPECT_EQ(obj.UVChannels.size(), 1);
    EXPECT_EQ(obj.UVIndices[0].size(), 12);
    EXPECT_EQ(obj.VertexIndices.size(), 12);
    EXPECT_EQ(obj.Vertices.size(), 8);

    EXPECT_TRUE(obj.Validate());
    EXPECT_EQ(f.GetObjectCount(), 1);
    f.Close();
}

TEST(GeometryFile, RipByNormals) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/flat_cube.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    EXPECT_EQ(r, ysError::None);

    EXPECT_EQ(obj.MaterialName, "Material");
    EXPECT_EQ(obj.Name, "Cube");

    EXPECT_EQ(obj.NormalIndices.size(), 12);
    EXPECT_EQ(obj.Normals.size(), 6);
    EXPECT_EQ(obj.TangentIndices.size(), 12);
    EXPECT_EQ(obj.Tangents.size(), 6);
    EXPECT_EQ(obj.UVChannels.size(), 1);
    EXPECT_EQ(obj.UVIndices[0].size(), 12);
    EXPECT_EQ(obj.VertexIndices.size(), 12);
    EXPECT_EQ(obj.Vertices.size(), 8);

    EXPECT_TRUE(obj.Validate());

    obj.RipByNormals();
    EXPECT_EQ(obj.Vertices.size(), 24);
    EXPECT_TRUE(obj.Validate());

    f.Close();
}

TEST(GeometryFile, RipByTangents) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/cube.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    EXPECT_EQ(r, ysError::None);

    EXPECT_TRUE(obj.Validate());

    EXPECT_EQ(obj.Vertices.size(), 8);
    obj.RipByTangents();
    EXPECT_EQ(obj.Vertices.size(), 24);
    EXPECT_TRUE(obj.Validate());

    f.Close();
}

TEST(GeometryFile, RipByUVs) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/cube.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    EXPECT_EQ(r, ysError::None);

    EXPECT_TRUE(obj.Validate());

    EXPECT_EQ(obj.Vertices.size(), 8);
    obj.RipByUVs();
    EXPECT_EQ(obj.Vertices.size(), 24);
    EXPECT_TRUE(obj.Validate());

    f.Close();
}

TEST(GeometryFile, FullRip) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/cube.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    EXPECT_EQ(r, ysError::None);

    EXPECT_TRUE(obj.Validate());

    EXPECT_EQ(obj.Vertices.size(), 8);
    obj.RipByTangents();
    obj.RipByNormals();
    obj.RipByUVs();
    EXPECT_EQ(obj.Vertices.size(), 24);
    EXPECT_TRUE(obj.Validate());

    f.Close();
}

TEST(GeometryFile, ConnectedUVsRip) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/cube_connected_uvs.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    EXPECT_EQ(r, ysError::None);

    EXPECT_TRUE(obj.Validate());

    EXPECT_EQ(obj.Vertices.size(), 8);
    obj.RipByNormals();
    EXPECT_EQ(obj.Vertices.size(), 8);

    obj.RipByUVs();
    EXPECT_EQ(obj.Vertices.size(), 18);

    EXPECT_TRUE(obj.Validate());

    f.Close();
}

TEST(GeometryFile, FullRipCompilation) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/cube.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    obj.RipByTangents();
    obj.RipByNormals();
    obj.RipByUVs();

    f.Close();

    int objectCount = 1;

    ysGeometryExportFile exportFile;
    exportFile.Open("../../../test/geometry_files/cube.dca");
    exportFile.WriteCustomData((void *)&objectCount, sizeof(int));
    exportFile.WriteObject(&obj);
    exportFile.Close();

    std::fstream file("../../../test/geometry_files/cube.dca", std::ios::in | std::ios::binary);

    int objectCountCompiled;
    file.read((char *)&objectCountCompiled, sizeof(int));

    EXPECT_EQ(objectCountCompiled, 1);

    ysGeometryExportFile::ObjectOutputHeader header;
    file.read((char *)&header, sizeof(ysGeometryExportFile::ObjectOutputHeader));
    file.close();
}

TEST(GeometryFile, FullModel) {
    ysInterchangeFile0_0 f;
    ysError r = f.Open("../../../test/geometry_files/ant.dia");
    EXPECT_EQ(r, ysError::None);

    ysInterchangeObject obj;
    r = f.ReadObject(&obj);
    obj.RipByTangents();
    obj.RipByNormals();
    obj.RipByUVs();

    f.Close();
}
