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
#include<time.h>

using namespace std;
QOpenGLShaderProgram* shaderProgram;
QOpenGLShader* vertexShader;
QOpenGLShader* fragmentShader;
QOpenGLVertexArrayObject vao;
QOpenGLBuffer vvbo;
QOpenGLBuffer cvbo;
QOpenGLBuffer uvbo;
class Player 
{
	std::pair<QLineF, QLineF> frustum;
	QVector3D position;
	float (*modelviewmatrix)[4];
	float frustumarea;
public:
	Player()
	{		
	}
	void setFrustum(QLineF L, QLineF R)
	{				
		frustum.first = L;
		frustum.second = R;
	}
	void setPosition(GLfloat x, GLfloat y, GLfloat z)
	{
		position = QVector3D(x,y,z);
	}
	std::pair<QLineF, QLineF> getFrustum()
	{
		return frustum;
	}
	QVector3D getPosition()
	{
		return position;
	}
	void setModelViewMatrix(QPointF leftintersect, QPointF rightintersect)
	{
		float model[16] =
		{
			rightintersect.y(),1,rightintersect.x(),1,
			rightintersect.y(),-1,rightintersect.x(),1,
			leftintersect.y(),-1,leftintersect.x(),1,
			leftintersect.y(),1,leftintersect.x(),1
		};
		float (*screen)[4] = new float[4][4];		
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				screen[i][j] = 0;
			}
		}
		QMatrix4x4 modelmatrix(model);
		QMatrix4x4 viewmatrix, temp;
		viewmatrix.setToIdentity();
		temp.setToIdentity();
		temp.translate(-MazeWidget::maze->viewer_posn[Maze::Y], 0, -MazeWidget::maze->viewer_posn[Maze::X]);
		viewmatrix.rotate(180, 1, 0, 0);
		viewmatrix.rotate(MazeWidget::maze->viewer_dir, 0, -1, 0);
		viewmatrix *= temp;
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
		modelviewmatrix = screen;
	}
	void getModelViewMatrix(float mv[4][4])
	{
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				mv[i][j] = modelviewmatrix[i][j];
			}
		}		
	}
	void setFrustumArea(float area)
	{		
		frustumarea = area;
	}
	float getFrustumArea()
	{
		return frustumarea;
	}
};
Player player;
float maxWH = 0;
float wall_length = 1.f;
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
	int i = 0;
	for (int j = 0; j < 4; j++) 
	{
		for (int k = 0; k < 4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
	}	
}
void drawTextures(GLint textureid, QVector<QVector3D> vts,float time)
{	
	shaderProgram->setUniformValue("Texture", textureid);
	shaderProgram->setUniformValue("mode",1);
	shaderProgram->setUniformValue("time", time);
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
	//Set projection matrix and viewport
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
				<< QVector2D(edgeEndX, edgeEndY);
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
void setProjection(float mv[4][4])
{
	float depth = maxWH / (10.f * tan(degree_change(MazeWidget::maze->viewer_fov * 0.5f)));
	for (size_t i = 0; i < 4; i++)
	{
		mv[i][0] *= depth / mv[i][2];
		mv[i][1] *= depth / mv[i][2];
	}
}
void drawcells(Cell *current, Player player)
{
	for (size_t i = 0; i < 4; i++)
	{
		bool draw = false;
		Edge *each = current->edges[i];		
		QLineF edge_line = QLineF(each->endpoints[0]->posn[Maze::X], each->endpoints[0]->posn[Maze::Y], each->endpoints[1]->posn[Maze::X], each->endpoints[1]->posn[Maze::Y]);
		QPointF rip;
		QPointF lip;
		int intersect_l = player.getFrustum().first.intersect(edge_line, &lip);
		int intersect_r = player.getFrustum().second.intersect(edge_line, &rip);
		if (intersect_l == 1 && intersect_r == 1)
		{
			draw = true;
		}
		else if (intersect_l != 1 && intersect_r != 1)
		{					
			/*float edge_point1 = 1 / (2 * player.getFrustumArea())
				*(player.getPosition().y() * player.getFrustum().second.x2() - player.getPosition().x() * player.getFrustum().second.y2()
				+ (player.getFrustum().second.y2() - player.getPosition().y() * each->endpoints[0]->posn[Maze::X] + (player.getPosition().x() - player.getFrustum().second.x2())
				*each->endpoints[0]->posn[Maze::Y]));
			float edge_point2 = 1 / (2 * player.getFrustumArea())
				*(player.getPosition().x() * player.getFrustum().first.y2() - player.getPosition().y() * player.getFrustum().first.x2()
				+ (player.getPosition().y() - player.getFrustum().first.y2()) * each->endpoints[0]->posn[Maze::X] 
				+ (player.getFrustum().first.x2() - player.getPosition().x()) * each->endpoints[0]->posn[Maze::Y]);

			if ((0 <= edge_point1 && edge_point1 <= 1.0f) &&
				(0 <= edge_point2 && edge_point2 <= 1.0f) &&
				(edge_point1 + edge_point2  <= 1.0f))
			{
				draw = true;
				lip = QPointF(each->endpoints[0]->posn[Maze::X], each->endpoints[0]->posn[Maze::Y]);
				rip = QPointF(each->endpoints[1]->posn[Maze::X], each->endpoints[1]->posn[Maze::Y]);
			}*/
		}
		else if (intersect_l == 1 && intersect_r != 1 || intersect_l != 1 && intersect_r == 1)
		{
			float edge1_start = 1 / (2 * player.getFrustumArea())*(player.getPosition().y()*player.getFrustum().second.x2() - player.getPosition().x()*player.getFrustum().second.y2() + (player.getFrustum().second.y2() - player.getPosition().y())*each->endpoints[0]->posn[Maze::X] + (player.getPosition().x() - player.getFrustum().second.x2())*each->endpoints[0]->posn[Maze::Y]);
			float edge2_start = 1 / (2 * player.getFrustumArea())*(player.getPosition().x()*player.getFrustum().first.y2() - player.getPosition().y()*player.getFrustum().first.x2() + (player.getPosition().y() - player.getFrustum().first.y2())*each->endpoints[0]->posn[Maze::X] + (player.getFrustum().first.x2() - player.getPosition().x())*each->endpoints[0]->posn[Maze::Y]);
			float edge1_end = 1 / (2 * player.getFrustumArea())*(player.getPosition().y()*player.getFrustum().second.x2() - player.getPosition().x()*player.getFrustum().second.y2() + (player.getFrustum().second.y2() - player.getPosition().y())*each->endpoints[1]->posn[Maze::X] + (player.getPosition().x() - player.getFrustum().second.x2())*each->endpoints[1]->posn[Maze::Y]);
			float edge2_end = 1 / (2 * player.getFrustumArea())*(player.getPosition().x()*player.getFrustum().first.y2() - player.getPosition().y()*player.getFrustum().first.x2() + (player.getPosition().y() - player.getFrustum().first.y2())*each->endpoints[1]->posn[Maze::X] + (player.getFrustum().first.x2() - player.getPosition().x())*each->endpoints[1]->posn[Maze::Y]);
			if (intersect_l == 1 || intersect_r == 1)
			{
				if ((0 <= edge1_start && edge1_start <= 1.0f) &&
					(0 <= edge2_start && edge2_start <= 1.0f) &&
					(edge1_start + edge2_start <= 1.0f))
				{
					draw = true;
					if(intersect_l == 1)
						rip = QPointF(each->endpoints[0]->posn[Maze::X], each->endpoints[0]->posn[Maze::Y]);
					else
						lip = QPointF(each->endpoints[0]->posn[Maze::X], each->endpoints[0]->posn[Maze::Y]);
				}
				if ((0 <= edge1_end && edge1_end <= 1.0f) &&
					(0 <= edge2_end && edge2_end <= 1.0f) &&
					(edge1_end + edge2_end <= 1.0f))
				{
					draw = true;
					if (intersect_l == 1)
						rip = QPointF(each->endpoints[1]->posn[Maze::X], each->endpoints[1]->posn[Maze::Y]);
					else
						lip = QPointF(each->endpoints[1]->posn[Maze::X], each->endpoints[1]->posn[Maze::Y]);
				}
			}		
		}		
		player.setModelViewMatrix(lip, rip);
		float(*mv)[4] = new float[4][4];
		player.getModelViewMatrix(mv);
		setProjection(mv);

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
				<< QVector2D(mv[0][0], mv[0][1])
				<< QVector2D(mv[1][0], mv[1][1]);		
			if ((mv[1][0] > 0 && mv[2][0] < 0) &&
				(mv[2][1] > 0 && mv[1][1] < 0))
			{
				twoDvts
					<< QVector2D(mv[3][0], mv[3][1])
					<< QVector2D(mv[2][0], mv[2][1]);					
			}
			else
			{
				twoDvts
					<< QVector2D(mv[2][0], mv[2][1])
					<< QVector2D(mv[3][0], mv[3][1]);					
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
	maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
	QVector<QVector3D> vts;
	vts 
		<< QVector3D(-1, 1, 0) << QVector3D(1, 1, 0) << QVector3D(1, 0, 0) 
		<< QVector3D(1, 0, 0) << QVector3D(-1, 0, 0) << QVector3D(-1, 1, 0);
	textures[0]->bind(1);//Draw miku
	drawTextures(1, vts, clock());
	vts.clear();
	textures[1]->bind(2);//Draw bricks
	vts 
		<< QVector3D(-1, 0,0) << QVector3D(1, 0,0) << QVector3D(1, -1,0) 
		<< QVector3D(1, -1, 0) << QVector3D(-1, -1, 0)<< QVector3D(-1, 0, 0);
	drawTextures(2, vts,clock());

	player.setPosition(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], MazeWidget::maze->viewer_posn[Maze::Z]);
	player.setFrustum(
		QLineF(player.getPosition().x(), player.getPosition().y(),player.getPosition().x() + 2 * maxWH * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),player.getPosition().y() + 2 * maxWH * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2))),
		QLineF(player.getPosition().x(), player.getPosition().y(),player.getPosition().x() + 2 * maxWH * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),player.getPosition().y() + 2 * maxWH * sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2))));	
	//Draw walls
	player.setFrustumArea(
		0.5f *(-player.getFrustum().first.y2() * player.getFrustum().second.x2() 
		+ player.getPosition().y() * (-player.getFrustum().first.x2() + player.getFrustum().second.x2())
		+ player.getPosition().x() * (player.getFrustum().first.y2() - player.getFrustum().second.y2()) 
		+ player.getFrustum().first.x2() * player.getFrustum().second.y2())
	);
	Cell *current = MazeWidget::maze->view_cell;
	drawcells(current, player);
}