#include "Cube.h"
Cube::Cube() {

}
void Cube::Init() {
	shaderProgram = new QOpenGLShaderProgram();
	QString filepath = "./src/BSGC/prj3/Shader/Cube.vs";
	QFileInfo vsfile(filepath);
	//vertex shader
	if (vsfile.exists()) {
		vs = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vs->compileSourceFile(filepath)) {
			shaderProgram->addShader(vs);
		}
		else {
			qDebug() << "vs error!";
		}
	}else{
		qDebug() << filepath <<"Not found!";
	}

	filepath = "./src/BSGC/prj3/Shader/Cube.fs";
	QFileInfo fsfile(filepath);
	//fragment shader
	if (fsfile.exists()) {
		fs = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fs->compileSourceFile(filepath)) {
			shaderProgram->addShader(fs);
		}
		else {
			qDebug() << "fs error!";
		}
	}
	else {
		qDebug() << filepath << "Not found!";
	}
	shaderProgram->link();

	vao.create();
	vao.bind();

	vertices_vbo.create();
	vertices_vbo.bind();
	vertices_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);		
}

void Cube::StartDraw() {
	shaderProgram->bind();
	vao.bind();
}
void Cube::Paint(QVector<GLfloat> vertices, GLfloat* pm, GLfloat* mm,int size) {
	GLfloat p[4][4];
	GLfloat m[4][4];
	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			p[i][j] = pm[i * 4 + j];
			m[i][j] = mm[i * 4 + j];
		}
	}
	m[0][3] = 0;
	m[1][3] = 0;
	m[2][3] = 0;
	shaderProgram->setUniformValue("ProjectionMatrix", p);
	shaderProgram->setUniformValue("ModelViewMatrix", m);
	vertices_vbo.bind();
	int vt_size = size;
	
	QVector<GLfloat> vt, uv;
	for (size_t i = 0; i < vt_size; i++)
	{
		vt << vertices[i];
	}
	int uv_size = size/3*2;
	float uvs[] = {
		0.f,0.f,
		1.f,0.f,
		1.f,1.f,
		1.f,1.f,
		0.f,1.f,
		0.f,0.f,
	};
	for (size_t i = 0; i < uv_size; i++)
	{
		uv << uvs[i%12];
	}
	vertices_vbo.allocate(vt.data(), (vt_size+ uv_size) * sizeof(GLfloat));
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);
	shaderProgram->enableAttributeArray(0);

	vertices_vbo.write(vt_size *sizeof(GLfloat), uv.data(), uv_size* sizeof(GLfloat));	
	shaderProgram->setAttributeBuffer(1, GL_FLOAT, vt_size*sizeof(GLfloat), 2,0);
	shaderProgram->enableAttributeArray(1);
	vertices_vbo.release();
	
	glDrawArrays(GL_TRIANGLES, 0, vt_size/3);
	
}
void Cube::EndDraw() {
	vao.release();
	shaderProgram->release();
}

