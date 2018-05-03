#ifndef _FBO_HG_
#define _FBO_HG_

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class cFBO
{
public:
	cFBO() : 
		ID(0), 
		colourTexture_0_ID(0),
		normalTexture_1_ID(1),
		depthTexture_ID(0), 
		width(-1), height(-1) {};

	GLuint ID;
	GLuint colourTexture_0_ID;
	GLuint normalTexture_1_ID;
	GLuint vertexWorldPos_2_ID;


	GLuint depthTexture_ID;	
	GLint width;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string &error);
	bool shutdown(void);

	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string &error);
	
	void clearBuffers(bool bClearColour = true, bool bClearDepth = true);

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};

#endif