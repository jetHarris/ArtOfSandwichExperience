#include "globalGameStuff.h"

class cShaderHolder
{
public:
	cShaderHolder();
	void loadUniforms(int shaderID);
	GLint renderPassNumber_LocID;
	GLboolean renderPassVertexNum;
	GLboolean shadowStuff;
	GLint lightSpaceMatrixLocation;
	GLint shadowLocId;
	GLint uniLoc_mView;
	GLint uniLoc_mProjection;
	GLint uniLoc_bFire;
	GLint uniLoc_bTitleScreen;
	GLint uniLoc_titleCounter;
	GLint uniLoc_bIsSplitscreen;
	GLint uniLoc_mModel;
	GLint uniLoc_mWorldInvTrans;
	GLint uniLoc_eyePosition;
	GLint uniLoc_ambientToDiffuseRatio;
	GLint uniLoc_materialSpecular;
	GLint uniLoc_materialDiffuse;
	GLint uniLoc_bIsSkyBoxObject;
	GLint uniLoc_bDiscardTexture;
	GLint uniLoc_Time;
	GLint uniLoc_bIsDebugWireFrameObject;
	GLint uniLoc_bUseLighting;
	GLint uniLoc_bIsLookedAt;
	GLint isReflectRefract_UniLoc;
	GLint reflectBlendRatio_UniLoc;
	GLint refractBlendRatio_UniLoc;
	GLint coefficientRefract_UniLoc;
	GLint bUseTextureAsDiffuse_UniLoc;
	GLint materialDiffuse_UniLoc;
	GLint texFBOColour2DLocID;
	GLint texFBONormal2DLocID;
	GLint texFBOWorldPosition2DLocID;
	GLint screenWidthLocID;
	GLint screenHeightLocID;
	GLint lightDirectionLocID;
	GLint isParticleLocID;
};
