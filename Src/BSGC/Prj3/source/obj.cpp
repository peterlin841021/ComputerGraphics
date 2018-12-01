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
	if (buffersize.size() == 2)
	{
		for (size_t i = buffersize[0]; i < buffersize[0] + buffersize[1]; i++)
		{
			colors << values[i];
		}
	}
		
	vbo.bind();
	vbo.allocate(positions.constData(), (buffersize[0] + buffersize[1]) * sizeof(GLfloat));
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);	
	shaderProgram->enableAttributeArray(0);
	if (colorMode == 0)
	{
		vbo.write(buffersize[0] * sizeof(GLfloat), colors.constData(), buffersize[1] * sizeof(GLfloat));
		shaderProgram->setAttributeBuffer(2, GL_FLOAT, buffersize[0] * sizeof(GLfloat),3, 0);
		shaderProgram->enableAttributeArray(2);
	}
	else if(colorMode == 1)
	{
		if (buffersize[1] != 0)
		{
			vbo.write(buffersize[0] * sizeof(GLfloat), colors.constData(), buffersize[1] * sizeof(GLfloat));
			shaderProgram->setAttributeBuffer(1, GL_FLOAT, buffersize[0] * sizeof(GLfloat), 2, 0);
			shaderProgram->enableAttributeArray(1);
		}		
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
	vbo.release();
}
void Obj::Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector<GLfloat> values, std::vector<int> buffersize, float alpha, float time, int effect)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	shaderProgram->setUniformValue("effect", effect);	
	shaderProgram->setUniformValue("alpha", alpha);
	shaderProgram->setUniformValue("time", time);	
	QVector<GLfloat> positions, colors, texturecoords, indexes;

	for (size_t i = 0; i < buffersize[0] + buffersize[1]; i++)
	{
		if (i < buffersize[0])
			positions << values[i];
		else
			positions << 0;
	}
	for (size_t i = buffersize[0]; i < buffersize[0] + buffersize[1]; i++)
	{
		colors << values[i];
	}

	vbo.bind();
	vbo.allocate(positions.constData(), (buffersize[0] + buffersize[1]) * sizeof(GLfloat));
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);
	shaderProgram->enableAttributeArray(0);
	if (buffersize[1] != 0)
	{
		vbo.write(buffersize[0] * sizeof(GLfloat), colors.constData(), buffersize[1] * sizeof(GLfloat));
		shaderProgram->setAttributeBuffer(1, GL_FLOAT, buffersize[0] * sizeof(GLfloat), 2, 0);
		shaderProgram->enableAttributeArray(1);
	}
	shaderProgram->setPatchVertexCount(4);
	glDrawArrays(GL_PATCHES, 0, 4);
	vbo.release();
}
void Obj::Init(int shaders)
{
	InitShader(shaders);
	
	vao.create();
	vao.bind();
	vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
}
void Obj::InitShader(size_t shaders)
{
	// Create Shader	
	QString vertexShaderPath = "./src/BSGC/prj3/Shader/vs.glsl";
	QString fragmentShaderPath = "./src/BSGC/prj3/Shader/fs.glsl";
	QString tessellationControlShaderPath = "./src/BSGC/prj3/Shader/tcs.glsl";
	QString tessellationEvalutionShaderPath = "./src/BSGC/prj3/Shader/tes.glsl";
	QString geometryShaderPath = "./src/BSGC/prj3/Shader/gs.glsl";
	shaderProgram = new QOpenGLShaderProgram();

	if (shaders >= 2)
	{
		QFileInfo  vertexShaderFile(vertexShaderPath);
		if (vertexShaderFile.exists())
		{
			vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
			if (vertexShader->compileSourceFile(vertexShaderPath))
				shaderProgram->addShader(vertexShader);
			else
				qWarning() << "Vertex Shader Error " << vertexShader->log();
		}
		else
			qDebug() << vertexShaderFile.filePath() << " can't be found";

		QFileInfo  fragmentShaderFile(fragmentShaderPath);
		if (fragmentShaderFile.exists())
		{
			fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
			if (fragmentShader->compileSourceFile(fragmentShaderPath))
				shaderProgram->addShader(fragmentShader);
			else
				qWarning() << "fragment Shader Error " << fragmentShader->log();
		}
		else
			qDebug() << fragmentShaderFile.filePath() << " can't be found";
	}
	if (shaders > 2)
	{
		QFileInfo  tessellationControlShaderFile(tessellationControlShaderPath);
		if (tessellationControlShaderFile.exists())
		{
			tessellationControlShader = new QOpenGLShader(QOpenGLShader::TessellationControl);
			if (tessellationControlShader->compileSourceFile(tessellationControlShaderPath))
				shaderProgram->addShader(tessellationControlShader);
			else
				qWarning() << "Tessellation control shader error " << tessellationControlShader->log();
		}
		else
			qDebug() << tessellationControlShaderFile.filePath() << " can't be found";

		QFileInfo  tessellationEvalutionShaderFile(tessellationEvalutionShaderPath);
		if (tessellationEvalutionShaderFile.exists())
		{
			tessellationEvalutionShader = new QOpenGLShader(QOpenGLShader::TessellationEvaluation);
			if (tessellationEvalutionShader->compileSourceFile(tessellationEvalutionShaderPath))
				shaderProgram->addShader(tessellationEvalutionShader);
			else
				qWarning() << "Tessellation evalution shader error " << tessellationEvalutionShader->log();
		}
		else
			qDebug() << tessellationEvalutionShaderFile.filePath() << " can't be found";

		/*QFileInfo  geometryShaderFile(geometryShaderPath);
		if (geometryShaderFile.exists())
		{
			geometryShader = new QOpenGLShader(QOpenGLShader::Geometry);
			if (geometryShader->compileSourceFile(geometryShaderPath))
				shaderProgram->addShader(geometryShader);
			else
				qWarning() << "Geometry shader error " << geometryShader->log();
		}
		else
			qDebug() << geometryShaderFile.filePath() << " can't be found";*/
	}			
	shaderProgram->link();
}