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
#include"LineSeg.h"

QOpenGLShaderProgram* shaderProgram;
QOpenGLShader* vertexShader;
QOpenGLShader* fragmentShader;
QOpenGLVertexArrayObject vao;
QOpenGLBuffer vvbo;
QOpenGLBuffer cvbo;
QOpenGLBuffer uvbo;

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
void DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j < 4; j++)
		for (int k = 0; k < 4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
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
	glDrawArrays(GL_TRIANGLES, 0, vts.size());
}
void drawMap(QVector<QVector2D> vts, QVector<QVector3D> colors,float *mm,float *pm)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(mm, MV);
	DimensionTransformation(pm, P);
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	shaderProgram->setUniformValue("mode", 2);	
	shaderProgram->bind();
	vao.bind();
	vvbo.bind();

	vvbo.allocate(vts.constData(), vts.size() * sizeof(QVector2D));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 2, NULL);
	vvbo.release();
	shaderProgram->enableAttributeArray(0);

	cvbo.bind();	
	cvbo.allocate(colors.constData(), colors.size() * sizeof(QVector3D));
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	cvbo.release();
	shaderProgram->enableAttributeArray(1);		
	glDrawArrays(GL_LINES, 0, vts.size());
}
void drawFrustum(QVector<QVector2D> vts, QVector<QVector3D> colors, float *mm, float *pm)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(mm, MV);
	DimensionTransformation(pm, P);
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	shaderProgram->setUniformValue("mode", 2);
	shaderProgram->bind();
	vao.bind();
	vvbo.bind();

	vvbo.allocate(vts.constData(), vts.size() * sizeof(QVector2D));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 2, NULL);
	vvbo.release();
	shaderProgram->enableAttributeArray(0);

	cvbo.bind();
	cvbo.allocate(colors.constData(), colors.size() * sizeof(QVector3D));
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	cvbo.release();
	shaderProgram->enableAttributeArray(1);	
	glDrawArrays(GL_LINES, 0, vts.size());
}
void drawWall(QVector<QVector2D> vts, QVector<QVector3D> colors)
{	
	shaderProgram->setUniformValue("mode", 0);
	shaderProgram->bind();
	vao.bind();
	vvbo.bind();

	vvbo.allocate(vts.constData(), vts.size() * sizeof(QVector2D));
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 2, NULL);
	vvbo.release();
	shaderProgram->enableAttributeArray(0);

	cvbo.bind();
	cvbo.allocate(colors.constData(), colors.size() * sizeof(QVector3D));
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	cvbo.release();
	shaderProgram->enableAttributeArray(1);	
	glDrawArrays(GL_QUADS, 0, vts.size());
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
		glViewport(0, 0, MazeWidget::w/2 , MazeWidget::h);
		float maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		glOrtho(-0.1, maxWH + 0.1, -0.1, maxWH + 0.1, 0, 10);				
		Mini_Map();
		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Map_3D();				
	}
}
void OpenGLWidget::resizeGL(int w,int h)
{

}
void OpenGLWidget::loadTexture2D(QString str, GLuint &textureID)
{

}
float degree_change(float num)
{
	return num / 180.0f * 3.14159f;
}
void OpenGLWidget::Mini_Map()	
{
	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];	
	QVector<QVector2D> vts;
	QVector<QVector3D> colors;
	GLfloat mm[16], pm[16];
	glGetFloatv(GL_PROJECTION_MATRIX, mm);
	glGetFloatv(GL_MODELVIEW_MATRIX, pm);
	for (int i = 0; i < (int)MazeWidget::maze->num_edges; i++)
	{
		float edgeStartX = (MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X]);
		float edgeStartY = (MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y]);
		float edgeEndX = (MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X]);
		float edgeEndY = (MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y]);		
		colors			
			<< QVector3D(MazeWidget::maze->edges[i]->color[0], MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2])
			<< QVector3D(MazeWidget::maze->edges[i]->color[0], MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);			
		if (MazeWidget::maze->edges[i]->opaque)
		{			
			vts 
				<< QVector2D(edgeStartX, edgeStartY)
				<< QVector2D(edgeEndX, edgeEndY)			
				;
		}
	}
	drawMap(vts, colors,mm,pm);

	vts.clear();
	colors.clear();
	//Draw frustum
	float maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
	float len = 0.1f *10.f / maxWH;	
	colors
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, 1);	
	vts
		<< QVector2D(viewerPosX, viewerPosY)
		<< QVector2D(
			viewerPosX + maxWH * len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
			viewerPosY + maxWH * len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));
	vts
		<< QVector2D(viewerPosX, viewerPosY)
		<< QVector2D(viewerPosX + maxWH * len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
			viewerPosY + maxWH * len * sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
	drawFrustum(vts, colors,mm, pm);
}
void drawcells(Cell *current, QLineF rightfrustum, QLineF leftfrustum)
{
	for (size_t i = 0; i < 4; i++)
	{
		Edge *each = current->edges[i];
		bool draw = false;		
		QLineF edge_line = QLineF(each->endpoints[0]->posn[Maze::X], each->endpoints[0]->posn[Maze::Y], each->endpoints[1]->posn[Maze::X], each->endpoints[1]->posn[Maze::Y]);
		QPointF rip;
		QPointF lip;
		int intersect_l = leftfrustum.intersect(edge_line, &lip);
		int intersect_r = rightfrustum.intersect(edge_line, &rip);
		if (intersect_l == 1 && intersect_r == 1)
		{
			draw = true;
		}
		if (intersect_l != 1 && intersect_r != 1)
		{
			draw = true;
		}
		else if (intersect_l == 1 && intersect_r != 1 || intersect_l != 1 && intersect_r == 1)
		{
			if (intersect_l == 1)
			{
				draw = true;
			}
			if (intersect_r == 1)
			{
				draw = true;
			}
		}
		float model[16] = {
			rip.y(),1,rip.x(),1,
			rip.y(),-1,rip.x(),1,
			lip.y(),-1,lip.x(),1,
			lip.y(),1,lip.x(),1
		};
		QMatrix4x4 modelmatrix(model);
		QMatrix4x4 viewmatrix, temp;
		viewmatrix.setToIdentity();
		temp.setToIdentity();		
		viewmatrix.rotate(180, 1, 0, 0);
		viewmatrix.rotate(MazeWidget::maze->viewer_dir, 0, -1, 0);
		temp.translate(-MazeWidget::maze->viewer_posn[Maze::Y], 0, -MazeWidget::maze->viewer_posn[Maze::X]);
		viewmatrix *= temp;

		float screen[4][4] = { 0 };
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				for (size_t k = 0; k < 4; k++)
				{
					screen[i][j] += viewmatrix(j, k) * modelmatrix(i, k);
				}
			}
		}
		float maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		float wall_length = 1.f;
		float depth = maxWH / (5.f*2.f*tan(degree_change(MazeWidget::maze->viewer_fov*0.5f)));
		for (size_t i = 0; i < 4; i++)
		{
			screen[i][0] *= depth / screen[i][2];
			screen[i][1] *= depth / screen[i][2];
		}
		if (draw && each->opaque)
		{
			QVector<QVector2D> twoDvts;
			QVector<QVector3D> colors;			
			colors
				<< QVector3D(each->color[0], each->color[1], each->color[2])
				<< QVector3D(each->color[0], each->color[1], each->color[2])
				<< QVector3D(each->color[0], each->color[1], each->color[2])
				<< QVector3D(each->color[0], each->color[1], each->color[2]);

			twoDvts
				<< QVector2D(screen[0][0], screen[0][1])
				<< QVector2D(screen[1][0], screen[1][1]);									
			if ((screen[1][0] > 0 && screen[2][0] < 0) &&
				(screen[2][1] > 0 && screen[1][1] < 0))
			{
				twoDvts
					<< QVector2D(screen[3][0], screen[3][1])
					<< QVector2D(screen[2][0], screen[2][1]);				
			}
			else
			{
				twoDvts
					<< QVector2D(screen[2][0], screen[2][1])
					<< QVector2D(screen[3][0], screen[3][1]);
			}
			glEnd();				
			drawWall(twoDvts,colors);
		}
	}
}
void OpenGLWidget::Map_3D()
{	
	//Right window
	glViewport(600, 0, MazeWidget::w, MazeWidget::h);
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
	//- : clockwise +:counter clockwise	
	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	QLineF rf = 
		QLineF(viewerPosX, viewerPosY,
			viewerPosX + 2 * MazeWidget::maze->max_xp * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
			viewerPosY + 2 * MazeWidget::maze->max_yp * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));

	QLineF lf = 
		QLineF(viewerPosX, viewerPosY,
			viewerPosX + 2 * MazeWidget::maze->max_xp * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
			viewerPosY + 2 * MazeWidget::maze->max_yp * sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
	//Draw walls	
	Cell *current = MazeWidget::maze->view_cell;
	drawcells(current,rf,lf);
}