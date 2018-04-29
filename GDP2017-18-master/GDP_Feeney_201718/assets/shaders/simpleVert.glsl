// Vertex shader
#version 420

uniform mat4 mView;
uniform mat4 mProjection;
uniform mat4 mModel;
uniform mat4 mWorldInvTranspose;

uniform bool renderPassNumVertex;

uniform bool shadowStuff;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform bool isASkyBox;		// Same as the one in the fragments

uniform bool isParticle;	

uniform sampler2D texHeightMap;			// Is Texture Unit 0 no matter WHAT! WHY???
uniform bool bIsHeightMap;


// "Vertex" attribute (now use 'in')
in vec4 vCol;		// attribute
in vec3 vPos;		// was: vec2 vPos
in vec3 vNorm;		// Vertex normal
in vec4 uvX2;		// Added: UV 1 and 2
// Added for bump mapping:
in vec3 vTangent;		// For bump (or normal) mapping
in vec3 vBitangent;		// For bump (or normal) mapping
in float sliced;


out vec4 fColor;				// was: vec4
out vec3 fVertNormal;		// Also in "world" (no view or projection)
out vec3 fVecWorldPosition;	// 
out vec4 fUV_X2;			// Added: UV 1 and 2 to fragment
out vec3 fTangent;		// For bump (or normal) mapping
out vec3 fBitangent;	// For bump (or normal) mapping
out vec4 FragPosLightSpacePass;
out float slicedFrag;


layout(std140) uniform NUB_perFrame
{
	vec3 eyePosition;	// Camera position
} perFramNUB;

layout (location = 3) in vec3 aOffset;
layout (location = 4) in vec4 pColour;


out vec4 pColourFrag;

void main()
{
	if (shadowStuff == true)
	{
		gl_Position = lightSpaceMatrix * mModel * vec4(vPos, 1.0);
		return;
	}
	if(!renderPassNumVertex)
	{
		vec4 vertPosition = vec4(vPos, 1.0f);		
		mat4 matModel = mModel;

		if (isParticle)
		{
			pColourFrag = pColour;
			vertPosition.y += aOffset.y * 2;
			vertPosition.x += aOffset.x * 10;
			vertPosition.z += aOffset.z * 10;
		}

		vec3 FragPos = vec3(matModel * vertPosition);
		FragPosLightSpacePass = lightSpaceMatrix * vec4(FragPos, 1.0);

		gl_Position = mProjection * mView * matModel * vertPosition;

		
		// Calculate vertex and normal based on ONLY world 
		fVecWorldPosition = vec3( matModel * vertPosition ).xyz;

		// This normal is in "world space" but only has rotation
		fVertNormal = vec3( mWorldInvTranspose * vec4(vNorm, 1.0f) ).xyz;	
			
		// Pass the tangent and bi-tangent out to the fragment shader
		fTangent = vTangent;
		fBitangent = vBitangent;
	
		slicedFrag = sliced;
		fColor = vCol;
		fUV_X2 = uvX2;			// Sent to fragment shader
	}
	
}

