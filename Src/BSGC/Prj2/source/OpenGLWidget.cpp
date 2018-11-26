#include "glew.h"
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
#include<fstream>
#include <sstream> 
#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "Common.h"

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
QOpenGLBuffer *vertexbuffer,*normalbuffer,*indexsbuffer,*texturebuffer;

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
					screen[i][j] += viewmatrix(j, k) * modelmatrix(i, k);//Column major
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
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
	GLint  mode;
	GLint time;
} uniforms;
//model info
typedef struct
{
	GLuint vao;
	GLuint vbo;
	GLuint vboTex;
	GLuint ebo;

	GLuint p_normal;
	int materialId;
	int indexCount;
	GLuint m_texture;

	QMatrix4x4 model;
} Shape;
Shape m_shape;
void LoadModel(const char *objName, const char * textureName);
void Init();
void Render(float *mm, float *pm, GLint shaderMode, GLfloat time);
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
void drawTextures(GLint textureid, QVector<QVector3D> vts, float *mm, float *pm,float time)
{	
	shaderProgram->setUniformValue("Texture", textureid);
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(mm, MV);
	DimensionTransformation(pm, P);
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
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
	//Init();
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(MazeWidget::maze!=NULL)
	{
		////View 1		
		Mini_Map();
		////View 2				
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
	GLfloat mm[16], pm[16];
	glGetFloatv(GL_PROJECTION_MATRIX, mm);
	glGetFloatv(GL_MODELVIEW_MATRIX, pm);
	vts 
		<< QVector3D(-1 , 1 ,0) 
		<< QVector3D(1 , 1 , 0) 
		<< QVector3D(1 , 0 , 0)
		<< QVector3D(1 , 0 , 0) 
		<< QVector3D(-1 , 0 , 0) 
		<< QVector3D(-1 , 1 , 0);			
	drawTextures(1, vts,mm,pm,clock());


	vts.clear();
	//
	vts 
		<< QVector3D(-1, 0,0) << QVector3D(1, 0,0) << QVector3D(1, -1,0) 
		<< QVector3D(1, -1, 0) << QVector3D(-1, -1, 0)<< QVector3D(-1, 0, 0);
	
	drawTextures(2, vts, mm,pm,clock());
	
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
void Init()
{
	/*glewInit();
	sp = glCreateProgram();	
	char** vsSource = Common::LoadShaderSource("./src/bsgc/prj2/assets/vs.glsl");
	char** fsSource = Common::LoadShaderSource("./src/bsgc/prj2/assets/fs.glsl");
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs,1, vsSource,NULL);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs,1, fsSource, NULL);
	glAttachShader(sp,vs);
	glAttachShader(sp,fs);	
	glLinkProgram(sp);
	uniforms.proj_matrix = glGetUniformLocation(sp, "pm");
	uniforms.mv_matrix = glGetUniformLocation(sp, "mv");
	*/
	glewInit();
	shaderProgram = new QOpenGLShaderProgram();
	vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	vertexShader->compileSourceFile("./src/bsgc/prj2/assets/vs.glsl");	
	fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
	fragmentShader->compileSourceFile("./src/bsgc/prj2/assets/fs.glsl");
	shaderProgram->addShader(vertexShader);
	shaderProgram->addShader(fragmentShader);
	shaderProgram->link();	
	LoadModel("./src/bsgc/prj2/assets/MikuHair.obj","./src/bsgc/prj2/assets/MikuHair.png");
}
vector<float> vts, normals,texture_coord,indexes;
QVector<QOpenGLTexture*>  Textures;
void LoadModel(const char *objName, const char * textureName)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, objName,"./src/bsgc/prj2/assets/");
	if (err.size() > 0)
	{
		printf("Load Models Fail! Please check the solution path\n");
		return;
	}
	
	for (size_t i = 0; i < shapes.size(); i++)
	{						
		/*glGenVertexArrays(1, &m_shape.vao);
		glBindVertexArray(m_shape.vao);
		glGenBuffers(3, &m_shape.vbo);
		glGenBuffers(1, &m_shape.p_normal);
		glBindBuffer(GL_ARRAY_BUFFER, m_shape.vbo);
		glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float) + shapes[i].mesh.normals.size() * sizeof(float), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, shapes[i].mesh.positions.size() * sizeof(float), &shapes[i].mesh.positions[0]);
		glBufferSubData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float), shapes[i].mesh.normals.size() * sizeof(float), &shapes[i].mesh.normals[0]);		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)(shapes[i].mesh.positions.size() * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, m_shape.p_normal);
		glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.normals.size() * sizeof(float), shapes[i].mesh.normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, m_shape.vboTex);
		glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.texcoords.size() * sizeof(float), shapes[i].mesh.texcoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shape.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].mesh.indices.size() * sizeof(unsigned int), shapes[i].mesh.indices.data(), GL_STATIC_DRAW);
		m_shape.materialId = shapes[i].mesh.material_ids[0];
		m_shape.indexCount = shapes[i].mesh.indices.size();
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);*/

		vao.create();
		//vao.bind();
		
		for (size_t j = 0; j < shapes[i].mesh.positions.size(); j++)
		{
			vts.push_back(shapes[i].mesh.positions[j]);
		}
		vertexbuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		vertexbuffer->create();
		vertexbuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
		/*vvbo.bind();
		vvbo.allocate(vts.data(), vts.size() * sizeof(float));
		shaderProgram->setAttributeBuffer(0, GL_FLOAT,0,3,0);
		shaderProgram->enableAttributeArray(0);*/
		normalbuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		normalbuffer->create();
		normalbuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
		/*normalbuffer.bind();
		normalbuffer.allocate(shapes[i].mesh.normals.data(), shapes[i].mesh.normals.size() * sizeof(float));
		shaderProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3, 0);*/
		for (size_t j = 0; j < shapes[i].mesh.normals.size(); j++)
		{
			normals.push_back(shapes[i].mesh.normals[j]);
		}		
		texturebuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		texturebuffer->create();
		texturebuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
		/*texturebuffer.bind();
		texturebuffer.allocate(shapes[i].mesh.texcoords.data(), shapes[i].mesh.texcoords.size() * sizeof(float));
		shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);*/
		for (size_t j = 0; j < shapes[i].mesh.texcoords.size(); j++)
		{
			texture_coord.push_back(shapes[i].mesh.texcoords[j]);
		}		
		indexsbuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		indexsbuffer->create();
		/*indexsbuffer.bind();
		indexsbuffer.allocate(shapes[i].mesh.indices.data(), shapes[i].mesh.indices.size() * sizeof(unsigned int));*/
		for (size_t j = 0; j < shapes[i].mesh.indices.size(); j++)
		{
			indexes.push_back(shapes[i].mesh.indices[j]);
		}
		m_shape.materialId = shapes[i].mesh.material_ids[0];
		m_shape.indexCount = shapes[i].mesh.indices.size();
	}	
	//TextureData tdata = Common::Load_png(textureName);
	////Generate empty texture
	//glGenTextures(1, &m_shape.m_texture);
	//glBindTexture(GL_TEXTURE_2D, m_shape.m_texture);
	////Do texture setting
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	Textures.push_back(new QOpenGLTexture(QImage(textureName)));
}
void Render(float *mm, float *pm, GLint shaderMode, GLfloat time)
{	
	//glBindVertexArray(m_shape.vao);
	//glLinkProgram(sp);
	//glActiveTexture(GL_TEXTURE0);//!
	//glBindTexture(GL_TEXTURE_2D, m_shape.m_texture);	
	//glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, pm);
	//glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mm);	
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(pm, P);
	DimensionTransformation(mm, MV);
	vao.bind();
	Textures[0]->bind(1);
	shaderProgram->setUniformValue("pm", P);
	shaderProgram->setUniformValue("mv", MV);
	shaderProgram->setUniformValue("tex", 1);

	vertexbuffer->bind();
	QVector<QVector3D> vt;
	for (int i = 0; i < vts.size(); i+=3)
	{
		vt << QVector3D(vts[i], vts[i+1], vts[i+2]);
	}
	vertexbuffer->allocate(vt.constData(), vt.size() * sizeof(QVector3D));
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);
	shaderProgram->enableAttributeArray(0);

	QVector<QVector3D> ns;
	for (int i = 0; i < normals.size(); i += 3)
	{
		ns << QVector3D(normals[i], normals[i + 1], normals[i + 2]);
	}
	normalbuffer->bind();
	normalbuffer->allocate(ns.constData(), ns.size() * sizeof(float));
	shaderProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3, 0);

	texturebuffer->bind();
	texturebuffer->allocate(texture_coord.data(), texture_coord.size() * sizeof(float));
	shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

	indexsbuffer->bind();
	indexsbuffer->allocate(indexes.data(), indexes.size() * sizeof(unsigned int));

	glDrawElements(GL_TRIANGLES,m_shape.indexCount, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, vts.size());
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);
	vao.release();
	shaderProgram->release();
	//glUniform1i(uniforms.mode, shaderMode);	
	//glUniform1f(uniforms.time, time);	
}
