#ifndef BASEMODEL_H
#define BASEMODEL_H
#include<glew.h>
#include<glut.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include"shader.h"
using namespace glm;
typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} TextureData;
class BaseModel{

	//uniform id
	struct
	{
		GLint  mv_matrix;
		GLint  proj_matrix;
	} uniforms;

	//model info
	typedef struct
	{
		GLuint vao;
		GLuint vbo;
		GLuint vboTex;
		GLuint ebo;

		GLuint p_normal;
		int materialId;
		int indexCount;
		GLuint m_texture;

		glm::mat4 model;
	} Shape;


public:
	BaseModel();
	BaseModel(char* modelFile,char* textFile);
	~BaseModel();
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();

	void Translate(glm::vec3 vec);
	void Rotate(glm::vec3 axis, float angle);
	void Scale(glm::vec3 vec);

private:
	char* objName;
	char* textName;
	ShaderProgram* program;
	Shape m_shape;
	virtual void LoadModel();
	glm::mat4 translateMatrix;
	glm::mat4 rotateMatrix;
	glm::mat4 scaleMatrix;
};

#endif  // BASEMODEL_H
