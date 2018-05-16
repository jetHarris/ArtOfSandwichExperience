#ifndef _cMesh_HG_
#define _cMesh_HG_

#include "sVertex_xyz_rgba_n_uv2.h"
#include "cTriangle.h" 

#include <string>
#include <glm/vec3.hpp>

class cMesh
{
public:
	cMesh();
	~cMesh();

	std::string name;	// the friendly name of the mesh

	//all the vertices of the mesh
	sVertex_xyz_rgba_n_uv2* pVertices;
	int numberOfVertices;

	//all the triangles of the mesh
	cTriangle* pTriangles;
	int numberOfTriangles;

	// First 2 are the "bounding box" edges
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	// This is Max - Min for each x,y,z
	glm::vec3 maxExtentXYZ;	
	float maxExtent;		// Biggest of above
	float scaleForUnitBBox;

	void CalculateExtents(void);

	void CalculateNormals(void);
};


#endif
