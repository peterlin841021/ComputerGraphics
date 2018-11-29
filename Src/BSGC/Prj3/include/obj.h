
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

class Obj
{
public:
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	QOpenGLBuffer cvbo;
	QOpenGLBuffer fbo;
	QOpenGLBuffer indexbo;
	GLuint textureId;
public:
	Obj();
	void Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,QVector<GLfloat> values,std::vector<int> buffersize,float alpha,float time,int linetype, int colorMode,int effect);
	void Init(int buffers);	
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();	
	void End();	
};