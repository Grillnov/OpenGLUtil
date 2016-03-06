#ifndef PHONG
#define PHONG
#include "GLApplication.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "TrackBallCamera.h"
#include <ObjUtils.h>
class Asset
{
public:
	static const char* GetAssetDir(const char* name)
	{
		static std::string oldAsset;
		oldAsset = "D:\\OpenGLUtil\\ModelView";
		oldAsset.append("\\assets\\");
		oldAsset.append(name);
		return oldAsset.c_str();
	}
};

class Phong :public GLApplication
{
public:
	virtual void MouseScroll(double x, double y)
	{
		if (y > 0)
			camera->Zoom(0.95);
		else
			camera->Zoom(1.05);
	}
	virtual void CreateApplication()
	{
		glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		this->plain = Mesh_Obj(Asset::GetAssetDir("BasePlain.obj"), 20.0f, 20.0f, 20.0f);
		plain.Attach();
		plain.Calc_Tangent();

		camera = new TrackBallCamera(this, 5.0f, 0.1f, 1000.0f, 45.0f);

		this->tex = GLTexture::LoadFromFile(Asset::GetAssetDir("wall.jpg"));
		this->tex->Attach();
		this->tex->GenerateMipmap();

		this->normaltex = GLTexture::LoadFromFile(Asset::GetAssetDir("wallnormal.jpg"));
		this->normaltex->Attach();
		this->normaltex->GenerateMipmap();

		this->shader = new GLShader(Asset::GetAssetDir("phong_normalMap.glsl"), 
			"transformMatrix\ntex\ntexNorm\nModelViewMatrix\n"
			"lightPosition\ndiffuseColor\nlightColor\nambientColor");
		this->shader->Attach();
	}
	virtual void RenderFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, GetWindowWidth(), GetWindowHeight());

		camera->OnFrameUpdate();
		glm::mat4 MVP = camera->GetProjectionViewMatrix();
		glm::mat4 MV = camera->GetModelViewMatrix();
		shader->UseProgram();

		shader->UniformMatrix4fv("transformMatrix", 1, GL_FALSE, &MVP[0][0]);
		shader->UniformMatrix4fv("ModelViewMatrix", 1, GL_FALSE, &MV[0][0]);

		tex->ActivateAndBindTexture(GL_TEXTURE0);
		shader->Uniform1i("tex", 0);
		normaltex->ActivateAndBindTexture(GL_TEXTURE1);
		shader->Uniform1i("texNorm", 1);

		GLfloat pfLightPos[3] = { 0.0f, 0.0f, 3.0f };
		shader->Uniform3fv("lightPosition", 1, pfLightPos);
		GLfloat pfSpecularColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		shader->Uniform4fv("lightColor", 1, pfSpecularColor);
		GLfloat pfAmbientColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		shader->Uniform4fv("ambientColor", 1, pfAmbientColor);
		GLfloat pfDiffuseColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		shader->Uniform4fv("diffuseColor", 1, pfDiffuseColor);

		//cube.DrawTriangles();
		plain.DrawTriangles();
	}
	virtual void ShutdownApplication()
	{
		//this->cube.Detach();
		this->shader->Detach();
		this->tex->Detach();
	}
private:
	Mesh_Obj plain;
	GLTexture *tex, *normaltex;
	TrackBallCamera *camera;
	GLShader *shader;
};
#endif