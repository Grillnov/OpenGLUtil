#include <ObjUtils.h>

struct vec3
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	vec3(){};
	vec3(double x, double y, double z) :x(x), y(y), z(z){}
};

struct Vertex
{
	GLfloat vertexCoord[3];
	GLfloat normalDir[3];
	GLfloat texCoord[3];
	unsigned ID;

	Vertex(const vec3& Coord, const vec3& Normal, const vec3& tex, unsigned ID)
	{
		this->ID = ID;
		vertexCoord[0] = Coord.x;
		vertexCoord[1] = Coord.y;
		vertexCoord[2] = Coord.z;

		normalDir[0] = Normal.x;
		normalDir[1] = Normal.y;
		normalDir[2] = Normal.z;

		texCoord[0] = tex.x;
		texCoord[1] = tex.y;
		texCoord[2] = tex.z;
	}
};

struct Edge
{
	GLuint index1;
	GLuint index2;
	GLuint offset;
};

bool operator<(const Vertex& lhs, const Vertex& rhs)
{
	return lhs.ID < rhs.ID;
}

bool operator==(const Vertex& lhs, const Vertex& rhs)
{
	return lhs.ID == rhs.ID;
}

Mesh_Obj::Mesh_Obj(string FileDirectory, float scalePosx, float scalePosy, float scalePosz, float scaleTex)
:EdgeSum(0), is_Attached(false)
{
	for (int i = Coord; i != texCoord; ++i)
	{
		VertexAttribNumbers[i] = 0;
	}

	this->Dir = FileDirectory;
	this->Scale[scalex] = scalePosx;
	this->Scale[scaley] = scalePosy;
	this->Scale[scalez] = scalePosz;
	this->Scale[scaleuv] = scaleTex;
}

void Mesh_Obj::RawAttach()
{
	if (this->is_Attached)
		return;
	else
		this->is_Attached = true;
	fstream fin(this->Dir, ios::in);

	if (!fin.is_open())
	{
		fin.close();
		cout << this->Dir << " :Obj Not Found!" << endl;
		exit(1);
	}
	else
	{
		cout << this->Dir << " :Obj Loading Now!" << endl;
	}

	fin.seekg(0, ios::end);
	size_t fileSize = fin.tellg();
	fin.seekg(0);
	string source;
	source.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	fin.close();

	vector<vec3> VertexAttribTemp[texCoord + 1];
	vector<GLuint> ElementArrayTemp;
	set<Vertex> VertexBuffer;
	set<Edge> EdgeTemp;

	glGenVertexArrays(1, VAO);
	glGenBuffers(4, VBO);
	glBindVertexArray(VAO[0]);

	stringstream objStream(source);
	while (!objStream.eof())
	{
		string line;
		getline(objStream, line);
		stringstream lineStream(line);
		char numb;
		if (line.empty())
			continue;
		switch (line.at(0))
		{
		case 'v':
			switch (line.at(1))
			{
			case ' ':
				float CoordData[3];
				lineStream >> numb >> CoordData[0] >> CoordData[1] >> CoordData[2];
				VertexAttribTemp[Coord].push_back(vec3(CoordData[0], CoordData[1], CoordData[2]));
				break;
			case 'n':
				float normalData[3];
				lineStream >> numb >> numb >> normalData[0] >> normalData[1] >> normalData[2];
				VertexAttribTemp[normalDir].push_back(vec3(normalData[0], normalData[1], normalData[2]));
				break;
			case 't':
				float texCoordData[3];
				char numb;
				lineStream >> numb >> numb >> texCoordData[0] >> texCoordData[1] >> texCoordData[2];
				VertexAttribTemp[texCoord].push_back(vec3(texCoordData[0], texCoordData[1], texCoordData[2]));
				break;
			default:
				break;
			}
			break;
		case 'f':
			lineStream >> numb;
			if (line.find("//") != line.npos)
			{
				for (int i = 0; i != 3; ++i)
				{
					unsigned PosTexNor[3];
					lineStream >> PosTexNor[Coord] >> numb >> numb >> PosTexNor[normalDir];
					for (int i = 0; i != 3; ++i)
					{
						PosTexNor[i]--;
					}
					ElementArrayTemp.push_back(PosTexNor[0]);
					VertexBuffer.insert(Vertex(VertexAttribTemp[Coord].at(PosTexNor[Coord]), VertexAttribTemp[normalDir].at(PosTexNor[normalDir]), vec3(0, 0, 0), PosTexNor[0]));
				}
			}
			else
			{
				for (int i = 0; i != 3; ++i)
				{
					unsigned PosTexNor[3];
					lineStream >> PosTexNor[Coord] >> numb >> PosTexNor[texCoord] >> numb >> PosTexNor[normalDir];
					for (int i = 0; i != 3; ++i)
					{
						PosTexNor[i]--;
					}
					ElementArrayTemp.push_back(PosTexNor[0]);
					VertexBuffer.insert(Vertex(VertexAttribTemp[Coord].at(PosTexNor[Coord]), VertexAttribTemp[normalDir].at(PosTexNor[normalDir]), VertexAttribTemp[texCoord].at(PosTexNor[texCoord]), PosTexNor[0]));
				}
			}
			break;
		default:
			break;
		}
	}

	for (int i = 0; i != 3; ++i)
	{
		VertexAttribTemp[i].clear();
	}

	VertexSum = VertexBuffer.size();
	IndexSum = ElementArrayTemp.size();

	glBindBuffer(GL_ARRAY_BUFFER, VBO[Coord]);
	GLfloat* VertexlocalBuffer = new GLfloat[4 * VertexSum];
	unsigned counter = 0;
	for (auto a : VertexBuffer)
	{
		VertexlocalBuffer[4 * counter] = a.vertexCoord[0] / this->Scale[scalex];
		VertexlocalBuffer[4 * counter + 1] = a.vertexCoord[1] / this->Scale[scaley];
		VertexlocalBuffer[4 * counter + 2] = a.vertexCoord[2] / this->Scale[scalez];
		VertexlocalBuffer[4 * counter + 3] = 1.0f;
		++counter;
	}
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat)*VertexSum, VertexlocalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	delete[] VertexlocalBuffer;

	VertexlocalBuffer = new GLfloat[3 * VertexSum];
	glBindBuffer(GL_ARRAY_BUFFER, VBO[normalDir]);
	counter = 0;
	for (auto a : VertexBuffer)
	{
		VertexlocalBuffer[3 * counter] = a.normalDir[0];
		VertexlocalBuffer[3 * counter + 1] = a.normalDir[1];
		VertexlocalBuffer[3 * counter + 2] = a.normalDir[2];
		++counter;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*VertexSum, VertexlocalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	delete[] VertexlocalBuffer;

	VertexlocalBuffer = new GLfloat[3 * VertexSum];
	glBindBuffer(GL_ARRAY_BUFFER, VBO[texCoord]);
	counter = 0;
	for (auto a : VertexBuffer)
	{
		VertexlocalBuffer[3 * counter] = a.texCoord[0] / this->Scale[scaleuv];
		VertexlocalBuffer[3 * counter + 1] = a.texCoord[1] / this->Scale[scaleuv];
		VertexlocalBuffer[3 * counter + 2] = a.texCoord[2] / this->Scale[scaleuv];
		++counter;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*VertexSum, VertexlocalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	delete[] VertexlocalBuffer;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[vertexID]);
	counter = 0;
	GLuint* localIDBuffer = new GLuint[VertexSum];
	for (auto a : VertexBuffer)
	{
		localIDBuffer[counter] = a.ID;
		++counter;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint)*VertexSum, localIDBuffer, GL_STATIC_DRAW);
	glVertexAttribIPointer(3, 1, GL_INT, 0, nullptr);
	glEnableVertexAttribArray(3);
	delete[] localIDBuffer;

	VertexBuffer.clear();

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementArrayTemp.size()*sizeof(GLuint), ElementArrayTemp.data(), GL_STATIC_DRAW);
	cout << this->Dir << ": " << "Faces in triangles: " << ElementArrayTemp.size() / 3 << endl;

	ElementArrayTemp.clear();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh_Obj::DrawTriangles()
{
	if (this->is_Attached)
	{
		glBindVertexArray(this->VAO[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
		glDrawElements(GL_TRIANGLES, this->IndexSum, GL_UNSIGNED_INT, nullptr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		cout << this->Dir << "Not attached yet!" << endl;
		exit(1);
	}
}

void Mesh_Obj::Detach()
{
	if (!this->is_Attached)
		return;
	else
		this->is_Attached = false;
	glDeleteVertexArrays(1, this->VAO);
	glDeleteBuffers(4, this->VBO);
	glDeleteBuffers(1, &this->IBO);
}

void Mesh_Obj::Output_to_Files()
{
	fstream fin(this->Dir, ios::in);

	if (!fin.is_open())
	{
		fin.close();
		cout << this->Dir << " :Obj Not Found!" << endl;
		exit(1);
	}
	else
	{
		cout << this->Dir << " :Obj Outputting Now!" << endl;
	}

	fin.seekg(0, ios::end);
	size_t fileSize = fin.tellg();
	fin.seekg(0);
	string source;
	source.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	fin.close();

	vector<vec3> VertexAttribTemp[texCoord + 1];
	vector<GLuint> ElementArrayTemp;
	set<Vertex> VertexBuffer;
	set<Edge> EdgeTemp;

	stringstream objStream(source);
	while (!objStream.eof())
	{
		string line;
		getline(objStream, line);
		stringstream lineStream(line);
		char numb;
		if (line.empty())
			continue;
		switch (line.at(0))
		{
		case 'v':
			switch (line.at(1))
			{
			case ' ':
				float CoordData[3];
				lineStream >> numb >> CoordData[0] >> CoordData[1] >> CoordData[2];
				VertexAttribTemp[Coord].push_back(vec3(CoordData[0], CoordData[1], CoordData[2]));
				break;
			case 'n':
				float normalData[3];
				lineStream >> numb >> numb >> normalData[0] >> normalData[1] >> normalData[2];
				VertexAttribTemp[normalDir].push_back(vec3(normalData[0], normalData[1], normalData[2]));
				break;
			case 't':
				float texCoordData[3];
				char numb;
				lineStream >> numb >> numb >> texCoordData[0] >> texCoordData[1] >> texCoordData[2];
				VertexAttribTemp[texCoord].push_back(vec3(texCoordData[0], texCoordData[1], texCoordData[2]));
				break;
			default:
				break;
			}
			break;
		case 'f':
			lineStream >> numb;
			if (line.find("//") != line.npos)
			{
				for (int i = 0; i != 3; ++i)
				{
					unsigned PosTexNor[3];
					lineStream >> PosTexNor[Coord] >> numb >> numb >> PosTexNor[normalDir];
					for (int i = 0; i != 3; ++i)
					{
						PosTexNor[i]--;
					}
					ElementArrayTemp.push_back(PosTexNor[0]);
					VertexBuffer.insert(Vertex(VertexAttribTemp[Coord].at(PosTexNor[Coord]), VertexAttribTemp[normalDir].at(PosTexNor[normalDir]), vec3(0, 0, 0), PosTexNor[0]));
				}
			}
			else
			{
				for (int i = 0; i != 3; ++i)
				{
					unsigned PosTexNor[3];
					lineStream >> PosTexNor[Coord] >> numb >> PosTexNor[texCoord] >> numb >> PosTexNor[normalDir];
					for (int i = 0; i != 3; ++i)
					{
						PosTexNor[i]--;
					}
					ElementArrayTemp.push_back(PosTexNor[0]);
					VertexBuffer.insert(Vertex(VertexAttribTemp[Coord].at(PosTexNor[Coord]), VertexAttribTemp[normalDir].at(PosTexNor[normalDir]), VertexAttribTemp[texCoord].at(PosTexNor[texCoord]), PosTexNor[0]));
				}
			}
			break;
		default:
			break;
		}
	}

	for (int i = 0; i != 3; ++i)
	{
		VertexAttribTemp[i].clear();
	}

	VertexSum = VertexBuffer.size();
	IndexSum = ElementArrayTemp.size();

	GLfloat* VertexlocalBuffer = new GLfloat[4 * VertexSum];
	unsigned counter = 0;
	for (auto a : VertexBuffer)
	{
		VertexlocalBuffer[4 * counter] = a.vertexCoord[0] / this->Scale[scalex];
		VertexlocalBuffer[4 * counter + 1] = a.vertexCoord[1] / this->Scale[scaley];
		VertexlocalBuffer[4 * counter + 2] = a.vertexCoord[2] / this->Scale[scalez];
		VertexlocalBuffer[4 * counter + 3] = 1.0f;
		++counter;
	}
	string appendix = ".bin";
	fstream fout(this->Dir + appendix, ios::out|ios::binary);
	fout << '#' << VertexSum << ' ' << IndexSum << '\n';
	fout.write((char*)VertexlocalBuffer, 4 * sizeof(GLfloat)*VertexSum);
	delete[] VertexlocalBuffer;
	
	VertexlocalBuffer = new GLfloat[3 * VertexSum];
	counter = 0;
	for (auto a : VertexBuffer)
	{
		VertexlocalBuffer[3 * counter] = a.normalDir[0];
		VertexlocalBuffer[3 * counter + 1] = a.normalDir[1];
		VertexlocalBuffer[3 * counter + 2] = a.normalDir[2];
		++counter;
	}
	fout.write((char*)VertexlocalBuffer, 3 * sizeof(GLfloat)*VertexSum);
	delete[] VertexlocalBuffer;
	
	VertexlocalBuffer = new GLfloat[3 * VertexSum];
	counter = 0;
	for (auto a : VertexBuffer)
	{
		VertexlocalBuffer[3 * counter] = a.texCoord[0] / this->Scale[scaleuv];
		VertexlocalBuffer[3 * counter + 1] = a.texCoord[1] / this->Scale[scaleuv];
		VertexlocalBuffer[3 * counter + 2] = a.texCoord[2] / this->Scale[scaleuv];
		++counter;
	}
	fout.write((char*)VertexlocalBuffer, 3 * sizeof(GLfloat)*VertexSum);
	delete[] VertexlocalBuffer;

	counter = 0;
	GLuint* localIDBuffer = new GLuint[VertexSum];
	for (auto a : VertexBuffer)
	{
		localIDBuffer[counter] = a.ID;
		++counter;
	}
	fout.write((char*)localIDBuffer, sizeof(GLuint)*VertexSum);
	delete[] localIDBuffer;

	VertexBuffer.clear();

	GLuint* localIndicesBuffer = new GLuint[ElementArrayTemp.size()];
	counter = 0;
	for (auto a : ElementArrayTemp)
	{
		localIndicesBuffer[counter] = a;
		++counter;
	}
	fout.write((char*)localIndicesBuffer, ElementArrayTemp.size()*sizeof(GLuint));
	cout << this->Dir << ": " << "Faces in triangles: " << IndexSum / 3 << endl;
	delete[] localIndicesBuffer;

	ElementArrayTemp.clear();
}

void Mesh_Obj::Attach()
{
	string binaryDir = this->Dir + ".bin";
	fstream fin(binaryDir, ios::in | ios::binary);
	if (!fin.is_open())
	{
		fin.close();
		cout << binaryDir << " :Binary Not Found!" << endl;
		RawAttach();
		return;
	}
	else
		cout << binaryDir << " :Binary Loading Now!" << endl;

	if (this->is_Attached)
		return;
	else
		this->is_Attached = true;

	glGenVertexArrays(1, VAO);
	glGenBuffers(4, VBO);
	glBindVertexArray(VAO[0]);

	string headLine;
	getline(fin, headLine);
	stringstream headInfo(headLine);
	char numbHashTag;
	headInfo >> numbHashTag >> this->VertexSum >> this->IndexSum;

	GLfloat* Buffer = new GLfloat[4*VertexSum];
	fin.read((char*)Buffer, 4 * sizeof(GLfloat)*VertexSum);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[Coord]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat)*VertexSum, Buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	delete[] Buffer;

	Buffer = new GLfloat[3 * VertexSum];
	fin.read((char*)Buffer, 3 * sizeof(GLfloat)*VertexSum);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[normalDir]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat)*VertexSum, Buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	delete[] Buffer;

	Buffer = new GLfloat[3 * VertexSum];
	fin.read((char*)Buffer, 3 * sizeof(GLfloat)*VertexSum);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[texCoord]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat)*VertexSum, Buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);
	delete[] Buffer;

	GLuint* Buffer2 = new GLuint[VertexSum];
	fin.read((char*)Buffer2, VertexSum*sizeof(GLuint));
	glBindBuffer(GL_ARRAY_BUFFER, VBO[vertexID]);
	glBufferData(GL_ARRAY_BUFFER, VertexSum*sizeof(GLuint), Buffer2, GL_STATIC_DRAW);
	glVertexAttribIPointer(3, 1, GL_INT, 0, nullptr);
	glEnableVertexAttribArray(3);
	delete[] Buffer2;

	Buffer2 = new GLuint[IndexSum];
	fin.read((char*)Buffer2, IndexSum*sizeof(GLuint));
	fin.close();
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexSum*sizeof(GLuint), Buffer2, GL_STATIC_DRAW);
	cout << this->Dir << ": " << "Faces in triangles: " << this->IndexSum / 3 << endl;
	delete[] Buffer2;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh_Obj::Calc_Tangent()
{
	if (!this->is_Attached)
	{
		cout << "Not attached yet, Tangent calculation abandoned!" << endl;
		return;
	}

	fstream fin(this->Dir + ".nor", ios::in | ios::binary);
	if (fin.is_open())
	{
		GLfloat* LocalBuffer = new GLfloat[3 * this->VertexSum];
		fin.read(reinterpret_cast<char*>(LocalBuffer), 3 * sizeof(GLfloat)*this->VertexSum);
		glBindVertexArray(this->VAO[0]);

		glGenBuffers(1, &this->TangentBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->TangentBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3 * VertexSum, LocalBuffer, GL_STATIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
	}

	fin.close();
	fstream fout(this->Dir + ".nor", ios::out | ios::binary);

	glBindVertexArray(this->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[VertexAttrib::Coord]);
	GLfloat* CoordAccess = reinterpret_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[VertexAttrib::texCoord]);
	GLfloat* UVAccess = reinterpret_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	GLuint* IndiceAccess = reinterpret_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY));
	
	GLfloat* LocalBuffer = new GLfloat[3 * this->VertexSum];
	for (int i = 0; i < this->IndexSum; i += 3)
	{
		GLuint TriangleIndex[3];
		for (int j = 0; j != 3; ++j)
		{
			TriangleIndex[j] = IndiceAccess[i + j];
		}
		GLfloat TriangleCoord[3][3];
		for (int j = 0; j != 3; ++j)
		{
			for (int k = 0; k != 3; ++k)
			{
				TriangleCoord[j][k] = CoordAccess[TriangleIndex[j] * 4 + k];
			}
		}
		GLfloat UVCoord[3][2];
		for (int j = 0; j != 3; ++j)
		{
			UVCoord[j][0] = UVAccess[TriangleIndex[j] * 3];
			UVCoord[j][1] = UVAccess[TriangleIndex[j] * 3 + 1];
		}
		GLfloat deltaU1 = UVCoord[1][0] - UVCoord[0][0];
		GLfloat deltaU2 = UVCoord[2][0] - UVCoord[0][0];
		GLfloat deltaV1 = UVCoord[1][1] - UVCoord[0][1];
		GLfloat deltaV2 = UVCoord[2][1] - UVCoord[0][1];
		GLfloat factor = static_cast<double>(1) / (deltaU1*deltaV2 - deltaU2*deltaV1);
		for (int j = 0; j != 3; ++j)
		{
			for (int k = 0; k != 3; ++k)
			{
				LocalBuffer[TriangleIndex[j] + k] = factor*(deltaV2*(TriangleCoord[1][k] - TriangleCoord[0][k])
					- deltaV1*(TriangleCoord[2][k] - TriangleCoord[0][k]));
			}
		}
	}
	glGenBuffers(1, &this->TangentBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->TangentBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3 * VertexSum, LocalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);

	fout.write(reinterpret_cast<char*>(LocalBuffer), 3 * sizeof(GLfloat)*this->VertexSum);
	fout.close();
	delete[] LocalBuffer;

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[VertexAttrib::Coord]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[VertexAttrib::texCoord]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}