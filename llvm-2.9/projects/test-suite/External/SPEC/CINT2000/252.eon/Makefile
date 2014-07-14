LEVEL = ../../../..
RUN_OPTIONS     = chair.control.cook chair.camera chair.surfaces chair.cook.ppm ppm pixels_out.cook
STDOUT_FILENAME = cook_log.out
STDERR_FILENAME = cook_log.err
FP_ABSTOLERANCE := 0.005

CPPFLAGS = -include errno.h -DHAS_ERRLIST -DUSE_STRERROR -DSPEC_STDCPP -DNDEBUG

include $(LEVEL)/Makefile.config

ifeq ($(ARCH),PowerPC)
 CPPFLAGS += -DFMAX_IS_DOUBLE
endif

Source = $(addprefix $(SPEC_BENCH_DIR)/src/,  \
            ggCoverageSolidTexture.cc ggPathDielectricMaterial.cc ggBox2.cc \
            ggBox3.cc ggRasterSurfaceTexture.cc ggJitterSample1.cc \
            ggNRooksSample2.cc ggJitterSample2.cc ggRGBFPixel.cc \
            ggRotatingPinholeCamera.cc ggRandomSample2.cc ggRandomSample3.cc \
            ggSolidNoise2.cc ggSolidNoise3.cc ggSolidTexture.cc \
            ggOakSolidTexture.cc ggSpecularMaterial.cc ggNA.cc ggMaterial.cc \
            ggDiffuseMaterial.cc ggDielectricMaterial.cc ggConductorMaterial.cc \
            ggEdgeDiscrepancy.cc ggPolishedMaterial.cc ggFrameInterpolation.cc \
            ggQuaternion.cc ggSpline.cc ggONB2.cc ggONB3.cc ggFrame2.cc \
            ggFrame3.cc ggHMatrix3.cc ggSphere.cc ggRGBE.cc ggGamma.cc \
            ggBRDF.cc ggDiffuseBRDF.cc ggPhongBRDF.cc ggPolishedBRDF.cc \
            ggCamera.cc ggPinholeCamera.cc ggThinLensCamera.cc ggErr.cc \
            ggPixelFilter.cc ggTrianglePixelFilter.cc ggBoxPixelFilter.cc \
            ggFormat.cc ggGeometry.cc ggHAffineMatrix3.cc ggHShearMatrix3.cc \
            ggHBoxMatrix3.cc ggHPerspectiveMatrix3.cc ggHTranslationMatrix3.cc \
            ggHRigidBodyMatrix3.cc ggHRotationMatrix3.cc ggHScaleMatrix3.cc \
            ggHPoint3.cc ggPoint2.cc ggPoint3.cc ggOptics.cc ggPlane.cc \
            ggPolygon.cc ggGrayPixel_x.cc ggRGBPixel_x.cc \
            ggRay2.cc ggRay3.cc ggBoardFloorSolidTexture.cc ggVector2.cc \
            ggVector3.cc ggString.cc ggFineSpectrum.cc ggSpectrum.cc eon.cc \
            eonImageCalculator.cc mrObjectRecord.cc mrCoarsePixelRenderer.cc \
            mrIndirectPixelRenderer.cc mrRushmeierPixelRenderer.cc \
            mrBruteForcePixelRenderer.cc mrKajiyaPixelRenderer.cc \
            mrCookPixelRenderer.cc mrGrid.cc mrScene.cc mrCamera.cc \
            mrTriangle.cc mrFastTriangle.cc mrPolygon.cc mrXYDisk.cc \
            mrXZDisk.cc mrYZDisk.cc mrPhongAreaXYRectangleLuminaire.cc \
            mrPhongAreaXZRectangleLuminaire.cc \
            mrPhongAreaYZRectangleLuminaire.cc mrEmitter.cc mrImposter.cc \
            mrLinkedObjects.cc mrBox.cc mrXCylinder.cc mrYCylinder.cc \
            mrZCylinder.cc mrDiffuseAreaZCylinderLuminaire.cc \
            mrDiffuseCosineZCylinderLuminaire.cc \
            mrDiffuseVisibleAreaZCylinderLuminaire.cc mrXEllipticalCylinder.cc \
            mrYEllipticalCylinder.cc mrZEllipticalCylinder.cc \
            mrSpotAreaXYDiskLuminaire.cc mrDiffuseAreaXYRectangleLuminaire.cc \
            mrDiffuseAreaXZRectangleLuminaire.cc \
            mrDiffuseAreaYZRectangleLuminaire.cc mrSurfaceList.cc \
            mrDiffuseAreaTriangleLuminaire.cc mrDiffuseAreaSphereLuminaire.cc \
            mrDiffuseCosineSphereLuminaire.cc \
            mrDiffuseSolidAngleSphereLuminaire.cc mrShellLuminaire.cc \
            mrInstance.cc mrMaterial.cc mrPhongAreaTriangleLuminaire.cc \
            mrSolidTexture.cc mrSphere.cc mrSurface.cc mrSurfaceTexture.cc \
            mrXYRectangle.cc mrXZRectangle.cc mrYZRectangle.cc myrand.cc)

# Yes, we know this is an old crufty C++ benchmark.  Don't tell us about it GCC!
CXXFLAGS += -fno-exceptions -Wno-deprecated -Wno-non-template-friend -fpermissive

LDFLAGS = -lstdc++ -lm
LIBS    = -lm

include ../../Makefile.spec2000

