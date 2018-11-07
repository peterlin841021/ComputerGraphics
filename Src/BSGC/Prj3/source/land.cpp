#include "land.h"

Land::Land()
{
}
void Land::DimensionTransformation(GLfloat source[],GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for(int j=0;j<4;j++)
		for(int k=0;k<4;k++)
		{
			target[j][k] = source[i];
			i++;
		}
}
void Land::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}
void Land::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, float x, float z,float xsize, float zsize,float height)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix,P);
	DimensionTransformation(ModelViewMatrix,MV);	
	shaderProgram->setUniformValue("ProjectionMatrix",P);	
	shaderProgram->setUniformValue("ModelViewMatrix",MV);
	
	vvbo.bind();
	vertices
	<< QVector3D(x, height, z)
	<< QVector3D(x+xsize, height,z)
	<< QVector3D(x+xsize, height,z+zsize)
	<< QVector3D(x, height,z+zsize);
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));	
	shaderProgram->setAttributeArray(0,GL_FLOAT,0,3,NULL);	
	vvbo.release();
	shaderProgram->enableAttributeArray(0);

	uvbo.bind();
	uvs
		<< QVector2D(0.0f, 0.0f)
		<< QVector2D(1.0f, 0.f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(0.0f, 1.0f);
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
			
	shaderProgram->setAttributeArray(1,GL_FLOAT,0,2,NULL);
	uvbo.release();	
	shaderProgram->enableAttributeArray(1);
	glDrawArrays(GL_TRIANGLE_FAN,0,vertices.size());
	vertices.clear();
	uvs.clear();
}
void Land::End()
{	
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

void Land::Init()
{
	InitShader("./src/BSGC/prj3/Shader/land.vs","./src/BSGC/prj3/Shader/land.fs");
	InitVAO();
	InitVBO();
}
void Land::InitVAO()
{	
	vao.create();
	vao.bind();
}
void Land::InitVBO()
{									
	vvbo.create();	
	vvbo.bind();	
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	uvbo.create();	
	uvbo.bind();	
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
}
void Land::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if(vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if(vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug()<<vertexShaderFile.filePath()<<" can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if(fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if(fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug()<<fragmentShaderFile.filePath()<<" can't be found";
	shaderProgram->link();
}