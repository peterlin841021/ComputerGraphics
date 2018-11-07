#include "water.h"

Water::Water()
{
}
void Water::Begin()
{	
	shaderProgram->bind();
	vao.bind();
}

void Water::End()
{	
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	vao.release();
	shaderProgram->release();
}

void Water::DimensionTransformation(GLfloat source[],GLfloat target[][4])
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
void Water::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector<GLfloat> vts)
{	
	GLfloat P[4][4];
	GLfloat MV[4][4];	
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);	
	shaderProgram->setUniformValue("ProjectionMatrix", P);	
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	QVector<GLfloat> pos, uv;
	int pos_count = 0, uv_count = 0;
	int size = vts.size()/4200;
	if (size == 0) {
		size = 18;
		for (size_t i = 0; i < size; i++)
		{
			pos << vts[i];
			pos_count++;
		}
		uv_count = size/3*2;
		uv 
			<< 0.f << 0.f
			<< 1.f << 0.f
			<< 1.f << 1.f
			<< 1.f << 1.f
			<< 0.f << 1.f
			<< 0.f << 0.f
			;
	}
	else {
		for (size_t i = 0; i < size * size * 18; i++)
		{
			pos << vts[i];
			pos_count++;
		}
		for (size_t i = size * size * 18; i < vts.size(); i++)
		{
			uv << vts[i];
			uv_count++;
		}
	}
	
	
	vvbo.bind();
	vvbo.allocate(vts.data(), (pos_count+ uv_count) *sizeof(GLfloat));
		
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);	
	shaderProgram->enableAttributeArray(0);
	
	vvbo.write(pos_count * sizeof(GLfloat), uv.data(), uv_count * sizeof(GLfloat));
	shaderProgram->setAttributeBuffer(1, GL_FLOAT, pos_count * sizeof(GLfloat), 2, 0);
	shaderProgram->enableAttributeArray(1);
		
	glDrawArrays(GL_TRIANGLES, 0, pos_count /3);
	vvbo.release();
	pos.clear();
	uv.clear();
}
void Water::Init()
{
	InitShader("./src/BSGC/prj3/Shader/water.vs", "./src/BSGC/prj3/Shader/water.fs");
	InitVAO();
	InitVBO();
}
void Water::InitVAO()
{	 
	vao.create();
	vao.bind();
	
}
void Water::InitVBO()
{	
	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	/*uvbo.create();
	uvbo.bind();
	uvbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);*/
}
void Water::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
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