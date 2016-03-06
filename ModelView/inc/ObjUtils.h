#ifndef MESH_OBJ
#define MESH_OBJ
#include <iostream>
#include <sstream>
#include <fstream>
#include <gl.h>
#include <map>
#include <set>
#include <vector>
#include <string>

using namespace std;

class Mesh_Obj
{
private:
	GLuint VAO[1];
	enum VertexAttrib{ Coord, normalDir, texCoord, vertexID };
	GLuint VBO[4];
	GLuint IBO;
	GLuint TangentBO;
	unsigned VertexAttribNumbers[texCoord + 1];
	unsigned EdgeSum, VertexSum, IndexSum;
	string Dir;
	enum ScaleInfo{ scalex, scaley, scalez, scaleuv };
	float Scale[scaleuv + 1];
	bool is_Attached;
	void RawAttach();
public:
	Mesh_Obj() :is_Attached(false){};
	Mesh_Obj(string FileDirectory, float scalePosx = 1.0f, float scalePosy = 1.0f, float scalePosz = 1.0f, float scaleTex = 1.0f);
	void Attach();
	void Output_to_Files();
	void Calc_Tangent();
	GLuint getVertexArray()
	{
		return this->VAO[0];
	}
	GLuint getCoordBuffer()
	{
		return this->VBO[Coord];
	}
	GLuint getNormalBuffer()
	{
		return this->VBO[normalDir];
	}
	GLuint getTexCoordBuffer()
	{
		return this->VBO[texCoord];
	}
	GLuint getIDBuffer()
	{
		return this->VBO[vertexID];
	}
	void DrawTriangles();
	void Detach();
};
#endif