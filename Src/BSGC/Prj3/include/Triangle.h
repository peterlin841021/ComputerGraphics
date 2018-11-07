#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtCore/QVector>
#include <QtGui/QVector3D>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <vector>
class Triangle 
{
public:	
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector3D> colors;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;	
	QOpenGLBuffer indexbuffer;
public:
	Triangle();
	void Init();
	void InitVAO();
	void InitVBO();	
	void InitShader(QString vertexShaderPath,QString fragmentShaderPath);
	void Start();
	void End();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,std::vector<GLfloat> v, std::vector<GLuint> f);
	void DimensionTransformation(GLfloat source[],GLfloat target[][4]);
};