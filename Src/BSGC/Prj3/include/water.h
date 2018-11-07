
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





class Water
{
public:
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	
	QVector<QVector3D> colors;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;	
public:
	Water();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,QVector<GLfloat> vts);
	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();	
	void End();	
};