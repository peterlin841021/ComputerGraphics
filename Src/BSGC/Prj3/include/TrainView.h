#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  

#include <QtOpenGL/QGLWidget>  
#include <QtGui/QtGui>  
#include <QtOpenGL/QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "ArcBallCam.H"
#include "3DUtils.H"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "water.h"
#include "land.h"
#include "Cube.h"
#include<iostream>
#include<time.h>
#include<cmath>
#include<fstream>


using namespace std;

class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


class TrainView : public QGLWidget  
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);	
	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);
	void drawTrain(Pnt3f pos, Pnt3f orient,Pnt3f orient_cross ,bool shadow);
	void TrainView::changeSpeed(int speed);
	void drawTrack(bool shadow);
	void initializeGL();
	void initializeTexture();
	void load3Dmodel(char *modelpath);
public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene
	float t_time;
	unsigned int DIVIDE_LINE = 12;
	int camera;
	int curve;
	int track;
	bool isrun;
	typedef enum {
		spline_Linear = 0,
		spline_CardinalCubic = 1,
		spline_CubicB_Spline = 2
	} spline_t;
	struct TrackTrail {
		Pnt3f points;
		Pnt3f orients;
		Pnt3f orients_cross;
		TrackTrail() {

		}
		TrackTrail(Pnt3f p, Pnt3f o, Pnt3f oc) {
			points = p;
			orients = o;
			orients_cross = oc;
		}
	};
	struct Model{
		std::vector<GLfloat> vs;
		std::vector<GLfloat> vts;
		std::vector<GLfloat> vns;
		std::vector<GLuint> fs;
		Model() {

		}
		Model(vector<GLfloat> v, vector<GLfloat> vt, vector<GLfloat> vn, vector<GLuint> f) {
			vs = v;
			vts = vt;
			vns = vn;
			fs = f;
		}
	};
	Triangle* triangle;
	Square* stand;	
	Square* nendoroid_back;
	Square*	nendoroid_front;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;
	vector<Pnt3f> track_path;
	vector<Pnt3f> track_orient_cross;
	vector<Pnt3f> track_orient;
	int cp_size = 4;
	int path_index = 0;	
	float current_time = 0;
	float track_angle = 0;
	Pnt3f trainStart, trainEnd;	
	Cube *skybox;
	Water *water;
	Land *land;
	Land *grass;
	float t_temp = 0;	
	bool trackupdate = true;
	vector<TrackTrail> path;	
	int train_speed = 5;
	float offset = 0.f;
	float wt = 0;
	float shake = 0.5f;	
	Model miku ;
};  
#endif // TRAINVIEW_H  