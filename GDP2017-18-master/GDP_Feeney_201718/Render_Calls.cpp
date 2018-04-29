#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <iostream>
#include <sstream>

// cSimpleAssimpSkinnedMesh class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include "cAnimationState.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include "cEmitter.h"
#include "cSoundPlayer.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "cFBO.h"
// Here, the scene is rendered in 3 passes:
// 1. Render geometry to G buffer
// 2. Perform deferred pass, rendering to Deferred buffer
// 3. Then post-pass ("2nd pass" to the scree)
//    Copying from the Pass2_Deferred buffer to the final screen
extern cFBO g_FBO_Pass1_G_Buffer;
//extern cFBO g_FBO_Pass2_Deferred;
extern int selectedGameObjectIndex;

extern cEmitter* g_pFlameEmitter;
extern std::vector< cParticle* > g_FlameParticles;

extern std::vector<std::vector<std::pair<glm::vec3, cGameObject*>>> goalSandwhiches;
extern cGameObject* pParticleObject;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;
extern float totalRunningTime;

extern cCamera* g_pTheSecondCamera;
extern cCamera* g_pTheShadowCamera;
extern bool splitscreen;
extern bool controlsScreen;

extern unsigned int SHADOW_WIDTH;
extern unsigned int SHADOW_HEIGHT;
extern glm::mat4 lightSpaceMatrix;
extern glm::mat4 lightProjection;
extern glm::mat4 lightView;
extern GLint uniLoc_bUseLighting;
extern glm::vec3 lightDirection;
extern GLint isParticleLocID;

//default hold distances of objects
float destinationDistance = 57.0f;
float destinationDistance2 = 57.0f;
GLint curShaderProgID;


//uniforms needed for passing information to the shader
extern GLint uniLoc_mView;
extern GLint uniLoc_mProjection;
extern GLint lightSpaceMatrixLocation;
extern GLint uniLoc_bFire;
extern GLint uniLoc_bTitleScreen;
extern GLint uniLoc_titleCounter;
extern GLint uniLoc_bIsSplitscreen;
extern GLint uniLoc_mModel;
extern GLint uniLoc_mWorldInvTrans;
extern GLint uniLoc_eyePosition;
extern GLint uniLoc_ambientToDiffuseRatio;
extern GLint uniLoc_materialSpecular;
extern GLint uniLoc_materialDiffuse;
extern GLint uniLoc_bIsSkyBoxObject;
extern GLint uniLoc_bDiscardTexture;
extern GLint uniLoc_Time;
extern GLint uniLoc_bIsDebugWireFrameObject;
extern GLint uniLoc_bIsLookedAt;
extern GLint isReflectRefract_UniLoc;
extern GLint reflectBlendRatio_UniLoc;
extern GLint refractBlendRatio_UniLoc;
extern GLint coefficientRefract_UniLoc;
extern GLint bUseTextureAsDiffuse_UniLoc;
extern GLint materialDiffuse_UniLoc;
extern GLint lightDirectionLocID;


//freetype stuff
extern FT_Library mft;
extern FT_Face mface;
extern GLint attribute_coord;
extern GLint uniform_igcolour;
extern GLint uniform_tex;
extern GLint uniform_color;
extern GLuint mdp_vbo;
extern GLuint textProgram;
extern GLuint mvao;

extern enum RenderSceneTypes;
extern cGameObject* pToaster;

extern cSoundPlayer* g_pSoundManager;

extern int playerScore;
extern float gameTimerInSeconds;


cGameObject* rayCastedObject;
cGameObject* rayCastedObject2;
bool draggingFirst = false;
bool draggingSecond = false;

extern bool titleScreen;
extern float titleScreenCounter;

extern bool gameOver;

const static int MAXPARTICLES = 3500;
int liveCounter = 0;

//glm::mat4 locations[MAXPARTICLES];
glm::vec3 locations[MAXPARTICLES];
glm::vec4 colours[MAXPARTICLES];
GLuint particles_position_buffer;
GLuint particles_colour_buffer;


struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

// Draw a single object
// If pParentGO == NULL, then IT'S the parent
void DrawObject(cGameObject* pTheGO, cGameObject* pParentGO, int type);

// Draws one mesh in the game object. 
// The game object is passed to get the orientation.
void DrawMesh(sMeshDrawInfo &theMesh, cGameObject* pTheGO, int type);
// Very quick and dirty way of connecting the samplers
namespace QnDTexureSamplerUtility
{
	void LoadUniformLocationsIfNeeded(unsigned int shaderID);
	void setAllSamplerUnitsToInvalidNumber(GLint invalidTextureUnit);
	void clearAllBlendValuesToZero(void);
	void SetSamplersForMeshTextures(sMeshDrawInfo &meshDrawInfo,
		std::map<std::string /*textureName*/, CTexUnitInfoBrief> &mapTexAndUnitInfo);
	//	void set2DSamplerAndBlenderByIndex(GLint samplerIndex, float blendRatio, GLint textureUnitID );
	//	void setCubeSamplerAndBlenderByIndex( GLint samplerIndex, float blendRatio, GLint textureUnitID );
};

// This a simple cached texture binding system
void setTextureBindings(GLint shaderID, sMeshDrawInfo &theMesh);

void DrawGoalSandwhiches(int type)
{
	int totalSandwhiches = goalSandwhiches.size();
	for (int i = 0; i < totalSandwhiches; ++i)
	{
		int totalIngredients = goalSandwhiches[i].size();
		for (int j = 0; j < totalIngredients; ++j)
		{
			glm::vec3 holderPos = goalSandwhiches[i][j].second->getPosition();
			goalSandwhiches[i][j].second->setPosition(goalSandwhiches[i][j].first);
			DrawObject(goalSandwhiches[i][j].second, NULL, type);
			goalSandwhiches[i][j].second->setPosition(holderPos);
		}
	}
}


void SetUpParticles()
{
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particles_colour_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_colour_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	sVAOInfo VAODrawInfo;
	if (::g_pVAOManager->lookupVAOFromName(pParticleObject->vecMeshes[0].name, VAODrawInfo) == false)
	{	// Didn't find mesh
		return;
	}

	glBindVertexArray(VAODrawInfo.VAO_ID);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(
		3, // attribute. No particular reason for 1, but must match the layout in the shader.
		3, // size : x + y + z + size => 4
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, particles_colour_buffer);
	glVertexAttribPointer(
		4, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : x + y + z + size => 4
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glBindVertexArray(0);
	

}

//void DrawParticle(cParticle* pThePart, int type);
void DrawParticles(int type)
{
	//get all the particle locations
	//put them into the uniform the shader will use for their position
	int numParticles = g_pFlameEmitter->m_vecParticles.size();

	if (type != 2)
	{
		liveCounter = 0;
		for (int index = 0; index < numParticles; index++)
		{
			if (g_pFlameEmitter->m_vecParticles[index]->lifetime > 0)
			{
				locations[liveCounter] = g_pFlameEmitter->m_vecParticles[index]->pos;
				colours[liveCounter++] = g_pFlameEmitter->m_vecParticles[index]->colour;
			}

			//if (g_pFlameEmitter->m_vecParticles[index]->lifetime > 0)
			//DrawParticle(g_pFlameEmitter->m_vecParticles[index], type);
		}

		if (liveCounter == 0)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, liveCounter * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, liveCounter * sizeof(GLfloat) * 3, locations);

		glBindBuffer(GL_ARRAY_BUFFER, particles_colour_buffer);
		glBufferData(GL_ARRAY_BUFFER, liveCounter * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, liveCounter * sizeof(GLfloat) * 4, colours);
	}


	////updating all the model matrixes for the shader
	//glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	//glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * sizeof(glm::mat4), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	//glBufferSubData(GL_ARRAY_BUFFER, 0, MAXPARTICLES * sizeof(glm::mat4), locations); //max particles here might have to be num particles

	

	sVAOInfo VAODrawInfo;
	if (::g_pVAOManager->lookupVAOFromName(pParticleObject->vecMeshes[0].name, VAODrawInfo) == false)
	{	// Didn't find mesh
		//std::cout << "WARNING: Didn't find mesh " << meshToDraw << " in VAO Manager" << std::endl;
		return;
	}

	// 'model' or 'world' matrix
	glm::mat4x4 mModel = glm::mat4x4(1.0f);	//		mat4x4_identity(m);
											//	glm::mat4x4 mModel = pTheGO->getFinalMeshQOrientation();

	glm::mat4 trans = glm::mat4x4(1.0f);
	mModel = mModel * trans;


	// The scale is relative to the original model
	glm::mat4 matScale = glm::mat4x4(1.0f);
	matScale = glm::scale(matScale,
		glm::vec3(pParticleObject->vecMeshes[0].scale,
			pParticleObject->vecMeshes[0].scale,
			pParticleObject->vecMeshes[0].scale));
	mModel = mModel * matScale;

	curShaderProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");


	glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(mModel));

	glm::mat4 mWorldInTranpose = glm::inverse(glm::transpose(mModel));

	glUniformMatrix4fv(uniLoc_mWorldInvTrans, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(mWorldInTranpose));


	// Other uniforms:

	glm::vec3 eye;
	if (type == 2)
	{
		eye = ::g_pTheSecondCamera->getEyePosition();
	}
	else if (type == 3)
	{
		//eye = glm::vec3(0, 0, 0);
		eye = ::g_pTheShadowCamera->getEyePosition();
	}
	else
	{
		eye = ::g_pTheCamera->getEyePosition();
	}
	glUniform3f(uniLoc_eyePosition, eye.x, eye.y, eye.z);

	// Diffuse is often 0.2-0.3 the value of the diffuse

	glUniform1f(uniLoc_ambientToDiffuseRatio, 0.2f);

	// Specular: For now, set this colour to white, and the shininess to something high 
	//	it's an exponent so 64 is pretty shinny (1.0 is "flat", 128 is excessively shiny)

	glUniform4f(uniLoc_materialSpecular, 1.0f, 1.0f, 1.0f, 64.0f);

	glUniform4f(uniLoc_materialDiffuse,
		pParticleObject->vecMeshes[0].debugDiffuseColour.r,
		pParticleObject->vecMeshes[0].debugDiffuseColour.g,
		pParticleObject->vecMeshes[0].debugDiffuseColour.b,
		pParticleObject->vecMeshes[0].debugDiffuseColour.a);


	glUniform1f(uniLoc_Time, totalRunningTime);


	glUniform1f(uniLoc_bIsDebugWireFrameObject, 0.0f);	// FALSE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Default

	glUniform1f(uniLoc_bIsLookedAt, 0.0f);	// FALSE

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);		// Test for z and store in z buffer

	glUniform1f(isReflectRefract_UniLoc, GL_FALSE);

	if (pParticleObject->useLighting)
		glUniform1f(uniLoc_bUseLighting, GL_TRUE);
	else
		glUniform1f(uniLoc_bUseLighting, GL_FALSE);

	glUniform1f(isParticleLocID, GL_TRUE);
	glUniform1f(bUseTextureAsDiffuse_UniLoc, GL_FALSE);


	glBindVertexArray(VAODrawInfo.VAO_ID);

	glDrawElementsInstanced(GL_TRIANGLES,
		VAODrawInfo.numberOfIndices,
		GL_UNSIGNED_INT,					
		0,
		liveCounter);
	// Unbind that VAO
	glBindVertexArray(0);
	glUniform1f(isParticleLocID, GL_FALSE);

}
//void DrawParticle(cParticle* pThePart, int type)
//{
//	glm::vec3 oldPosition = pParticleObject->getPosition();
//	pParticleObject->setPosition(pThePart->pos);
//
//	DrawObject(pParticleObject, NULL, type);
//	//pParticleObject->setPosition(oldPosition);
//	return;
//}

// See: http://www.glfw.org/docs/latest/window_guide.html#window_size
void window_size_callback(GLFWwindow* window, int width, int height)
{
	if ((::g_FBO_Pass1_G_Buffer.width != width) || (::g_FBO_Pass1_G_Buffer.height != height))
	{
		// Window size has changed, so resize the offscreen frame buffer object
		std::string error;
		if (!::g_FBO_Pass1_G_Buffer.reset(width, height, error))
		{
			std::cout << "In window_size_callback(), the g_FBO_Pass1_G_Buffer.reset() call returned an error:" << std::endl;
			std::cout << "\t" << error << std::endl;
		}
		else
		{
			std::cout << "Offscreen g_FBO_Pass1_G_Buffer now: " << width << " x " << height << std::endl;
		}
	}//if ( ( ::g_FBO_Pass1_G_Buffer.width....

	//if ((::g_FBO_Pass2_Deferred.width != width) || (::g_FBO_Pass2_Deferred.height != height))
	//{
	//	// Window size has changed, so resize the offscreen frame buffer object
	//	std::string error;
	//	if (!::g_FBO_Pass2_Deferred.reset(width, height, error))
	//	{
	//		std::cout << "In window_size_callback(), the g_FBO_Pass2_Deferred.reset() call returned an error:" << std::endl;
	//		std::cout << "\t" << error << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << "Offscreen g_FBO_Pass2_Deferred now: " << width << " x " << height << std::endl;
	//	}
	//}//if ( ( ::g_FBO_Pass1_G_Buffer.width....

	return;
}

void RenderText(const char *text, float x, float y, float sx, float sy) {
	glDisable(GL_CULL_FACE);
	glUniform1i(uniform_igcolour, false);

	const char *p;
	FT_GlyphSlot g = mface->glyph;

	GLuint tex;
	//
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, mdp_vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);


	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(mface, *p, FT_LOAD_RENDER))
			continue;

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);


		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
			{ x2, -y2, 0, 0 },
			{ x2 + w, -y2, 1, 0 },
			{ x2, -y2 - h, 0, 1 },
			{ x2 + w, -y2 - h, 1, 1 },
		};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(attribute_coord);
	glDeleteTextures(1, &tex);


}


void RenderScene(std::vector< cGameObject* > &vec_pGOs, GLFWwindow* pGLFWWindow, double deltaTime, int type)
{

	float ratio;
	int width, height;
	glm::mat4x4 matProjection;			// was "p"

	glfwGetFramebufferSize(pGLFWWindow, &width, &height);


	if (type == 0 || type == 4)
	{
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
	}
	else if (type == 1)
	{
		ratio = width / ((float)height / 2);
		glViewport(0, height / 2, width, height / 2);
	}
	else if (type == 2)
	{
		ratio = width / ((float)height / 2);
		glViewport(0, 0, width, height / 2);
	}
	else if (type == 3)
	{
		//ratio = width / (float)height;
		//glViewport(0, 0, width, height);

		ratio = SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	}

	//		// Clear colour AND depth buffer
	//		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			//glEnable(GL_DEPTH_TEST);

			//        glUseProgram(program);
	::g_pShaderManager->useShaderProgram("mySexyShader");
	GLint curShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	// Update all the light uniforms...
	// (for the whole scene)
	::g_pLightManager->CopyLightInformationToCurrentShader();

	if (type != 3) //not the shadow pass
	{
		// Projection and view don't change per scene (maybe)
		matProjection = glm::perspective(0.6f,			// FOV
			ratio,		// Aspect ratio
			1.0f,		// Near (as big as possible)
			10000.0f);	// Far (as small as possible)
	}
	else
	{
		matProjection = lightProjection;
	}


	// View or "camera" matrix
	glm::mat4 matView = glm::mat4(1.0f);	// was "v"

	// Now the view matrix is taken right from the camera class
	if (type == 2) //second player
	{
		matView = ::g_pTheSecondCamera->getViewMatrix();
	}
	else if (type == 3) //shadow pass
	{
		matView = lightView;
	}
	else
	{
		matView = ::g_pTheCamera->getViewMatrix();
	}




	glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(matView));
	glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(matProjection));
	glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(lightSpaceMatrix));

	glUniform3f(lightDirectionLocID, lightDirection.x, lightDirection.y, lightDirection.z);

	// Enable blend ("alpha") transparency for the scene
	// NOTE: You "should" turn this OFF, then draw all NON-Transparent objects
	//       Then turn ON, sort objects from far to near ACCORDING TO THE CAMERA
	//       and draw them
	glEnable(GL_BLEND);		// Enables "blending"
							//glDisable( GL_BLEND );
							// Source == already on framebuffer
							// Dest == what you're about to draw
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//raycast into the scene with the camera
	//cameraPos + cameraFront should be the answer for the target
	if (type == 2) //second screen
	{
		gPhysicsWorld->RayCast(g_pTheSecondCamera->cameraPos, g_pTheSecondCamera->cameraPos + (g_pTheSecondCamera->cameraFront * 130.0f));
	}
	else if (type == 4 || type == 1) //first screen either splitscreen or single screen
	{
		gPhysicsWorld->RayCast(g_pTheCamera->cameraPos, g_pTheCamera->cameraPos + (g_pTheCamera->cameraFront * 130.0f));
	}
	unsigned int sizeOfVector = (unsigned int)vec_pGOs.size();	//*****//



	bool somethingRayCastedThisPass = false;

	for (int index = 0; index != sizeOfVector; index++)
	{
		//cGameObject* pTheGO = ::g_vecGameObjects[index];
		cGameObject* pTheGO = vec_pGOs[index];

		//update the position and maybe the orientation
		if (pTheGO->physics)
		{
			glm::vec3 tempPos;
			pTheGO->rBody->GetPosition(tempPos);
			glm::quat orient;
			pTheGO->rBody->GetOrientation(orient);
			//only go through the following if it is not the shadow pass
			if (type != 3 && type != 0)
			{
				//only update physics objects on the first camera pass of splitscreen
				if (type == 4 || type == 1)
				{
					pTheGO->overwriteQOrientation(orient * pTheGO->rBodyOrientationOffset);
					pTheGO->setPosition(tempPos + pTheGO->rBodyOffset);
				}

				bool dragging;
				if (type == 2)
					dragging = draggingSecond;
				else
					dragging = draggingFirst;

				if (!dragging)
				{
					//see if the object is being rayCasted
					if (pTheGO->lookable)
					{
						bool rayCasted = true;
						pTheGO->rBody->GetRayCasted(rayCasted);
						if (rayCasted)
						{
							pTheGO->isLookedAt = true;
							//pTheGO->vecMeshes[0].bDrawAsWireFrame = true;
							//set the object being raycasted
							if (type == 2)
								rayCastedObject2 = pTheGO;
							else
								rayCastedObject = pTheGO;

							somethingRayCastedThisPass = true;
						}
						else
						{
							pTheGO->isLookedAt = false;
							//pTheGO->vecMeshes[0].bDrawAsWireFrame = false;
						}

					}
				}
				else if (type != 2 && pTheGO == rayCastedObject)
				{

					//give the raycastedobject velocity towards the center of the screen
					//calculate the point we want to send it to
					glm::vec3 destination = g_pTheCamera->cameraPos + (g_pTheCamera->cameraFront * destinationDistance);
					//where the object currently is
					//tempPos
					//std::cout << "X: " << rayCastedObject->getPosition().x << " Y: " << rayCastedObject->getPosition().y << " Z:" << rayCastedObject->getPosition().z << std::endl;
					glm::vec3 pushForce = (glm::normalize(destination - tempPos))* (10.0f * glm::distance(destination, tempPos));
					if (pushForce.y > 0)
						pushForce.y *= 2;
					//pTheGO->rBody->Push(pushForce);
					pTheGO->rBody->SetVelocity(pushForce);
					glm::vec3 angularVel;
					pTheGO->rBody->GetRotationalVel(angularVel);
					float angularLength = glm::length(angularVel);
					if (angularLength < 0.1 && angularLength != 0)
					{
						pTheGO->rBody->ZeroOutRotationalVel();
					}
					else
					{
						pTheGO->rBody->SetRotationalVel(angularVel* 0.9f);
					}

					//if the raycasted object is bread and it is above the toaster and the emitter is on, toast the bread
					if (rayCastedObject->type == ObjectType::BREADSLICE)
					{

						glm::vec3 toasterPos = pToaster->getPosition();
						glm::vec3 objectPos = rayCastedObject->getPosition();
						//test if the toast is close enough to be toasted
						if (g_pFlameEmitter->emitting && abs(toasterPos.x - objectPos.x) < 4 && abs(toasterPos.z - objectPos.z) < 4 && abs(toasterPos.y - objectPos.y) < 7)
						{
							g_pSoundManager->PlaySound(5);

							rayCastedObject->vecMeshes[0].vecMehs2DTextures.clear();
							rayCastedObject->vecMeshes[0].vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadLoafToastedT.bmp", 1.0f));
							rayCastedObject->vecMeshes[0].vecMehs2DTextures.push_back(sTextureBindBlendInfo("breadToastedInside.bmp", 0.0f));
							rayCastedObject->type = ObjectType::TOASTEDBREADSLICE;
						}
					}
				}
				else if (type == 2 && pTheGO == rayCastedObject2)
				{

					//give the raycastedobject velocity towards the center of the screen
					//calculate the point we want to send it to
					glm::vec3 destination = g_pTheSecondCamera->cameraPos + (g_pTheSecondCamera->cameraFront * destinationDistance2);
					//where the object currently is

					glm::vec3 pushForce = (glm::normalize(destination - tempPos))* (10.0f * glm::distance(destination, tempPos));
					if (pushForce.y > 0)
						pushForce.y *= 2;
					//pTheGO->rBody->Push(pushForce);
					pTheGO->rBody->SetVelocity(pushForce);
					glm::vec3 angularVel;
					pTheGO->rBody->GetRotationalVel(angularVel);
					float angularLength = glm::length(angularVel);
					if (angularLength < 0.1 && angularLength != 0)
					{
						pTheGO->rBody->ZeroOutRotationalVel();
					}
					else
					{
						pTheGO->rBody->SetRotationalVel(angularVel* 0.9f);
					}

					//if the raycasted object is bread and it is above the toaster and the emitter is on, toast the bread
					if (rayCastedObject2->type == ObjectType::BREADSLICE)
					{
						glm::vec3 toasterPos = pToaster->getPosition();
						glm::vec3 objectPos = rayCastedObject2->getPosition();
						//test if the toast is close enough to be toasted
						if (g_pFlameEmitter->emitting && abs(toasterPos.x - objectPos.x) < 4 && abs(toasterPos.z - objectPos.z) < 4 && abs(toasterPos.y - objectPos.y) < 7)
						{
							g_pSoundManager->PlaySound(5);

							rayCastedObject2->vecMeshes[0].vecMehs2DTextures.clear();
							rayCastedObject2->vecMeshes[0].vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadLoafToastedT.bmp", 1.0f));
							rayCastedObject2->vecMeshes[0].vecMehs2DTextures.push_back(sTextureBindBlendInfo("breadToastedInside.bmp", 0.0f));
							rayCastedObject2->type = ObjectType::TOASTEDBREADSLICE;
						}
					}
				}
			}

		}

		// This is the top level vector, so they are all "parents" 
		DrawObject(pTheGO, NULL, type);

	}//for ( int index = 0...


	bool dragging;
	if (type == 2)
		dragging = draggingSecond;
	else
		dragging = draggingFirst;

	if (!dragging && !somethingRayCastedThisPass)
	{
		if (type == 2)
			rayCastedObject2 = NULL;
		else if (type == 4 || type == 1)
			rayCastedObject = NULL;
	}

	bool first = true;



	glUniform1f(uniLoc_bIsSplitscreen, splitscreen);
	//glUniform1f(uniLoc_bFire, 1.0f);

	if (type != 3 && type != 0) //not deferred render pass, and not shadow pass
	{
		DrawParticles(type);
		glUniform1f(uniLoc_bFire, 0.0f);
		DrawGoalSandwhiches(type);
	}

	glUniform1f(uniLoc_bFire, 0.0f);

	//print stuff to screen
	if (type == 0)
	{
		glUseProgram(textProgram);
		GLfloat usedColour[4] = { 0, 0, 1, 1 };
		GLfloat green[4] = { 0, 1, 1, 1 };

		glUniform4fv(uniform_color, 1, usedColour);

		glBindVertexArray(mvao);



		if (titleScreen)
		{
			//so the counter will not be displayed if someone is looking at the controls during the game
			if (controlsScreen)
				return;
			//at this time the instruction to click to start should be given
			if (titleScreenCounter > 4 && titleScreenCounter < 4.1)
			{
				RenderText("Click or Press A to start", -0.54, 0.1, 0.002, 0.002);
			}
			else if (titleScreenCounter < 4) //otherwise should render the countdown to start
			{
				int seconds = titleScreenCounter;
				float ratio = ((float)seconds / titleScreenCounter) * 0.009;
				std::string num = std::to_string(seconds);

				if (seconds == 1)
					RenderText(num.c_str(), -0.05 - ratio * 2, 0, ratio, ratio);
				else
					RenderText(num.c_str(), -0.05 - ratio * 6, 0, ratio, ratio);
			}
		}
		else if (!gameOver) //if the game is running then display the score and the time left
		{
			int minutes = gameTimerInSeconds / 60;
			int seconds = gameTimerInSeconds - (minutes * 60);
			std::stringstream displayStringStream;
			if (seconds < 10)
				displayStringStream << "TIMER: " << minutes << ":" << "0" << seconds << "    SCORE:" << playerScore;
			else
				displayStringStream << "TIMER: " << minutes << ":" << seconds << "    SCORE:" << playerScore;

			RenderText(displayStringStream.str().c_str(), -0.95, 0.9, 0.002, 0.002);
		}
		else
		{
			//credits and displaying score
			std::stringstream displayStringStream;
			if (playerScore == 69)
			{
				displayStringStream << "SCORE: " << playerScore << " NICE!!";
			}
			else
			{
				displayStringStream << "SCORE: " << playerScore;
			}
			
			RenderText(displayStringStream.str().c_str(), -0.5, +0.3, 0.006, 0.006);


			glUniform4fv(uniform_color, 1, green);

			RenderText("Programmed By:", -0.7, -0.1, 0.003, 0.003);

			//glUniform4fv(uniform_color, 1, usedColour);
			RenderText("Luke Harris", -0.5, -0.3, 0.003, 0.003);

			glUniform4fv(uniform_color, 1, green);

			RenderText("Main Original Score By:", -0.7, -0.5, 0.003, 0.003);
			//glUniform4fv(uniform_color, 1, usedColour);
			RenderText("Hunter Bennet", -0.5, -0.7, 0.003, 0.003);
		}
	}

	return;
}



// Draw a single object
// If pParentGO == NULL, then IT'S the parent
void DrawObject(cGameObject* pTheGO, cGameObject* pParentGO, int type)
{
	if (!pTheGO)
	{	// Shouldn't happen, but if GO pointer is invlaid, return
		return;
	}

	//for the threaded solution of slicing
	//if (pTheGO->slicingFinished)
	//{
	//	pTheGO->loadMeshIntoVAONow();
	//}

	// Go through all meshes, drawing them
	unsigned int numMeshes = (unsigned int)pTheGO->vecMeshes.size();
	for (unsigned int meshIndex = 0; meshIndex != numMeshes; meshIndex++)
	{
		DrawMesh(pTheGO->vecMeshes[meshIndex], pTheGO, type);
	}


	return;
}





// Draws one mesh in the game object. 
// The game object is passed to get the orientation.
void DrawMesh(sMeshDrawInfo &theMesh, cGameObject* pTheGO, int type)
{
	if (type == 3)
	{
		if (!pTheGO->castShadow)
			return;
	}
	if (!theMesh.bIsVisible)
	{
		return;
	}


	sVAOInfo VAODrawInfo;
	if (::g_pVAOManager->lookupVAOFromName(theMesh.name, VAODrawInfo) == false)
	{	// Didn't find mesh
		//std::cout << "WARNING: Didn't find mesh " << meshToDraw << " in VAO Manager" << std::endl;
		return;
	}

	// 'model' or 'world' matrix
	glm::mat4x4 mModel = glm::mat4x4(1.0f);	//		mat4x4_identity(m);
//	glm::mat4x4 mModel = pTheGO->getFinalMeshQOrientation();

	glm::mat4 trans = glm::mat4x4(1.0f);
	trans = glm::translate(trans,
		theMesh.offset + pTheGO->getPosition());
	mModel = mModel * trans;

	// Now with quaternion rotation
	// This is combined with the original game object
	// (i.e. if the game object is rotated, then the mesh is rotated relative to the game object)
	glm::mat4 postRotQuat = glm::mat4(pTheGO->getFinalMeshQOrientation(theMesh.getQOrientation()));
	mModel = mModel * postRotQuat;

	//	mModel = mModel * trans * postRotQuat;


		// The scale is relative to the original model
	glm::mat4 matScale = glm::mat4x4(1.0f);
	matScale = glm::scale(matScale,
		glm::vec3(theMesh.scale,
			theMesh.scale,
			theMesh.scale));
	mModel = mModel * matScale;

	curShaderProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");


	glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(mModel));

	glm::mat4 mWorldInTranpose = glm::inverse(glm::transpose(mModel));

	glUniformMatrix4fv(uniLoc_mWorldInvTrans, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(mWorldInTranpose));




	// Other uniforms:

	glm::vec3 eye;
	if (type == 2)
	{
		eye = ::g_pTheSecondCamera->getEyePosition();
	}
	else if (type == 3)
	{
		//eye = glm::vec3(0, 0, 0);
		eye = ::g_pTheShadowCamera->getEyePosition();
	}
	else
	{
		eye = ::g_pTheCamera->getEyePosition();
	}
	glUniform3f(uniLoc_eyePosition, eye.x, eye.y, eye.z);





	// Diffuse is often 0.2-0.3 the value of the diffuse

	glUniform1f(uniLoc_ambientToDiffuseRatio, 0.2f);

	// Specular: For now, set this colour to white, and the shininess to something high 
	//	it's an exponent so 64 is pretty shinny (1.0 is "flat", 128 is excessively shiny)

	glUniform4f(uniLoc_materialSpecular, 1.0f, 1.0f, 1.0f, 64.0f);

	glUniform4f(uniLoc_materialDiffuse,
		theMesh.debugDiffuseColour.r,
		theMesh.debugDiffuseColour.g,
		theMesh.debugDiffuseColour.b,
		theMesh.debugDiffuseColour.a);


	// Set ALL the samplers to something (so they don't point to GL_TEXTURE0)
	QnDTexureSamplerUtility::LoadUniformLocationsIfNeeded(curShaderProgID);
	// There's no way we have 999 texture units...
	QnDTexureSamplerUtility::setAllSamplerUnitsToInvalidNumber(999);
	QnDTexureSamplerUtility::clearAllBlendValuesToZero();

	// Now set our samplers, and blend function to something
	// Basic texture binding setup (assign all loaded textures to samplers)
	g_pTextureManager->UpdateTextureBindingsByTextureNameSimple();
	std::map<std::string, CTexUnitInfoBrief> mapTexNameToTexUnit;
	g_pTextureManager->GetBoundTextureUnitsByTextureNames(mapTexNameToTexUnit);

	// Now look up what textures our object is using and set the samplers
	QnDTexureSamplerUtility::SetSamplersForMeshTextures(theMesh, mapTexNameToTexUnit);

	if (type == 0) //deferred render
	{
		if (titleScreen)
		{
			glUniform1f(uniLoc_bTitleScreen, GL_TRUE);
			glUniform1f(uniLoc_titleCounter, titleScreenCounter);
		}
		else
			glUniform1f(uniLoc_bTitleScreen, GL_FALSE);
	}

	if (theMesh.bIsSkyBoxObject)
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_TRUE);
	}
	else
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_FALSE);
	}

	if (pTheGO->bDiscardTexture)
	{
		glUniform1f(uniLoc_bDiscardTexture, 1.0f);	// TRUE
	}
	else
	{
		glUniform1f(uniLoc_bDiscardTexture, 0.0f);	// FALSE
	}
	glUniform1f(uniLoc_Time, totalRunningTime);

	// Wireframe?
	if (theMesh.bDrawAsWireFrame)
	{
		glUniform1f(uniLoc_bIsDebugWireFrameObject, 1.0f);	// TRUE
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Default
	}
	else
	{
		glUniform1f(uniLoc_bIsDebugWireFrameObject, 0.0f);	// FALSE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Default
	}



	// Wireframe?
	if (pTheGO->isLookedAt)
	{
		glUniform1f(uniLoc_bIsLookedAt, 1.0f);	// TRUE
	}
	else
	{
		glUniform1f(uniLoc_bIsLookedAt, 0.0f);	// FALSE
	}

	// GL_CULL_FACE
	if (theMesh.bDisableBackFaceCulling)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}// GL_CULL_FACE

	// GL_DEPTH_TEST
	if (theMesh.bDisableDepthTest)
	{
		glDisable(GL_DEPTH_TEST);		// Test for z and store in z buffer
	}
	else
	{
		glEnable(GL_DEPTH_TEST);		// Test for z and store in z buffer
	}// GL_DEPTH_TEST


	// STARTOF: Reflection and refraction shader uniforms


	// Is this the reflective sphere
	if (theMesh.bIsEnvirMapped)
	{
		glUniform1f(isReflectRefract_UniLoc, GL_TRUE);
	}
	else
	{
		glUniform1f(isReflectRefract_UniLoc, GL_FALSE);
	}
	// 
	glUniform1f(reflectBlendRatio_UniLoc, theMesh.reflectBlendRatio);
	glUniform1f(refractBlendRatio_UniLoc, theMesh.refractBlendRatio);
	glUniform1f(coefficientRefract_UniLoc, theMesh.coefficientRefract);
	// And more environment things

	if (pTheGO->useLighting)
		glUniform1f(uniLoc_bUseLighting, GL_TRUE);
	else
		glUniform1f(uniLoc_bUseLighting, GL_FALSE);


	glUniform1f(bUseTextureAsDiffuse_UniLoc, GL_TRUE);


	glBindVertexArray(VAODrawInfo.VAO_ID);

	glDrawElements(GL_TRIANGLES,
		VAODrawInfo.numberOfIndices,		// testMesh.numberOfTriangles * 3,	// How many vertex indices
		GL_UNSIGNED_INT,					// 32 bit int 
		0);
	// Unbind that VAO
	glBindVertexArray(0);

	return;
}



// Very quick and dirty way of connecting the samplers
namespace QnDTexureSamplerUtility
{
	bool bIsLoaded = false;

	// 2D textures:
	GLint texSamp2D00_LocID = -1;
	GLint texSamp2D01_LocID = -1;
	GLint texSamp2D02_LocID = -1;
	GLint texSamp2D03_LocID = -1;
	GLint texSamp2D04_LocID = -1;


	GLint texBlend00_LocID = -1;
	GLint texBlend01_LocID = -1;
	GLint texBlend02_LocID = -1;
	GLint texBlend03_LocID = -1;
	GLint texBlend04_LocID = -1;


	// Cube map textures
	GLint texSampCube00_LocID = -1;
	GLint texSampCube01_LocID = -1;
	GLint texSampCube02_LocID = -1;
	GLint texSampCube03_LocID = -1;

	GLint texCubeBlend00_LocID = -1;
	GLint texCubeBlend01_LocID = -1;
	GLint texCubeBlend02_LocID = -1;
	GLint texCubeBlend03_LocID = -1;

	void LoadUniformLocationsIfNeeded(unsigned int shaderID)
	{
		if (!bIsLoaded)
		{	// Load the uniform locations
			texSamp2D00_LocID = glGetUniformLocation(shaderID, "texSamp2D00");
			texSamp2D01_LocID = glGetUniformLocation(shaderID, "texSamp2D01");
			texSamp2D02_LocID = glGetUniformLocation(shaderID, "texSamp2D02");
			texSamp2D03_LocID = glGetUniformLocation(shaderID, "texSamp2D03");
			texSamp2D04_LocID = glGetUniformLocation(shaderID, "texSamp2D04");


			texBlend00_LocID = glGetUniformLocation(shaderID, "texBlend00");
			texBlend01_LocID = glGetUniformLocation(shaderID, "texBlend01");
			texBlend02_LocID = glGetUniformLocation(shaderID, "texBlend02");
			texBlend03_LocID = glGetUniformLocation(shaderID, "texBlend03");
			texBlend04_LocID = glGetUniformLocation(shaderID, "texBlend04");


			texSampCube00_LocID = glGetUniformLocation(shaderID, "texSampCube00");
			texSampCube01_LocID = glGetUniformLocation(shaderID, "texSampCube00");
			texSampCube02_LocID = glGetUniformLocation(shaderID, "texSampCube00");
			texSampCube03_LocID = glGetUniformLocation(shaderID, "texSampCube00");

			texCubeBlend00_LocID = glGetUniformLocation(shaderID, "texCubeBlend00");
			texCubeBlend01_LocID = glGetUniformLocation(shaderID, "texCubeBlend01");
			texCubeBlend02_LocID = glGetUniformLocation(shaderID, "texCubeBlend02");
			texCubeBlend03_LocID = glGetUniformLocation(shaderID, "texCubeBlend03");

			bIsLoaded = true;	// So we won't call these again
		}

		return;
	}// void LoadUniformLocationsIfNeeded()


	void setAllSamplerUnitsToInvalidNumber(GLint invalidTextureUnit)
	{
		glUniform1i(texSamp2D00_LocID, invalidTextureUnit);
		glUniform1i(texSamp2D01_LocID, invalidTextureUnit);
		glUniform1i(texSamp2D02_LocID, invalidTextureUnit);
		glUniform1i(texSamp2D03_LocID, invalidTextureUnit);
		glUniform1i(texSamp2D04_LocID, invalidTextureUnit);


		// Cube map textures
		glUniform1i(texSampCube00_LocID, invalidTextureUnit);
		glUniform1i(texSampCube01_LocID, invalidTextureUnit);
		glUniform1i(texSampCube02_LocID, invalidTextureUnit);
		glUniform1i(texSampCube03_LocID, invalidTextureUnit);

		return;
	}//void setAllSamplerUnitsToInvalidNumber()

	void clearAllBlendValuesToZero(void)
	{
		glUniform1f(texBlend00_LocID, 0.0f);
		glUniform1f(texBlend01_LocID, 0.0f);
		glUniform1f(texBlend02_LocID, 0.0f);
		glUniform1f(texBlend03_LocID, 0.0f);
		glUniform1f(texBlend04_LocID, 0.0f);


		glUniform1f(texCubeBlend00_LocID, 0.0f);
		glUniform1f(texCubeBlend01_LocID, 0.0f);
		glUniform1f(texCubeBlend02_LocID, 0.0f);
		glUniform1f(texCubeBlend03_LocID, 0.0f);

		return;
	}//void clearAllBlendValuesToZero()

	// Handy function to set sampelrs
	void set2DSamplerAndBlenderByIndex(GLint samplerIndex, float blendRatio, GLint textureUnitID)
	{
		switch (samplerIndex)
		{
		case 0:
			glUniform1i(texSamp2D00_LocID, textureUnitID);
			glUniform1f(texBlend00_LocID, blendRatio);
			break;
		case 1:
			glUniform1i(texSamp2D01_LocID, textureUnitID);
			glUniform1f(texBlend01_LocID, blendRatio);
			break;
		case 2:
			glUniform1i(texSamp2D02_LocID, textureUnitID);
			glUniform1f(texBlend02_LocID, blendRatio);
			break;
		case 3:
			glUniform1i(texSamp2D03_LocID, textureUnitID);
			glUniform1f(texBlend03_LocID, blendRatio);
			break;
		case 4:
			glUniform1i(texSamp2D04_LocID, textureUnitID);
			glUniform1f(texBlend04_LocID, blendRatio);
			break;
		default:
			// Invalid samplerIndex
			break;
		}//switch (samplerIndex)
		return;
	}//void set2DSamplerAndBlenderByIndex()

	void setCubeSamplerAndBlenderByIndex(GLint samplerIndex, float blendRatio, GLint textureUnitID)
	{
		switch (samplerIndex)
		{
		case 0:
			glUniform1i(texSampCube00_LocID, textureUnitID);
			glUniform1f(texCubeBlend00_LocID, blendRatio);
			break;
		case 1:
			glUniform1i(texSampCube01_LocID, textureUnitID);
			glUniform1f(texCubeBlend01_LocID, blendRatio);
			break;
		case 2:
			glUniform1i(texSampCube02_LocID, textureUnitID);
			glUniform1f(texCubeBlend02_LocID, blendRatio);
			break;
		case 3:
			glUniform1i(texSampCube03_LocID, textureUnitID);
			glUniform1f(texCubeBlend03_LocID, blendRatio);
			break;
		default:
			// Invalid samplerIndex;
			break;
		}//switch (samplerIndex)
		return;
	}//void setCubeSamplerAndBlenderByIndex()


	void SetSamplersForMeshTextures(sMeshDrawInfo &meshDrawInfo,
		std::map<std::string /*textureName*/, CTexUnitInfoBrief> &mapTexAndUnitInfo)
	{
		// 2D textures first
		int numTextures = (int)meshDrawInfo.vecMehs2DTextures.size();
		for (int samplerIndex = 0; samplerIndex != numTextures; samplerIndex++)
		{
			// What texture unit is this texture set to?
			std::map<std::string, CTexUnitInfoBrief>::iterator itTextUnitInfo
				= mapTexAndUnitInfo.find(meshDrawInfo.vecMehs2DTextures[samplerIndex].name);
			// Have we mapped that one?
			if (itTextUnitInfo != mapTexAndUnitInfo.end())
			{	// Yes, so assign it
				set2DSamplerAndBlenderByIndex(samplerIndex,
					meshDrawInfo.vecMehs2DTextures[samplerIndex].blendRatio,
					itTextUnitInfo->second.texUnitNumOffsetBy_GL_TEXTURE0);
				// Set blend function, too
			}
		}// 2D textures

		// Now cube maps
		numTextures = (int)meshDrawInfo.vecMeshCubeMaps.size();
		for (int samplerIndex = 0; samplerIndex != numTextures; samplerIndex++)
		{
			// What texture unit is this texture set to?
			std::map<std::string, CTexUnitInfoBrief>::iterator itTextUnitInfo
				= mapTexAndUnitInfo.find(meshDrawInfo.vecMeshCubeMaps[samplerIndex].name);
			// Have we mapped that one?
			if (itTextUnitInfo != mapTexAndUnitInfo.end())
			{	// Yes, so assign it
				setCubeSamplerAndBlenderByIndex(samplerIndex,
					meshDrawInfo.vecMeshCubeMaps[samplerIndex].blendRatio,
					itTextUnitInfo->second.texUnitNumOffsetBy_GL_TEXTURE0);
			}
		}// cube maps



	}//void SetSamplersForMeshTextures()

};//namespace QnDTexureSamplerUtility

