#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL/QtOpenGL>
#include <QtCore/QVector>
#include <QtGui/QVector3D>
#include <QtGui/QVector2D>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QImage>
class Cube {
public:
	QOpenGLShader *vs;
	QOpenGLShader *fs;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vertices_vbo;
	QOpenGLBuffer uv_vbo;
	QOpenGLShaderProgram * shaderProgram;
public:
	Cube();	
	void Init();
	void StartDraw();
	void EndDraw();
	void Paint(QVector<GLfloat> vertices, GLfloat * pm, GLfloat * mm,int size);
};
