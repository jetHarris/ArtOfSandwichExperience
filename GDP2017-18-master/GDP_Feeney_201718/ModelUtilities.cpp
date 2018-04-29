#include "ModelUtilities.h" 
#include "cVAOMeshManager.h"
#include "cMesh.h"
#include <sstream>
#include <vector>
#include "cGameObject.h"
#include "cModelAssetLoader.h"
#include "assimp/cAssimpBasic.h"
#include <iostream>
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

// Declared in globalStuff.h

// Returns 0 or NULL if not found
cGameObject* findObjectByFriendlyName(std::string friendlyName, std::vector<cGameObject*> &vec_pGameObjects)
{
	// Linear search, baby!
	unsigned int numObjects = (unsigned int)vec_pGameObjects.size();
	for (unsigned int index = 0; index != numObjects; index++)
	{
		if (vec_pGameObjects[index]->friendlyName == friendlyName)
		{
			return vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

cGameObject* findObjectByUniqueID(unsigned int ID, std::vector<cGameObject*> &vec_pGameObjects)
{
	// Linear search, baby!
	unsigned int numObjects = (unsigned int)vec_pGameObjects.size();
	for (unsigned int index = 0; index != numObjects; index++)
	{
		if ( ID = vec_pGameObjects[index]->getUniqueID() )
		{
			return vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

// Called by Load3DModelsIntoMeshManager() function
bool Load3DModelHelper( std::string fileName, 
					   std::string meshFriendlyName,
					   int shaderID, 
					   cVAOMeshManager* pVAOManager,
					   cModelAssetLoader* pModelAssetLoader,
					   bool bModelHasUVs, 
					   std::string &error )
{
	bool bAllGood = true;

	cMesh tempMesh;
	tempMesh.name = meshFriendlyName;

	std::stringstream ssError;

	if (bModelHasUVs)
	{
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV(fileName, tempMesh))
		{
			ssError << "Didn't load model >" << tempMesh.name << "<" << std::endl;
			bAllGood = false;
		}
	}
	else
	{
		if ( !pModelAssetLoader->LoadPlyFileIntoMeshWithNormals(fileName, tempMesh) )
		{
			ssError << "Didn't load model >" << tempMesh.name << "<" << std::endl;
			bAllGood = false;
		}
	}//if (bModelHasUVs)

	if ( bAllGood )
	{
			if (!pVAOManager->loadMeshIntoVAO(tempMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << tempMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}//if ( bAllGood )

	return bAllGood;
}


bool Load3DModelsIntoMeshManager( int shaderID, 
								  cVAOMeshManager* pVAOManager, 
								  cModelAssetLoader* pModelAssetLoader,
								  std::string &error )
{
	std::stringstream ssError;
	bool bAllGood = true;

	
	// *******************************************************
	// Test the assimp loader before the "old" loader
	cAssimpBasic myAssimpLoader;

	
	if ( !Load3DModelHelper( "Just_Inside_Door_Frame_for_Masking.ply", 
							 "Just_Inside_Door_Frame_for_Masking.ply", 
							 shaderID, pVAOManager, pModelAssetLoader, true, error ) )
	{
		std::cout << error << std::endl;
	}
	if ( !Load3DModelHelper( "Room_2_Bigger_Triangulated.ply", 
							 "Room_2_Bigger_Triangulated.ply", 
							 shaderID, pVAOManager, pModelAssetLoader, true, error ) )
	{
		std::cout << error << std::endl;
	}

	{
		cMesh testMesh;
		testMesh.name = "SmoothSphereRadius1InvertedNormals";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "SmoothSphere_Inverted_Normals_xyz_n.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}	



	{
		cMesh testMesh;
		testMesh.name = "knife.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("knife.ply", testMesh))
		{
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "lowPolySphere.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("lowPolySphere.ply", testMesh))
		{
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "toaster.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("toaster.ply", testMesh))
		{
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "frontWall.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("frontWall.ply", testMesh))
		{
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "plainPlane.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("plainPlane.ply", testMesh))
		{
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "plate.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("plate.ply", testMesh))
		{
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "lava3.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("lava3.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "plainPlane.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("plainPlane.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "olive.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("olive.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "breadLoaf.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("breadLoaf.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "breadSlice.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("breadSlice.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "cheese.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("cheese.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "cuttingBoard.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("cuttingBoard.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}

		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "cuttingBoard2.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("cuttingBoard2.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}

		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "meat.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("meat.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}

	{
		cMesh testMesh;
		testMesh.name = "tomato.ply";
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV("tomato.ply", testMesh))
		{
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}


	if ( ! bAllGood ) 
	{
		// Copy the error string stream into the error string that
		//	gets "returned" (through pass by reference)
		error = ssError.str();
	}

	return bAllGood;
}