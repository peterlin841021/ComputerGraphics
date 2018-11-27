#include "obj.h"

Obj::Obj()
{

}
void Obj::Begin()
{	
	shaderProgram->bind();
	vao.bind();
}

void Obj::End()
{	
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	vao.release();
	shaderProgram->release();
}

void Obj::DimensionTransformation(GLfloat source[],GLfloat target[][4])
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
void Obj::Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector<GLfloat> values, std::vector<int> buffersize, float alpha, float time, int linetype, int colorMode, int effect)
{	
	GLfloat P[4][4];
	GLfloat MV[4][4];	
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);	
	shaderProgram->setUniformValue("ProjectionMatrix", P);	
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	shaderProgram->setUniformValue("effect", effect);
	shaderProgram->setUniformValue("colormode", colorMode);
	shaderProgram->setUniformValue("alpha", alpha);
	shaderProgram->setUniformValue("time", time);
	QVector<GLfloat> positions,colors,texturecoords,indexes;

	for (size_t i = 0; i < buffersize[0] + buffersize[1]; i++)
	{
		if(i < buffersize[0])
			positions << values[i];
		else
			positions << 0;
	}
	for (size_t i = buffersize[0]; i < buffersize[0] + buffersize[1]; i++)
	{
		colors << values[i];		
	}
	
	vvbo.bind();
	vvbo.allocate(positions.constData(), (buffersize[0] + buffersize[1]) * sizeof(GLfloat));
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);	
	shaderProgram->enableAttributeArray(0);
	if (colorMode == 0)
	{
		vvbo.write(buffersize[0] * sizeof(GLfloat), colors.constData(), buffersize[1] * sizeof(GLfloat));
		shaderProgram->setAttributeBuffer(2, GL_FLOAT, buffersize[0] * sizeof(GLfloat),3, 0);
		shaderProgram->enableAttributeArray(2);
	}
	else if(colorMode == 1 || colorMode == 2)
	{
		vvbo.write(buffersize[0] * sizeof(GLfloat), colors.constData(), buffersize[1] * sizeof(GLfloat));
		shaderProgram->setAttributeBuffer(1, GL_FLOAT, buffersize[0] * sizeof(GLfloat), 2, 0);
		shaderProgram->enableAttributeArray(1);
	}
	switch (linetype)
	{
	case 0:
		glDrawArrays(GL_LINES, 0, positions.size()/2);
		break;
	case 1:
		glDrawArrays(GL_TRIANGLES, 0, positions.size() / 3);
		break;
	default:
		break;
	}
	
	vvbo.release();	
}

void Obj::Init(int buffers)
{
	InitShader("./src/BSGC/prj3/Shader/vs.glsl", "./src/BSGC/prj3/Shader/fs.glsl");
	
	vao.create();
	vao.bind();
	switch (buffers)
	{
	case 2:
		vvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		vvbo.create();
		vvbo.bind();
		vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

		/*uvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		uvbo.create();
		uvbo.bind();
		uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

		cvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		cvbo.create();
		cvbo.bind();
		cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);*/
		break;
	case 4:
		vvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		vvbo.create();
		vvbo.bind();
		vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

		uvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		uvbo.create();
		uvbo.bind();
		uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

		cvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		cvbo.create();
		cvbo.bind();
		cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

		fbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		fbo.create();
		fbo.bind();
		fbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

		indexbo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		indexbo.create();
		indexbo.bind();
		indexbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
		break;
	}	
}
void Obj::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
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