#include "GameAssetManager.h"

#include "../ErrorLogger.h"

#include "../Helper.h"

void GameAssetManager::LoadAllAssetsTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->Mesh_AM->LoadAllMeshDataTo_GPU_RAM(device, commandList);
	this->Texture_AM->LoadAllTextureDataTo_GPU_RAM(commandList);
}

void GameAssetManager::RemoveAssetsFrom_CPU_RAM()
{
	this->Mesh_AM->RemoveAllMeshDataFrom_CPU_RAM();
	this->Texture_AM->RemoveAllTextureDataFrom_CPU_RAM();
}

void GameAssetManager::SetAllTextureViewToRespectiveHeaps(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	Texture* texture = nullptr;
	for (unsigned int i = 0; i < this->ToBeAppliedHandles.size(); i++)
	{
		PreHandleStructure handleStruct = this->ToBeAppliedHandles[i];
		texture = this->Texture_AM->GetTexture(handleStruct.textureIndex);
		texture->CreateResourceView(device, handleStruct.handle);
		texture = nullptr;
	}
}

// Function to process each mesh from the model and submit it to Mesh Asset Manager and return the mesh index.
UINT64 GameAssetManager::ProcessMesh(aiMesh* mesh)
{
	std::vector<MeshVertex> vertices = {};
	std::vector<unsigned short> indices = {};
	
	// For each vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D toLoadPosition = mesh->mVertices[i];
		// Load Position.
		DirectX::XMFLOAT3 position = { toLoadPosition.x, toLoadPosition.y, toLoadPosition.z };
	
		aiVector3D toLoadNormals = mesh->mNormals[i];
		// Load Normals.
		DirectX::XMFLOAT3 normals = { toLoadNormals.x, toLoadNormals.y, toLoadNormals.z };
	
		// Load Texture Coordinates.
		DirectX::XMFLOAT2 texCoords = {};

		aiVector3D toLoadTangents = mesh->mTangents[i];
		// Load Tangents.
		DirectX::XMFLOAT3 tangent = { toLoadTangents.x, toLoadTangents.y, toLoadTangents.z };

		aiVector3D toLoadBitangents = mesh->mBitangents[i];
		// Load Bitangents.
		DirectX::XMFLOAT3 bitangent = { toLoadBitangents.x, toLoadBitangents.y, toLoadBitangents.z };

		if (mesh->mTextureCoords[0])
		{
			aiVector3D toLoadTextureCoordinates = mesh->mTextureCoords[0][i];
			texCoords = { toLoadTextureCoordinates.x, toLoadTextureCoordinates.y };
		}
	
		vertices.push_back(MeshVertex(position, normals, texCoords, tangent, bitangent));
	}
	
	// For each index
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return this->Mesh_AM->SetNewMesh(vertices, indices);
}

// Function to process the textures from the model, submit it to Texture Asset Manager and return the texture index.
UINT64 GameAssetManager::ProcessColorTexture(aiMesh* mesh, const aiScene* scene, std::string modelFileLocation)
{
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiString fileLocationStr;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &fileLocationStr);
		if (std::strcmp(fileLocationStr.C_Str(), "") != 0)
		{
			return this->SetNewTexture((modelFileLocation + fileLocationStr.C_Str()).c_str());
		}
	}

	return this->SetNewTexture(DefaultTextureDirectory::WhiteTexture);
}

UINT64 GameAssetManager::ProcessNormalTexture(aiMesh* mesh, const aiScene* scene, std::string modelFileLocation)
{
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiString fileLocationStr;
		material->GetTexture(aiTextureType_HEIGHT, 0, &fileLocationStr);
		if (std::strcmp(fileLocationStr.C_Str(), "") != 0)
		{
			return this->SetNewTexture((modelFileLocation + fileLocationStr.C_Str()).c_str());
		}
	}

	return this->SetNewTexture(DefaultTextureDirectory::NormalTexture);
}

// Function to process individual nodes of the mesh of the model.
void GameAssetManager::ProcessNode(aiNode* node, const aiScene* scene, ModelInfo* modelInfo, std::string modelFileLocation)
{
	// Process all mesh nodes if any.
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		ModelFragment mBit;

		// Mesh assignment.
		mBit.meshIndex = ProcessMesh(mesh);

		// Color texture assignment.
		mBit.colorTextureIndex = ProcessColorTexture(mesh, scene, modelFileLocation);

		// Normal texture assignment.
		mBit.normalTextureIndex = ProcessNormalTexture(mesh, scene, modelFileLocation);

		modelInfo->modelFragments.push_back(mBit);
	}
	// Process the all the nodes present as children.
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, modelInfo, modelFileLocation);
	}

	return;
}

ModelInfo GameAssetManager::LoadModel(std::string fileLocation)
{
	// Check if the file with same name exists and return the index.
	for (unsigned int i = 0; i < this->loadedModels.size(); i++)
	{
		if (std::strcmp(fileLocation.c_str(), this->loadedModels[i].fileName.c_str()) == 0)
		{
			return this->loadedModels[i];
		}
	}

	// If the file name does not exist in the model list then create a new model and return its related info.
	Assimp::Importer modelImporter;
	const aiScene* scene = modelImporter.ReadFile(fileLocation, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		ExitWithMessage("ASSIMP IMPORT ERROR");
	}

	// Store the model detail with file name.
	ModelInfo modelInfo = {};

	std::string modelFileLocation = fileLocation;
	size_t last_slash = modelFileLocation.find_last_of("/");
	modelFileLocation.erase(last_slash + 1);

	ProcessNode(scene->mRootNode, scene, &modelInfo, modelFileLocation);
	modelInfo.fileName = fileLocation;

	this->loadedModels.push_back(modelInfo);

	return modelInfo;
}

UINT64 GameAssetManager::SetNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices)
{
	return this->Mesh_AM->SetNewMesh(vertices, indices);
}

UINT64 GameAssetManager::SetNewGeometry2D(std::vector<MeshVertex2D> vertices, std::vector<unsigned short> indices)
{
	return this->Mesh_AM->SetNewGeometry2D(vertices, indices);
}

Mesh* GameAssetManager::GetMesh(UINT64 meshIndex)
{
	return this->Mesh_AM->GetMesh(meshIndex);
}

UINT64 GameAssetManager::SetNewTexture(std::string fileLocation)
{
	std::string sanitizedFileLocation = GetFileNameFromDirectory(fileLocation);

	// Check for any same loaded textures and return the texture index if any.
	for (unsigned int i = 0; i < this->loadedTextures.size(); i++)
	{
		if (std::strcmp(sanitizedFileLocation.c_str(), this->loadedTextures[i].fileName.c_str()) == 0)
			return this->loadedTextures[i].textureIndex;
	}

	TextureInfo textureInfo = {};

	textureInfo.textureIndex = this->Texture_AM->SetNewTexture(fileLocation.c_str());
	textureInfo.fileName = sanitizedFileLocation.c_str();

	this->loadedTextures.push_back(textureInfo);

	return textureInfo.textureIndex;
}

Texture* GameAssetManager::GetTexture(UINT64 textureIndex)
{
	return this->Texture_AM->GetTexture(textureIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE GameAssetManager::GetTextureHandle(UINT64 textureIndex)
{
	return this->Texture_AM->GetTextureHandleForRender(textureIndex);
}

void GameAssetManager::AssignTextureViewToHeap(UINT64 textureIndex, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->ToBeAppliedHandles.push_back({ textureIndex, handle });
}

void GameAssetManager::ReleaseAllAssets()
{
	this->Texture_AM->ReleaseAllTextures();
	this->Mesh_AM->ReleaseAllMeshes();
}

void GameAssetManager::SetGraphicsDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->Mesh_AM->SetDevice(device);
	this->Texture_AM->SetDevice(device);
}

#include "../Assets/UnlitShader/UnlitShader.h"
#include "../Assets/TextureShader/TextureShader.h"
#include "../Assets/CubemapShader/CubemapShader.h"
#include "../Assets/InstanceShader/InstanceShader.h"

void GameAssetManager::InitializeAllShadersForDeferredRender(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfRenderTarget, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	UnlitShader::GetInstance()->Initialize(device, numberOfRenderTarget, renderTargetFormats, depthStencilFormat, samples);
	TextureShader::GetInstance()->Initialize(device, numberOfRenderTarget, renderTargetFormats, depthStencilFormat, samples);
	CubemapShader::GetInstance()->Initialize(device, numberOfRenderTarget, renderTargetFormats, depthStencilFormat, samples);
	InstanceShader::GetInstance()->Initialize(device, numberOfRenderTarget, renderTargetFormats, depthStencilFormat, samples);
}

#include "../Assets/2D_Assets/Shaders/WireframeShader/WireframeShader.h"
#include "../Assets/2D_Assets/Shaders/CharacterShader/CharacterDisplayShader.h"

void GameAssetManager::InitializeAllShadersFor2DDeferredRender(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfRenderTarget, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	WireframeShader::GetInstance()->Initialize(device, numberOfRenderTarget, renderTargetFormats, depthStencilFormat, samples);
	CharacterDisplayShader::GetInstance()->Initialize(device, numberOfRenderTarget, renderTargetFormats, depthStencilFormat, samples);
}
