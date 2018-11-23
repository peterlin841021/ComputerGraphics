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
#define E 0.00001
#define VIEW 100
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
	float(*modelviewmatrix)[4];
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
		position = QVector3D(x, y, z);
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
		float(*screen)[4] = new float[4][4];
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				screen[i][j] = 0;
			}
		}
		QMatrix4x4 modelmatrix(model);
		QMatrix4x4 viewmatrix, temp;
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
float degree_change(float num);
void setProjection(float mv[4][4],int max);
pair<bool, pair<QPointF, QPointF>> clipWall(Edge *wall, Player player, QLineF L, QLineF R, int intersect_l, int intersect_r,QPointF lip, QPointF rip);
Player player;
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
void drawMap(QVector<QVector2D> vts, QVector<QVector3D> colors, float *mm, float *pm)
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
void drawWall(GLint textureid,QVector<QVector2D> vts, QVector<QVector3D> colors,float time)
{	
	//shaderProgram->setUniformValue("mode", 0);
	shaderProgram->setUniformValue("Texture", textureid);
	shaderProgram->setUniformValue("mode", 0);
	shaderProgram->setUniformValue("time", time);
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

	uvbo.bind();
	QVector<QVector2D> uvs;
	uvs
		<< QVector2D(0.0f, 0.0f)
		<< QVector2D(1.0f, 0.f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(0.0f, 1.0f);
		
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
	shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 2, NULL);
	uvbo.release();
	shaderProgram->enableAttributeArray(2);
	glDrawArrays(GL_QUADS, 0, vts.size());
}
void drawcells(Cell *current, Player player, QLineF L, QLineF R)
{
	current->counter = 1;
	for (size_t i = 0; i < 4; i++)
	{				
		Edge *wall = current->edges[i];		
		QLineF edge_line = QLineF
		(
			wall->endpoints[0]->posn[Maze::X], wall->endpoints[0]->posn[Maze::Y],
			wall->endpoints[1]->posn[Maze::X], wall->endpoints[1]->posn[Maze::Y]
		);			
		QPointF rip;
		QPointF lip;
		int intersect_l = L.intersect(edge_line, &lip);
		int intersect_r = R.intersect(edge_line, &rip);
		pair<bool, pair<QPointF, QPointF>> clip = clipWall(wall,player,L,R,intersect_l, intersect_r,lip,rip);	
		player.setModelViewMatrix(clip.second.first, clip.second.second);
		float(*mv)[4] = new float[4][4];
		player.getModelViewMatrix(mv);		
		int maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);		
		setProjection(mv, maxWH);		
		if (clip.first && wall->opaque)
		{
			QVector<QVector2D> twoDvts;
			QVector<QVector3D> colors;
			colors
				<< QVector3D(wall->color[0], wall->color[1], wall->color[2])
				<< QVector3D(wall->color[0], wall->color[1], wall->color[2])
				<< QVector3D(wall->color[0], wall->color[1], wall->color[2])
				<< QVector3D(wall->color[0], wall->color[1], wall->color[2]);
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
			drawWall(3,twoDvts, colors,clock());
		}
		else if (!wall->opaque && clip.first)//Far wall
		{
			Cell *next = wall->neighbors[!wall->Cell_Side(current)];			
			if (next->counter == 0)
			{
				if (intersect_l == 1 && intersect_r == 1 || intersect_l != 1 && intersect_r != 1)
				{
					QLineF L = QLineF(player.getPosition().x(), player.getPosition().y(), clip.second.first.x() + E, clip.second.first.y() + E);
					L.setLength(L.length() * VIEW);
					QLineF R = QLineF(player.getPosition().x(), player.getPosition().y(), clip.second.second.x() + E, clip.second.second.y() + E);
					R.setLength(R.length() * VIEW);					
					drawcells(next, player, L, R);
				}
				else if (intersect_l == 1 || intersect_r == 1)
				{
					QLineF temp;
					if (intersect_l == 1)
					{
						temp = QLineF(player.getPosition().x(), player.getPosition().y(), clip.second.second.x() + E, clip.second.second.y() + E);
						temp.setLength(temp.length() * VIEW);						
						drawcells(next, player, L, temp);
					}
					else
					{
						temp = QLineF(player.getPosition().x(), player.getPosition().y(), clip.second.first.x() + E, clip.second.first.y() + E);
						temp.setLength(temp.length() * VIEW);						
						drawcells(next, player, temp, R);
					}
				}
			}
		}
	}
}
float degree_change(float num)
{
	return num / 180.0f * 3.14159f;
}
void setProjection(float mv[4][4],int max)
{
	float depth = (max) / ((10.f * tan(degree_change(MazeWidget::maze->viewer_fov * 0.5f))));	
	depth /= (max /10);
	for (size_t i = 0; i < 4; i++)
	{
		(mv[i][0]) *= depth / mv[i][2];		
		(mv[i][1]) *= depth / mv[i][2];		
	}
}
pair<bool, pair<QPointF, QPointF>> clipWall(Edge *wall, Player player, QLineF L,QLineF R,int intersect_l, int intersect_r,QPointF lip, QPointF rip)
{
	bool shouldBeDraw = false;	
	pair<bool, pair<QPointF, QPointF>> clip_pair;
	player.setFrustumArea
	(
		0.5f *(-L.y2() * R.x2()
			+ player.getPosition().y() * (-L.x2() + R.x2())
			+ player.getPosition().x() * (L.y2() - R.y2())
			+ L.x2() * R.y2())
	);
	if (intersect_l == 1 && intersect_r == 1)
	{
		shouldBeDraw = true;
	}
	else if (intersect_l != 1 && intersect_r != 1)
	{							
		float edge_point1 = 1.f / (2 * player.getFrustumArea())*(player.getPosition().y() * R.x2() - player.getPosition().x() * R.y2() + (R.y2() - player.getPosition().y()) * wall->endpoints[0]->posn[Maze::X] + (player.getPosition().x() - R.x2()) * wall->endpoints[0]->posn[Maze::Y]);
		float edge_point2 = 1.f / (2 * player.getFrustumArea())*(player.getPosition().x() * L.y2() - player.getPosition().y() * L.x2() + (player.getPosition().y() - L.y2()) * wall->endpoints[0]->posn[Maze::X] + (L.x2() - player.getPosition().x()) * wall->endpoints[0]->posn[Maze::Y]);
		if ((0 <= edge_point1 && edge_point1 <= 1.0f) &&
			(0 <= edge_point2 && edge_point2 <= 1.0f) &&
			(edge_point1 + edge_point2 <= 1.0f))
		{
			shouldBeDraw = true;
			lip = QPointF(wall->endpoints[0]->posn[Maze::X], wall->endpoints[0]->posn[Maze::Y]);
			rip = QPointF(wall->endpoints[1]->posn[Maze::X], wall->endpoints[1]->posn[Maze::Y]);
		}
	}
	else if ((intersect_l == 1 && intersect_r != 1) || (intersect_l != 1 && intersect_r == 1))
	{
		float edge1_start = 1 / (2 * player.getFrustumArea())*(player.getPosition().y()*R.x2() - player.getPosition().x()*R.y2() + (R.y2() - player.getPosition().y())*wall->endpoints[0]->posn[Maze::X] + (player.getPosition().x() - R.x2())*wall->endpoints[0]->posn[Maze::Y]);
		float edge2_start = 1 / (2 * player.getFrustumArea())*(player.getPosition().x()*L.y2() - player.getPosition().y()*L.x2() + (player.getPosition().y() - L.y2())*wall->endpoints[0]->posn[Maze::X] + (L.x2() - player.getPosition().x())*wall->endpoints[0]->posn[Maze::Y]);
		float edge1_end = 1 / (2 * player.getFrustumArea())*(player.getPosition().y()*R.x2() - player.getPosition().x()*R.y2() + (R.y2() - player.getPosition().y())*wall->endpoints[1]->posn[Maze::X] + (player.getPosition().x() - R.x2())*wall->endpoints[1]->posn[Maze::Y]);
		float edge2_end = 1 / (2 * player.getFrustumArea())*(player.getPosition().x()*L.y2() - player.getPosition().y()*L.x2() + (player.getPosition().y() - L.y2())*wall->endpoints[1]->posn[Maze::X] + (L.x2() - player.getPosition().x())*wall->endpoints[1]->posn[Maze::Y]);
		if (intersect_l == 1)
		{
			if ((0 <= edge1_start && edge1_start <= 1.0f) &&
				(0 <= edge2_start && edge2_start <= 1.0f) &&
				(edge1_start + edge2_start <= 1.0f))
			{
				shouldBeDraw = true;
				rip = QPointF(wall->endpoints[0]->posn[Maze::X], wall->endpoints[0]->posn[Maze::Y]);
			}
			if ((0 <= edge1_end && edge1_end <= 1.0f) &&
				(0 <= edge2_end && edge2_end <= 1.0f) &&
				(edge1_end + edge2_end <= 1.0f))
			{
				shouldBeDraw = true;
				rip = QPointF(wall->endpoints[1]->posn[Maze::X], wall->endpoints[1]->posn[Maze::Y]);					
			}
		}
		else if (intersect_r == 1)
		{
			if ((0 <= edge1_start && edge1_start <= 1.0f) &&
				(0 <= edge2_start && edge2_start <= 1.0f) &&
				(edge1_start + edge2_start <= 1.0f))
			{
				shouldBeDraw = true;
				lip = QPointF(wall->endpoints[0]->posn[Maze::X], wall->endpoints[0]->posn[Maze::Y]);
			}
			if ((0 <= edge1_end && edge1_end <= 1.0f) &&
				(0 <= edge2_end && edge2_end <= 1.0f) &&
				(edge1_end + edge2_end <= 1.0f))
			{
				shouldBeDraw = true;
				lip = QPointF(wall->endpoints[1]->posn[Maze::X], wall->endpoints[1]->posn[Maze::Y]);
			}
		}			
	}
	clip_pair.first = shouldBeDraw;
	clip_pair.second.first = lip;
	clip_pair.second.second = rip;
	return clip_pair;
}
OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{	
	top_z = 1.5f;
	but_z = -1;
	
	/*QDir dir("Pic");
	if(dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";	*/
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0,0,0,1);
	glEnable(GL_TEXTURE_2D);
	shader_init();
	textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj2/assets/sky.png")));
	textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj2/assets/snowground.png")));
	textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj2/assets/brick.png")));
	textures[0]->bind(1);//Draw sky
	textures[1]->bind(2);//Draw snow ground
	textures[2]->bind(3);//Draw snow brick wall
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(MazeWidget::maze!=NULL)
	{
		//View 1		
		Mini_Map();
		//View 2				
		Map_3D();				
	}
}
void OpenGLWidget::Mini_Map()	
{
	//Left window
	float maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
	glViewport(0, 0, MazeWidget::w / 2, MazeWidget::h);
	glOrtho(-0.1, maxWH + 0.1, -0.1, maxWH + 0.1, 0, 10);
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
		if (MazeWidget::maze->edges[i]->opaque)
		{			
			float edgeStartX = (MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X]);
			float edgeStartY = (MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y]);
			float edgeEndX = (MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X]);
			float edgeEndY = (MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y]);
			colors
				<< QVector3D(MazeWidget::maze->edges[i]->color[0], MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2])
				<< QVector3D(MazeWidget::maze->edges[i]->color[0], MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
			vts
				<< QVector2D(edgeStartX, edgeStartY)
				<< QVector2D(edgeEndX, edgeEndY);				
		}
	}
	drawMap(vts, colors,mm,pm);
	vts.clear();
	colors.clear();
	//Draw frustum	
	float len = 2.f * (maxWH);
	colors
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, 1);	
	vts
		<< QVector2D(viewerPosX, viewerPosY)
		<< QVector2D(
			viewerPosX +  len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
			viewerPosY +  len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));
	vts
		<< QVector2D(viewerPosX, viewerPosY)
		<< QVector2D(
			viewerPosX +  len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
			viewerPosY +  len * sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
	drawFrustum(vts, colors,mm, pm);
}
void OpenGLWidget::Map_3D()
{		
	//Right window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(600, 0, MazeWidget::w/2, MazeWidget::h);
	int maxWH = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
	QVector<QVector3D> vts;
	vts 
		<< QVector3D(-1, 1, 0) << QVector3D(1, 1, 0) << QVector3D(1, 0, 0) 
		<< QVector3D(1, 0, 0) << QVector3D(-1, 0, 0) << QVector3D(-1, 1, 0);
	
	drawTextures(1, vts, clock());
	vts.clear();
	
	vts 
		<< QVector3D(-1, 0,0) << QVector3D(1, 0,0) << QVector3D(1, -1,0) 
		<< QVector3D(1, -1, 0) << QVector3D(-1, -1, 0)<< QVector3D(-1, 0, 0);
	drawTextures(2, vts,clock());
	//Init cells	
	float len = 2.f * maxWH;//20 or 80
	for (int i = 0; i < MazeWidget::maze->num_cells; i++)
	{
		MazeWidget::maze->cells[i]->counter = 0;
	}
	player.setPosition(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], MazeWidget::maze->viewer_posn[Maze::Z]);
	player.setFrustum(		
		QLineF(player.getPosition().x(), player.getPosition().y(), player.getPosition().x() + len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
			player.getPosition().y() + len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2))),		
		QLineF(player.getPosition().x(), player.getPosition().y(), player.getPosition().x() + len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)), 
			player.getPosition().y() + len *  sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)))
		);				
	MazeWidget::maze->Find_View_Cell(MazeWidget::maze->view_cell);
	//Draw walls
	drawcells(MazeWidget::maze->view_cell, player, player.getFrustum().first, player.getFrustum().second);	
}
