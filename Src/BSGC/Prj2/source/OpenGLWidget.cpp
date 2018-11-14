#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include <gl\gl.h>
#include <gl\GLU.h>
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/qopengltexture.h>

QOpenGLShaderProgram* shaderProgram;
QOpenGLShader* vertexShader;
QOpenGLShader* fragmentShader;
QOpenGLVertexArrayObject vao;
QOpenGLBuffer vvbo;
QOpenGLBuffer cvbo;
QOpenGLBuffer uvbo;
//QVector<QVector3D> vts;
//QVector<QVector2D> uvs;
QVector<QOpenGLTexture*> textures;
void shader_init()
{
	shaderProgram = new QOpenGLShaderProgram();
	vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);

	vertexShader->compileSourceFile("./src/bsgc/prj2/assets/vertex.glsl");
	fragmentShader->compileSourceFile("./src/bsgc/prj2/assets/fragment.glsl");
	shaderProgram->addShader(vertexShader);
	shaderProgram->addShader(fragmentShader);

	vao.create();
	vao.bind();

	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	cvbo.create();
	cvbo.bind();
	cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	uvbo.create();
	uvbo.bind();
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	shaderProgram->link();
}
void drawTextures(GLint textureid, QVector<QVector3D> vts)
{	
	shaderProgram->setUniformValue("Texture", textureid);
	shaderProgram->setUniformValue("mode",1);
	float w = 1, h = 1;
	shaderProgram->bind();
	vao.bind();
	QVector<QVector2D> uvs;
	vvbo.bind();	

	vvbo.allocate(vts.constData(), vts.size() * sizeof(QVector3D));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();
	shaderProgram->enableAttributeArray(0);

	uvbo.bind();
	uvs
		<< QVector2D(0.0f, 0.0f)
		<< QVector2D(1.0f, 0.f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(0.0f, 1.0f)
		<< QVector2D(0.0f, 0.0f);
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
	shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 2, NULL);
	uvbo.release();
	shaderProgram->enableAttributeArray(2);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, vts.size());	
	glDrawArrays(GL_TRIANGLES, 0, vts.size());
}
void drawMap(QVector<QVector3D> vts, QVector<QVector3D> colors)
{
	shaderProgram->setUniformValue("mode", 1);	
	shaderProgram->bind();
	vao.bind();
	vvbo.bind();

	vvbo.allocate(vts.constData(), vts.size() * sizeof(QVector3D));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();
	shaderProgram->enableAttributeArray(0);

	cvbo.bind();	
	cvbo.allocate(colors.constData(), colors.size() * sizeof(QVector3D));
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	cvbo.release();
	shaderProgram->enableAttributeArray(1);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, vts.size());	
	glDrawArrays(GL_LINES, 0, vts.size());
}
OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{	
	top_z = 1.5f;
	but_z = -1;
	
	QDir dir("Pic");
	if(dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";	
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0,0,0,1);
	glEnable(GL_TEXTURE_2D);
	shader_init();
	textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj2/assets/sky.png")));
	textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj2/assets/brick.png")));
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(MazeWidget::maze!=NULL)
	{
		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0 , 0 , MazeWidget::w/2 , MazeWidget::h);		
		float maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		glOrtho(-0.1, maxWH + 0.1, -0.1, maxWH + 0.1, 0, 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();

		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glViewport(MazeWidget::w/2,0, MazeWidget::w/2, MazeWidget::h);
		/*gluPerspective 定義透視
		//視野大小, nearplane, farplane, distance
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		//gluPerspective(MazeWidget::maze->viewer_fov, 1 , 0.01 , 200);

		/* gluLookAt
		//原本相機位置
		//看的方向
		//哪邊是上面
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

		/*gluLookAt(viewerPosX, viewerPosZ, viewerPosY,
			viewerPosX + cos(degree_change(MazeWidget::maze->viewer_dir)), viewerPosZ, viewerPosY + sin(degree_change(MazeWidget::maze->viewer_dir)),
			0.0, -1.0, 0.0);*/
		
		/*glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();*/
		glViewport(600, 0, MazeWidget::w / 2, MazeWidget::h);
		Map_3D();				
	}
}
void OpenGLWidget::resizeGL(int w,int h)
{
}

//Draw Left Part
void OpenGLWidget::Mini_Map()	
{					
	glBegin(GL_LINES);
		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

		for(int i = 0 ; i < (int)MazeWidget::maze->num_edges; i++)
		{
			float edgeStartX = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X];
			float edgeStartY = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y];
			float edgeEndX = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X];
			float edgeEndY = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y];

			glColor3f(MazeWidget::maze->edges[i]->color[0] , MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
			if(MazeWidget::maze->edges[i]->opaque)
			{
				glVertex2f(edgeStartX, edgeStartY);
				glVertex2f(edgeEndX, edgeEndY);
			}
		}
		//draw frustum		
		float maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		float len = 10.f * maxWH/10.f;		
		
		glColor3f(1, 1, 1);
		glVertex2f(viewerPosX, viewerPosY);
		glVertex2f(viewerPosX + maxWH / len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
			viewerPosY + maxWH / len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));

		glVertex2f(viewerPosX, viewerPosY);
		glVertex2f(viewerPosX + maxWH / len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
			viewerPosY + maxWH / len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
	glEnd();
}


//**********************************************************************
//
// * Draws the first-person view of the maze.
//   THIS IS THE FUINCTION YOU SHOULD MODIFY.
//
//Note: You must not use any openGL build-in function to set model matrix, view matrix and projection matrix.
//		ex: gluPerspective, gluLookAt, glTraslatef, glRotatef... etc.
//		Otherwise, You will get 0 !
//======================================================================
void OpenGLWidget::Map_3D()
{
	//glLoadIdentity();
	// 畫右邊區塊的所有東西	
	QVector<QVector3D> vts;
	vts 
		<< QVector3D(-1, 1, 0) << QVector3D(1, 1, 0) << QVector3D(1, 0, 0) 
		<< QVector3D(1, 0, 0) << QVector3D(-1, 0, 0) << QVector3D(-1, 1, 0);
	textures[0]->bind(1);//Draw miku
	drawTextures(1, vts);
	vts.clear();
	textures[1]->bind(2);//Draw bricks
	vts 
		<< QVector3D(-1, 0,0) << QVector3D(1, 0,0) << QVector3D(1, -1,0) 
		<< QVector3D(1, -1, 0) << QVector3D(-1, -1, 0)<< QVector3D(-1, 0, 0);
	drawTextures(2, vts);
	
	//drawTextures(1, vts);
	//glDisable(GL_TEXTURE_2D);
}
void OpenGLWidget::loadTexture2D(QString str,GLuint &textureID)
{
	/*glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	QImage img(str);
	QImage opengl_grass = QGLWidget::convertToGLFormat(img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);*/
}
float OpenGLWidget::degree_change(float num)
{
	return num /180.0f * 3.14159f;
}