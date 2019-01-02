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
#include "obj.h"
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
	void draw3DObj(bool doingShadows = false);
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
	void paint();
	void myTimer();
	void changeEffect(size_t effectNum);
public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene
	//float t_time;	
	int camera;
	int curve;
	int track;
	bool isrun;
	bool useFBO = false;
	bool trackUpdate = true;
	typedef enum 
	{
		spline_Linear = 0,
		spline_CardinalCubic = 1,
		spline_CubicB_Spline = 2
	} spline_t;
	struct TrackTrail 
	{
		Pnt3f points;
		Pnt3f orients;
		Pnt3f orients_cross;
		TrackTrail() 
		{

		}
		TrackTrail(Pnt3f p, Pnt3f o, Pnt3f oc) 
		{
			points = p;
			orients = o;
			orients_cross = oc;
		}
	};				
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;
	vector<Pnt3f> track_path;
	vector<Pnt3f> track_orient_cross;
	vector<Pnt3f> track_orient;	
	int path_index = 0;	
	//float current_time = 0;	
	Pnt3f trainStart, trainEnd;
	Obj *trackobj;
	Obj *trainobj;
	Obj *miku;
	float rightArmAngle = 0.f;
	float wholeRotateAngle = 0.f;
	float changeAngle = 5.f;	
	float changeAngleOpposite = -5.f;
	float angleTemp = changeAngleOpposite;
	Obj *skybox;
	Obj *water;
	Obj *land;
	Obj *nendoroid;
	Obj *miku3d;
	Obj *mountain;	
	vector<TrackTrail> path;
	//Timer clock
	clock_t effect_clock;
	clock_t train_clock;
	clock_t model_clock;
	float train_interval = 0.f;
	float shadowShake = 0.f;
	size_t effectNum  = 0;
	size_t train_speed = 5;
	size_t train_effect = 0;
};  
#endif // TRAINVIEW_H  