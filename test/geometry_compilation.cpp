#include <pch.h>

#include "../engines/basic/include/asset_manager.h"

TEST(GeometryCompilation, HierarchyTest) {
    dbasic::AssetManager assetManager;
    ysError err = assetManager.CompileInterchangeFile("../../../test/geometry_files/instance_test", 1.0f, true);
    EXPECT_EQ(err, ysError::None);

    assetManager.LoadSceneFile("../../../test/geometry_files/instance_test", false);
    assetManager.ResolveNodeHierarchy();

    int sceneObjectCount = assetManager.GetSceneObjectCount();
    EXPECT_EQ(sceneObjectCount, 10);

    dbasic::SceneObjectAsset *instance1 = assetManager.GetSceneObject("Instance_1");
    dbasic::SceneObjectAsset *instance2 = assetManager.GetSceneObject("Instance_2");

    EXPECT_NE(instance1, nullptr);
    EXPECT_NE(instance2, nullptr);

    EXPECT_EQ(instance1->GetChildrenCount(), 2);
    EXPECT_EQ(instance1->GetChildrenCount(), 2);

    ysTransform transform1, transform2;
    dbasic::RenderSkeleton *skeleton1 = assetManager.BuildRenderSkeleton(&transform1, instance1);
    dbasic::RenderSkeleton *skeleton2 = assetManager.BuildRenderSkeleton(&transform2, instance2);

    int a = 0;
}

TEST(GeometryCompilation, ArmatureTest) {
    dbasic::AssetManager assetManager;
    ysError err = assetManager.CompileInterchangeFile("../../../test/geometry_files/armature_test", 1.0f, true);
    EXPECT_EQ(err, ysError::None);

    assetManager.LoadSceneFile("../../../test/geometry_files/armature_test", false);
    assetManager.ResolveNodeHierarchy();

    int a = 0;
}

TEST(GeometryCompilation, FullModelTest) {    
    dbasic::AssetManager assetManager;
    ysError err = assetManager.CompileInterchangeFile("../../../test/geometry_files/ant", 1.0f, true);
    EXPECT_EQ(err, ysError::None);
}
