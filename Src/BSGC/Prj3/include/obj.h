
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
	QOpenGLShader* tessellationControlShader;
	QOpenGLShader* tessellationEvalutionShader;
	QOpenGLShader* geometryShader;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo;
	GLuint textureId;
public:
	Obj();
	void Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,QVector<GLfloat> values,std::vector<int> buffersize,float alpha,float time,int linetype, int colorMode, int effect);
	void Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector<GLfloat> values, std::vector<int> buffersize, float alpha, float time,int effect);
	void Init(int shaders);	
	void InitShader(size_t shaders);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();	
	void End();	
};