// Fragment shader
#version 450


in vec4 fColor;					
in vec3 fVertNormal;			// Also in "world" (no view or projection)
in vec3 fVecWorldPosition;		// 
in vec4 fUV_X2;					// Added: UV 1 and 2 to fragment

in float slicedFrag;
in vec4 pColourFrag;

in vec4 FragPosLightSpacePass;

out vec4 FBOout_colour;			// GL_COLOR_ATTACHMENT0
out vec4 FBOout_normal;			// GL_COLOR_ATTACHMENT1
out vec4 FBOout_vertexWorldPos;	// GL_COLOR_ATTACHMENT2

// The values our OBJECT material
uniform vec4 materialDiffuse;	
uniform bool bUseTextureAsDiffuse;
uniform bool bUseLighting;
//uniform vec4 materialAmbient;   		// 0.2
uniform float ambientToDiffuseRatio; 	// Maybe	// 0.2 or 0.3
uniform vec4 materialSpecular;  // rgb = colour of HIGHLIGHT only
								// w = shininess of the 
const float offset = 1.0 / 300.0;  

vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

float sharpKernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
);

float blurKernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float edgeKernel[9] = float[](
        1, 1, 1,
        1, -4, 1,
        1, 1, 1
);

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 lightDir;

vec3 eyePosition;

layout(std140) uniform NUB_perFrame
{
	vec3 eyePosition;	// Camera position
} perFramNUB;

/*****************************************************/
struct sLightDesc {
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;		// Colour (xyz), intensity (w)
	vec4 attenuation;	// x = constant, y = linear, z = quadratic
	vec4 direction;
	vec4 typeParams;	// x = type
						// 		0 = point
						// 		1 = directional
						// 		2 = spot
						// y = distance cut-off
	                    // z angle1, w = angle2		- only for spot
};
const int NUMBEROFLIGHTS = 8;

uniform sLightDesc myLight[NUMBEROFLIGHTS];

uniform int theLightId;

uniform bool bIsDebugWireFrameObject;
uniform bool bIsLookedAt;
uniform bool bIsFire;
uniform bool bIsSplitscreen;
uniform bool bIsTitleScreen;
uniform float titleCounter;

// For "Chromatic Aberration"
uniform float CAoffset;// = 0.02f; 


// Note: this CAN'T be an array (sorry). See 3D texture array
uniform sampler2D texSamp2D00;		// Represents a 2D image
uniform sampler2D texSamp2D01;		// Represents a 2D image
uniform sampler2D texSamp2D02;		// Represents a 2D image		
uniform sampler2D texSamp2D03;		// Represents a 2D image
uniform sampler2D texSamp2D04;		// Represents a 2D image
// ... and so on...


uniform float texBlend00;		
uniform float texBlend01;		
uniform float texBlend02;	
uniform float texBlend03;		
uniform float texBlend04;

// .... and so on... 

uniform bool isASkyBox;				// True samples the skybox texture	
uniform samplerCube texSampCube00;
uniform samplerCube texSampCube01;
uniform samplerCube texSampCube02;
uniform samplerCube texSampCube03;
uniform float texCubeBlend00;
uniform float texCubeBlend01;
uniform float texCubeBlend02;
uniform float texCubeBlend03;

uniform bool bDiscardTexture;

// For env. mapping (reflection and refraction)
uniform bool isReflectRefract;
uniform float reflectBlendRatio;		// How much reflection (0-1)
uniform float refractBlendRatio;		// How much refraction (0-1)
uniform float coefficientRefract; 		// coefficient of refraction 


//uniform bool bIsSecondPass;			// True if the render is doing 2nd pass  
uniform int renderPassNumber;			// Now there are 3 passes, 0 to 2
uniform float iTime;

//uniform sampler2D tex2ndPassSamp2D;		// Offscreen texture for 2nd pass
uniform sampler2D texFBOColour2D;
uniform sampler2D texFBONormal2D;
uniform sampler2D texFBOVertexWorldPos2D;

uniform sampler2D texFBOColour2D2;
uniform sampler2D texFBONormal2D2;
uniform sampler2D texFBOVertexWorldPos2D2;

uniform sampler2D fullRenderedImage2D;
uniform sampler2D fullRenderedImage2D2;



uniform float screenWidth;
uniform float screenHeight;


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow

	//fVertNormal.xyz; //normal of vertex
    float bias = max(0.003 * (1.0 - dot(fVertNormal.xyz, lightDir)), 0.0013); 
	//float bias = 0.005;
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

	if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
} 

// This function has now been moved to the bottom of the file...
// Calculate the contribution of a light at a vertex
vec3 calcLightColour( in vec3 vecNormal, 
                      in vec3 vecWorldPosition, 
                      in int lightID, 
					  in vec3 matDiffuse, 
                      in vec4 matSpecular );
/*****************************************************/

vec2 hash( vec2 p )
{
	p = vec2( dot(p,vec2(127.1,311.7)),
			 dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
	const float K1 = 0.366025404; // (sqrt(3)-1)/2;
	const float K2 = 0.211324865; // (3-sqrt(3))/6;
	
	vec2 i = floor( p + (p.x+p.y)*K1 );
	
	vec2 a = p - i + (i.x+i.y)*K2;
	vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0);
	vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;
	
	vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	
	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
	
	return dot( n, vec3(70.0) );
}

float fbm(vec2 uv)
{
	float f;
	mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
	//f  = 0.5000*noise( uv ); uv = m*uv;
	f += 0.2500*noise( uv ); uv = m*uv;
	f += 0.1250*noise( uv ); uv = m*uv;
	f += 0.0625*noise( uv ); uv = m*uv;
	f = 0.5 + 0.5*f;
	return f;
}


const float CALCULATE_LIGHTING = 1.0f;
const float DONT_CALCULATE_LIGHTING = 0.25f;

const int PASS_0_G_BUFFER_PASS = 0;
const int PASS_1_DEFERRED_RENDER_PASS = 1;
const int SHADOW_PASS = 2;


void main()
{	

	FBOout_colour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	FBOout_normal = vec4( 0.0f, 0.0f, 0.0f, DONT_CALCULATE_LIGHTING );
	FBOout_vertexWorldPos = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	switch (renderPassNumber)
	{
	case PASS_0_G_BUFFER_PASS:	 // =0
	
		// Is this a 'debug' wireframe object, i.e. no lighting, just use diffuse
		if ( bIsDebugWireFrameObject )
		{
			FBOout_colour.rgb = materialDiffuse.rgb;
			FBOout_colour.a = materialDiffuse.a;	
			FBOout_vertexWorldPos.xyz = fVecWorldPosition.xyz;
			FBOout_normal.a = DONT_CALCULATE_LIGHTING;

			return;		// Immediate return
		}

		
		if ( isASkyBox )
		{	// Sample from skybox texture and exit
			vec4 skyRGBA = texture( texSampCube00, fVertNormal.xyz );
			FBOout_colour = vec4(skyRGBA.rgb, 1.0f);
			FBOout_vertexWorldPos.xyz = fVecWorldPosition.xyz;
			FBOout_normal.a = DONT_CALCULATE_LIGHTING;
			return;	
		}
		

		else if ( isReflectRefract )
		{			
			

			vec3 vecReflectEyeToVertex = fVecWorldPosition - eyePosition;
			vecReflectEyeToVertex = normalize(vecReflectEyeToVertex);
			vec3 vecReflect = reflect( vecReflectEyeToVertex, fVertNormal.xyz );

			vec4 rgbReflection = texture( texSampCube00, fVertNormal.xyz );		
			vec3 vecReFRACT_EyeToVertex = eyePosition - fVecWorldPosition;
			vecReFRACT_EyeToVertex = normalize(vecReFRACT_EyeToVertex);				
			vec3 vecRefract = refract( vecReFRACT_EyeToVertex, fVertNormal.xyz, 
									   coefficientRefract );
			// Look up colour for reflection
			vec4 rgbRefraction = texture( texSampCube00, vecRefract );
			
			// Mix the two, based on how reflective the surface is
			FBOout_colour = (rgbReflection * reflectBlendRatio) + 
							 (rgbRefraction * refractBlendRatio);
			
			FBOout_vertexWorldPos.xyz = fVecWorldPosition.xyz;
			FBOout_normal.a = DONT_CALCULATE_LIGHTING;
			return;	
		}	
		
		// ***********************************************************************
		// This is the start of our "normal" "things that are lit" 1st pass
		// (the "geometry" pass - writing the geometry and other info)

		vec3 matDiffuse = vec3(0.0f, 0.0f, 0.0f);
		
		if ( bUseTextureAsDiffuse )
		{
			// ****************************************************************/
			//uniform sampler2D myAmazingTexture00;
			vec2 theUVCoords = fUV_X2.xy;		// use UV #1 of vertex
				
			vec4 texCol00 = texture( texSamp2D00, theUVCoords.xy );
			vec4 texCol01 = texture( texSamp2D01, theUVCoords.xy );
			vec4 texCol02 = texture( texSamp2D02, theUVCoords.xy );
			vec4 texCol03 = texture( texSamp2D03, theUVCoords.xy );
			vec4 texCol04 = texture( texSamp2D04, theUVCoords.xy );
			//... and so on (to how many textures you are using)
		//	
			// use the blend value to combine textures
			matDiffuse.rgb += (texCol00.rgb * texBlend00) + 
							  (texCol01.rgb * texBlend01) + 
							  (texCol02.rgb * texBlend02) + 
							  (texCol03.rgb * texBlend03) +
							  (texCol04.rgb * texBlend04);
				
			
			// We will look at specular or gloss maps later, 
			// 	but making the specular white is fine
			vec4 matSpecular = vec4(1.0f, 1.0f, 1.0f, 64.0f);


			// Add the ambient here (AFTER the lighting)
			// We have materialAmbient, but ambient is often 
			//	just a percentage ratio of the diffuse
			vec3 ambientContribution = matDiffuse.rgb * ambientToDiffuseRatio;
			//fragColourOut[0].rgb += ambientContribution.rgb;
			FBOout_colour.rgb += ambientContribution.rgb;
			// Transparency value (for alpha blending)
			FBOout_colour.a = materialDiffuse.a;

		}
		else
		{
		//for particles
			FBOout_colour = pColourFrag;
		}//if ( bUseTextureAsDiffuse )

		
		
		
		
		// These are set to the G-Buffer for the deferred pass
		FBOout_normal.rgb = fVertNormal.xyz;

		FBOout_vertexWorldPos.xyz = fVecWorldPosition.xyz;

		FBOout_normal.a = CALCULATE_LIGHTING;

		//FBOout_colour = texture( shadowMap, fUV_X2.xy );
		//FBOout_colour = fColor;

		if (slicedFrag > 0.5f)
		{
			FBOout_colour = texture( texSamp2D01, fUV_X2.xy );
			FBOout_colour.r *= 0.6f;
			FBOout_colour.g *= 0.6f;
			FBOout_colour.b *= 0.6f;
		}

		float shadow = ShadowCalculation(FragPosLightSpacePass); 
		if (shadow == 1.0f)
		{
			FBOout_normal.a = DONT_CALCULATE_LIGHTING;
		}

		if (bIsLookedAt)
		{
			vec3 outRGB = FBOout_colour.rgb * 12.0f;
			ivec3 outRGBi = ivec3(outRGB);
			FBOout_colour.rgb = vec3(outRGBi) / 12.0f;
			FBOout_colour.b = 1.0f;
		}
		if (bIsFire)
		{
			vec2 uv = fUV_X2.xy;
			vec2 q = uv;
			q.x *= 5.;
			q.y *= 2.;
			float strength = floor(q.x+1.);
			float T3 = max(3.,1.25*strength)*iTime;
			q.x = mod(q.x,1.)-0.5;
			q.y -= 0.25;
			float n = fbm(strength*q - vec2(0,T3));
			float c = 1. - 16. * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );
			float c1 = n * c * (1.5-pow(1.50*uv.y,4.));
			c1=clamp(c1,0.,1.);

			vec3 col = vec3(1.5*c1, 1.5*c1*c1*c1, c1*c1*c1*c1*c1*c1);
	
			float a = c * (1.-pow(uv.y,3.));
			FBOout_colour = vec4( mix(vec3(0.),col,a), 1.0);
			FBOout_normal.a = DONT_CALCULATE_LIGHTING;
			if (FBOout_colour.r <= 0.1f && FBOout_colour.g <= 0.1f && FBOout_colour.b <= 0.1f)
			{
				discard;
			}
			if (uv.x > 0.6 || uv.x < 0.4)
			{
				discard;
			}
		}

		if (!bUseLighting)
		{
			FBOout_normal.a = DONT_CALCULATE_LIGHTING;
			FBOout_colour.r *= 1.5;
			FBOout_colour.g *= 1.5;
			FBOout_colour.b *= 1.5;
		}
		
		break;	// end of PASS_0_G_BUFFER_PASS (0):
		
	case PASS_1_DEFERRED_RENDER_PASS:		// (1)
	
		
		vec2 textCoords = vec2( gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight );

		vec3 theColourAtThisPixel;
		vec4 theNormalAtThisPixel;
		vec4 theVertLocWorldAtThisPixel;

		theColourAtThisPixel = texture( texFBOColour2D, textCoords).rgb;
		theNormalAtThisPixel = texture( texFBONormal2D, textCoords).rgba;
		theVertLocWorldAtThisPixel = texture( texFBOVertexWorldPos2D, textCoords);

		
		
		if (bIsLookedAt)
		{
			vec3 sampleTex[9];
			for(int i = 0; i < 9; i++)
			{
				sampleTex[i] = vec3(texture(texFBOColour2D, textCoords.st + offsets[i]));
			}
			vec3 col = vec3(0.0);
			for(int i = 0; i < 9; i++)
				col += sampleTex[i] * edgeKernel[i];
			
			theColourAtThisPixel = col;
		}
    

		if (theNormalAtThisPixel.a == CALCULATE_LIGHTING)
		{
			// ELSE: do the lighting...
			for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
			{
				FBOout_colour.rgb += calcLightColour( theNormalAtThisPixel.xyz, 					
													   theVertLocWorldAtThisPixel.rgb, 
													   index, 
													   theColourAtThisPixel, 
													   materialSpecular );
			}
		}
		else
		{
			// Return the colour as it is on the colour FBO
			FBOout_colour.rgb = theColourAtThisPixel.rgb;
			FBOout_colour.a = 1.0f;
		}


		//the darkest we want something to get in the scene is just its base colour
		// && FBOout_colour.g < theColourAtThisPixel.g && FBOout_colour.b < theColourAtThisPixel.b
		if (FBOout_colour.r < theColourAtThisPixel.r)
		{
			FBOout_colour.r = theColourAtThisPixel.r;
			FBOout_colour.g = theColourAtThisPixel.g;
			FBOout_colour.b = theColourAtThisPixel.b;
		}
		


		FBOout_colour.rgb *= 1.5f;		//brighten up the whole scene
		FBOout_colour.a = 1.0f;

		if (bIsTitleScreen)
		{
			int interval1 = 13;
			int interval2 = 6;
			if (titleCounter > interval1)
				FBOout_colour.rgb = texture( texSamp2D00, textCoords ).rgb;
			else if (titleCounter > interval1-1)
			{
				float timeInSecond = titleCounter-(interval1-1);
				if (textCoords.x > timeInSecond)
					FBOout_colour.rgb = texture( texSamp2D01, textCoords ).rgb;
				else
				{
					textCoords.x /= timeInSecond;
					FBOout_colour.rgb = texture( texSamp2D00, textCoords ).rgb;
					//FBOout_colour.rgb = texture( texSamp2D00, textCoords ).rgb * timeInSecond;
					//FBOout_colour.rgb += texture( texSamp2D01, textCoords ).rgb * (1 -timeInSecond);
				}
			}
			else if (titleCounter > interval2)
				FBOout_colour.rgb = texture( texSamp2D01, textCoords ).rgb;
			else if (titleCounter > interval2-1)
			{
				float timeInSecond = titleCounter -(interval2-1);
				if (textCoords.x > timeInSecond)
					FBOout_colour.rgb = texture( texSamp2D02, textCoords ).rgb;
				else
				{
					textCoords.x /= timeInSecond;
					FBOout_colour.rgb = texture( texSamp2D01, textCoords ).rgb;
					//FBOout_colour.rgb = texture( texSamp2D01, textCoords ).rgb * timeInSecond;
					//FBOout_colour.rgb += texture( texSamp2D02, textCoords ).rgb * (1 -timeInSecond);
				}

			}
			else 
				FBOout_colour.rgb = texture( texSamp2D02, textCoords ).rgb;
		}
		else //otherwise draw the cursor
		{

		//cursor
		if (bIsSplitscreen)
		{
			//make 2 cursors instead of one
			//first crosshair   
			if (textCoords.y > 0.248f && textCoords.y < 0.252f && textCoords.x > 0.49 && textCoords.x < 0.51)
			{
				if (FBOout_colour.r > 0.8f && FBOout_colour.g > 0.8f && FBOout_colour.b > 0.8f)
				{
					FBOout_colour.r = 0.0f;
					FBOout_colour.g = 0.0f;
					FBOout_colour.b = 0.0f;
				}
				else
				{
					FBOout_colour.r = 1.0f;
					FBOout_colour.g = 1.0f;
					FBOout_colour.b = 1.0f;
				}
			}

			if (textCoords.x > 0.498f && textCoords.x < 0.502f && textCoords.y > 0.24 && textCoords.y < 0.26)
			{
				if (FBOout_colour.r > 0.8f && FBOout_colour.g > 0.8f && FBOout_colour.b > 0.8f)
				{
					FBOout_colour.r = 0.0f;
					FBOout_colour.g = 0.0f;
					FBOout_colour.b = 0.0f;
				}
				else
				{
					FBOout_colour.r = 1.0f;
					FBOout_colour.g = 1.0f;
					FBOout_colour.b = 1.0f;
				}
			}
			//second crosshair
			if (textCoords.y > 0.748f && textCoords.y < 0.752f && textCoords.x > 0.49 && textCoords.x < 0.51)
			{
				if (FBOout_colour.r > 0.8f && FBOout_colour.g > 0.8f && FBOout_colour.b > 0.8f)
				{
					FBOout_colour.r = 0.0f;
					FBOout_colour.g = 0.0f;
					FBOout_colour.b = 0.0f;
				}
				else
				{
					FBOout_colour.r = 1.0f;
					FBOout_colour.g = 1.0f;
					FBOout_colour.b = 1.0f;
				}
			}

			if (textCoords.x > 0.498f && textCoords.x < 0.502f && textCoords.y > 0.74 && textCoords.y < 0.76)
			{
				if (FBOout_colour.r > 0.8f && FBOout_colour.g > 0.8f && FBOout_colour.b > 0.8f)
				{
					FBOout_colour.r = 0.0f;
					FBOout_colour.g = 0.0f;
					FBOout_colour.b = 0.0f;
				}
				else
				{
					FBOout_colour.r = 1.0f;
					FBOout_colour.g = 1.0f;
					FBOout_colour.b = 1.0f;
				}
			}
		}
		else
		{
			if (textCoords.y > 0.498f && textCoords.y < 0.502f && textCoords.x > 0.49 && textCoords.x < 0.51)
			{
				if (FBOout_colour.r > 0.8f && FBOout_colour.g > 0.8f && FBOout_colour.b > 0.8f)
				{
					FBOout_colour.r = 0.0f;
					FBOout_colour.g = 0.0f;
					FBOout_colour.b = 0.0f;
				}
				else
				{
					FBOout_colour.r = 1.0f;
					FBOout_colour.g = 1.0f;
					FBOout_colour.b = 1.0f;
				}
			}

			if (textCoords.x > 0.498f && textCoords.x < 0.502f && textCoords.y > 0.49 && textCoords.y < 0.51)
			{
				if (FBOout_colour.r > 0.8f && FBOout_colour.g > 0.8f && FBOout_colour.b > 0.8f)
				{
					FBOout_colour.r = 0.0f;
					FBOout_colour.g = 0.0f;
					FBOout_colour.b = 0.0f;
				}
				else
				{
					FBOout_colour.r = 1.0f;
					FBOout_colour.g = 1.0f;
					FBOout_colour.b = 1.0f;
				}
			}
		}
		}
		
		// "2nd pass effects"
		// ******************************************************************
	
		break;	// end of pass PASS_1_DEFERRED_RENDER_PASS (1)
	
	case SHADOW_PASS:
	
		break;	// end of pass SHADOW_PASS:
	
	}// switch (passNumber)
	

	return;
}// main()








vec3 calcLightColour( in vec3 vecNormal, 
                      in vec3 vecWorldPosition, 
                      in int lightID, 
                      in vec3 matDiffuse, 	// ADDED
                      in vec4 matSpecular )	// ADDED
{
	vec3 colour = vec3( 0.0f, 0.0f, 0.0f );
	
	vec3 outDiffuse = vec3( 0.0f, 0.0f, 0.0f );
	vec3 outSpecular = vec3( 0.0f, 0.0f, 0.0f );
	

	// Distance between light and vertex (in world)
	vec3 vecToLight = myLight[lightID].position.xyz - vecWorldPosition;
	// How long is this vector?
	float lightDistance = length(vecToLight);
	// Short circuit distance
	if ( myLight[lightID].typeParams.y <  lightDistance )
	{
		return colour;
	}
	
	// The "normalized" vector to the light (don't care about distance, just direction)
	vec3 lightVector = normalize( vecToLight );
	
	// The normalized version of the vertex normal
	vecNormal = normalize(vecNormal);
	
	// How much diffuse light is being "reflected" off the surface 
	float diffFactor = max(0.0f, dot( lightVector, vecNormal ));
	
	
	outDiffuse.rgb = myLight[lightID].diffuse.rgb 		// Light contribution
	                 * matDiffuse.rgb				// Material contribution
					 * diffFactor;						// Factor based on direction
					 

	float attenuation = 1.0f / 
	   ( myLight[lightID].attenuation.x // 0  
	   + myLight[lightID].attenuation.y * lightDistance			// Linear
	   + myLight[lightID].attenuation.z * lightDistance * lightDistance );	// Quad
	outDiffuse.rgb *= attenuation;

	
	// Vector from vertex to eye 
	// i.e. this makes the vector base effectively at zero.
	vec3 viewVector = normalize( eyePosition - vecWorldPosition );
	vec3 vecLightReflection = reflect( normalize(lightVector), vecNormal );
	
	float specularShininess = matSpecular.w;	// 64.0f
	vec3 specMatColour = matSpecular.rgb;		// vec3(1.0f, 1.0f, 1.0f);
	
	outSpecular.rgb = pow( max(0.0f, dot(viewVector,vecLightReflection)), 
	                  specularShininess)
			          * specMatColour
				      * myLight[lightID].specular.rgb;// // myLightSpecular;
				   
	outSpecular *= attenuation;
	
	
// For now, to simplify, eliminate the specular
//	colour = outDiffuse + outSpecular;
	colour = outDiffuse;


	// Now we have the colour if this was a point light 
	// See if it's a spot light...
	if ( myLight[lightID].typeParams.x == 2.0f ) 			// x = type
	{	// Then it's a spot light! 
		
		// Vector from the vertex to the light... 
		vec3 vecVertexToLight = vecWorldPosition - myLight[lightID].position.xyz;
		// Normalize to unit length vector
		vec3 vecVtoLDirection = normalize(vecVertexToLight);
		
		float vertSpotAngle = max(0.0f, dot( vecVtoLDirection, myLight[lightID].direction.xyz ));
		// Is this withing the angle1?
		
		float angleInsideCutCos = cos(myLight[lightID].typeParams.z);		// z angle1
		float angleOutsideCutCos = cos(myLight[lightID].typeParams.w);		// z angle2
		
		if ( vertSpotAngle <= angleOutsideCutCos )
		{	// NO, it's outside this angle1
			colour = vec3(0.0f, 0.0f, 0.0f );
		}
		else if ( (vertSpotAngle > angleOutsideCutCos ) && 
		          (vertSpotAngle <= angleInsideCutCos) )
		{	// inside the 'penumbra' region
			// Use smoothstep to get the gradual drop off in brightness
			float penumbraRatio = smoothstep(angleOutsideCutCos, 
			                                 angleInsideCutCos, 
										     vertSpotAngle );          
			
			colour *= penumbraRatio;
		}

	}//if ( typeParams.x

	
	return colour;
}// vec3 calcLightColour(...) 





