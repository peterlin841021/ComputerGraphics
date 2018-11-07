#include "Triangle.h"
Triangle::Triangle()
{
}
void Triangle::DimensionTransformation(GLfloat source[],GLfloat target[][4])
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
void Triangle::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, std::vector<GLfloat> v, std::vector<GLuint> f)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	QVector<QVector3D> vertices,vs;
	for (size_t i = 0; i < v.size(); i+=3) {
		vertices << QVector3D(v[i], v[i+1], v[i+2]);
	}
	/*for (size_t i = 0; i < f.size(); i++) {
		vs << vertices[f[i]];
	}*/
	shaderProgram->bind();
	vao.bind();
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	vvbo.bind();
	//vvbo.allocate(vs.data(), (vs.size()) * sizeof(QVector3D));
	vvbo.allocate(vertices.data(), (vertices.size()) * sizeof(QVector3D));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0,3,0);
	shaderProgram->enableAttributeArray(0);
	vvbo.release();		
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, vs.size());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
 	//glDrawElements(GL_TRIANGLE_STRIP, f.size(), GL_UNSIGNED_INT,0);
	shaderProgram->disableAttributeArray(0);
}
void Triangle::Init()
{
	InitShader("./src/BSGC/prj3/Shader/Triangle.vs","./src/BSGC/prj3/Shader/Triangle.fs");
	InitVAO();
	InitVBO();
}
void Triangle::Start() {
	
}
void Triangle::End() {
	vao.release();
	shaderProgram->release();
}
void Triangle::InitVAO()
{
	vao.create();	
	vao.bind();
}
void Triangle::InitVBO()
{		
	vvbo.create();
	//vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	indexbuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	indexbuffer.create();
	indexbuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
}
void Triangle::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
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