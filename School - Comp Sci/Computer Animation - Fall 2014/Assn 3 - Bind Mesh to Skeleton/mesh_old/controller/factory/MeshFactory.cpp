#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "MeshFactory.h"


MeshFactory::MeshFactory() : tokenizer()
{
}

MeshFactory::~MeshFactory()
{
}

std::vector<std::string> MeshFactory::tokenizeFile(std::string& _fileName)
{
	//tokenizer.OpenFile(_fileName);
	//
	//std::string token;
	//while(tokenizer.GetToken(token)){
	//	tokens.push_back(token);
	//	token.clear();
	//}
	//
	//return tokens;
	tokenizer.openFile(_fileName);
	tokenizer.putFileInBuffer();
	std::vector<std::string> temp = tokenizer.tokenize();
	tokenizer.close();
	return temp;
}

Mesh* MeshFactory::generateMeshFromFile(const char* _filename, Skeleton* _skel){
	std::string strFilename(_filename);
	return generateMeshFromFile(strFilename, _skel);
}

Mesh* MeshFactory::generateMeshFromFile(std::string& _filename, Skeleton* _skel)
{
	skeleton = _skel;
	
	meshParser = MeshParser(tokenizeFile(_filename), _skel);
	//we give the mesh parser access to the skeleton
	meshParser.parseTokens();

	mesh = &meshParser.mesh;

	//now that the mesh verts have been input, we can weight them
	weightVertices();

	
	//after the parser has populated all the fields, the MeshFactory will 
	//do some stuff to derive the rest of the data
	
	return mesh;
}

void MeshFactory::weightVertices()
{
	//for every vertex...
	for(int i = 0; i < mesh->vertices.size(); i++){
		
		Vertex vert = mesh->vertices[i];
		Vector4 weightedVector;

		//for every weight in current vertex...
		for (auto & weight : vert.weights) {
			auto currentJointIndex = weight.index;
			auto transform = (*skeleton)[currentJointIndex].transform;
			auto w = weight.weight;
			weightedVector += (w * vert.position * transform);
		}
		
		//for(int j = 0; j < vert.weights.size(); j++){
		//
		//	int currentJointIndex = vert.weights[j].index;
		//	
		//	Matrix4 transform = skeleton->operator[](currentJointIndex).transform;
		//
		//	float weight = vert.weights[j].weight;
		//
		//	weightedVector += (weight * vert.position * transform);
		//	//weightedVector += (weight * transform * vert.position);
		//}

		Vertex weightedVert;
		weightedVert.position[0] = weightedVector[0];
		weightedVert.position[1] = weightedVector[1];
		weightedVert.position[2] = weightedVector[2];

		mesh->vertices[i] = weightedVert;

		//reset weightVertVect
		//weightedVertVect[0] = 0; weightedVertVect[1] = 0; weightedVertVect[2] = 0; weightedVertVect[3] = 0;
	}
}
