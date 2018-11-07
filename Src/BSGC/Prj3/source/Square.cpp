#include "Square.h"

Square::Square()
{
}
void Square::DimensionTransformation(GLfloat source[],GLfloat target[][4])
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
void Square::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}
void Square::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,std::vector<float> vertices)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix,P);
	DimensionTransformation(ModelViewMatrix,MV);		
	shaderProgram->setUniformValue("ProjectionMatrix",P);	
	shaderProgram->setUniformValue("ModelViewMatrix",MV);		
	vvbo.bind();	
	//vertices << QVector3D(-w,h,1)<< QVector3D(w, h, 1)<<QVector3D(w, 0, 0)<<QVector3D(-w, 0, 0);	
	vvbo.allocate(vertices.data(), vertices.size() * sizeof(float));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	shaderProgram->enableAttributeArray(0);
	vvbo.release();
	uvbo.bind();
	
	uvs
		<< QVector2D(0.0f, 0.0f)
		<< QVector2D(1.0f, 0.f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(0.0f, 1.0f);
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
			
	
	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeArray(1,GL_FLOAT,0,2,NULL);
	uvbo.release();
	glDrawArrays(GL_TRIANGLE_FAN,0, vertices.size()/3);
	vertices.clear();
	uvs.clear();
}
void Square::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	vao.release();
	shaderProgram->release();
}

void Square::Init()
{
	InitShader("./src/BSGC/prj3/Shader/Square.vs","./src/BSGC/prj3/Shader/Square.fs");
	InitVAO();
	InitVBO();
	//InitTexture();
}
void Square::InitVAO()
{
	 // Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Square::InitVBO()
{
	
	vvbo.create();	
	vvbo.bind();	
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

	uvbo.create();	
	uvbo.bind();	
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);	
}
void Square::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
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