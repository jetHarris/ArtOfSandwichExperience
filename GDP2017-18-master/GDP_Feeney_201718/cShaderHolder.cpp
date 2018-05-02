#include "cShaderHolder.h"

cShaderHolder::cShaderHolder()
{
}

void cShaderHolder::loadUniforms(int shaderID)
{
	renderPassNumber_LocID = glGetUniformLocation(shaderID, "renderPassNumber");
	renderPassVertexNum = glGetUniformLocation(shaderID, "renderPassNumVertex");
	shadowStuff = glGetUniformLocation(shaderID, "shadowStuff");
	shadowLocId = glGetUniformLocation(shaderID, "shadowMap"); //check what this is doing
	lightSpaceMatrixLocation = glGetUniformLocation(shaderID, "lightSpaceMatrix");
	uniLoc_bFire = glGetUniformLocation(shaderID, "bIsFire");
	uniLoc_bTitleScreen = glGetUniformLocation(shaderID, "bIsTitleScreen");
	uniLoc_titleCounter = glGetUniformLocation(shaderID, "titleCounter");
	uniLoc_bIsSplitscreen = glGetUniformLocation(shaderID, "bIsSplitscreen");
	uniLoc_mModel = glGetUniformLocation(shaderID, "mModel");
	uniLoc_mWorldInvTrans = glGetUniformLocation(shaderID, "mWorldInvTranspose");
	uniLoc_eyePosition = glGetUniformLocation(shaderID, "eyePosition");
	uniLoc_ambientToDiffuseRatio = glGetUniformLocation(shaderID, "ambientToDiffuseRatio");
	uniLoc_materialSpecular = glGetUniformLocation(shaderID, "materialSpecular");
	uniLoc_materialDiffuse = glGetUniformLocation(shaderID, "materialDiffuse");
	uniLoc_bIsSkyBoxObject = glGetUniformLocation(shaderID, "isASkyBox");
	uniLoc_bDiscardTexture = glGetUniformLocation(shaderID, "bDiscardTexture");
	uniLoc_Time = glGetUniformLocation(shaderID, "iTime");
	uniLoc_bIsDebugWireFrameObject = glGetUniformLocation(shaderID, "bIsDebugWireFrameObject");
	uniLoc_bUseLighting = glGetUniformLocation(shaderID, "bUseLighting");
	uniLoc_bIsLookedAt = glGetUniformLocation(shaderID, "bIsLookedAt");
	isReflectRefract_UniLoc = glGetUniformLocation(shaderID, "isReflectRefract");
	reflectBlendRatio_UniLoc = glGetUniformLocation(shaderID, "reflectBlendRatio");
	refractBlendRatio_UniLoc = glGetUniformLocation(shaderID, "refractBlendRatio");
	coefficientRefract_UniLoc = glGetUniformLocation(shaderID, "coefficientRefract");
	bUseTextureAsDiffuse_UniLoc = glGetUniformLocation(shaderID, "bUseTextureAsDiffuse");
	materialDiffuse_UniLoc = glGetUniformLocation(shaderID, "materialDiffuse");
	texFBOColour2DLocID = glGetUniformLocation(shaderID, "texFBOColour2D");
	texFBONormal2DLocID = glGetUniformLocation(shaderID, "texFBONormal2D");
	texFBOWorldPosition2DLocID = glGetUniformLocation(shaderID, "texFBOVertexWorldPos2D");
	screenWidthLocID = glGetUniformLocation(shaderID, "screenWidth");
	screenHeightLocID = glGetUniformLocation(shaderID, "screenHeight");
	lightDirectionLocID = glGetUniformLocation(shaderID, "lightDir");
	isParticleLocID = glGetUniformLocation(shaderID, "isParticle");
	uniLoc_mView = glGetUniformLocation(shaderID, "mView");
	uniLoc_mProjection = glGetUniformLocation(shaderID, "mProjection");
}
