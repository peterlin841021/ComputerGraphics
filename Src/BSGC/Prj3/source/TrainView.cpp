#include <glew.h>
#include <time.h>
#include"TrainView.h"

float uv[12] = { 0.f , 0.f, 1.f , 0.f, 1.f , 1.f, 1.f , 1.f, 0.f , 1.f, 0.f , 0.f };
void TrainView::initializeGL()
{	
	initializeTexture();
	miku = new Obj();
	miku->Init(2);
	land = new Obj();
	land->Init(2);
	
	water = new Obj();
	water->Init(2);
	//grass = new Land();
	//grass->Init();
	skybox = new Obj();
	skybox->Init(2);

	nendoroid_back = new Obj();
	nendoroid_back->Init(2);
	nendoroid_front = new Obj();
	nendoroid_front->Init(2);
}

//GLuint loadTexture(vector<QImage> faces) 
//{
//	GLuint id;		
//	if (!faces[0].isNull()) {	
//		glEnable(GL_TEXTURE_CUBE_MAP);
//		glGenTextures(1, &id);		
//		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//		for (GLuint i = 0; i < 6; i++)
//		{
//			//printf("%d*%d\n", faces[i].width(), faces[i].height());
//			QImage temp = QGLWidget::convertToGLFormat(faces[i]);
//			glTexImage2D(
//				GL_TEXTURE_CUBE_MAP + i, 0,
//				GL_RGBA, temp.width(), temp.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, temp.bits());
//		}
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);		
//		glDisable(GL_TEXTURE_CUBE_MAP);
//		return id;
//	}	
//	return -1;
//}
void TrainView::initializeTexture()
{	
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/miku_transparent.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/underwater.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/water_texture.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_front.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_back.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_left.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_right.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_top.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_bottom.jpg")));	
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/hangingstone_bottom.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/nendoroid_front.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/nendoroid_back.png")));	
}
TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();	
}  
TrainView::~TrainView()  
{}  

void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

void TrainView::changeSpeed(int speed) 
{
	train_speed = speed;		
}

void TrainView::paintGL()
{	
	glViewport(0,0,width(),height());
	glClearColor(0,0,0,0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);	
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	if (this->camera == 1) 
	{
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);	
	glDisable(GL_LIGHTING);

	glEnable(GL_LIGHTING);
	setupObjects();

	//Get modelview matrix	
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	//All texture
	Textures[0]->bind(0);//miku
	Textures[1]->bind(1);//land
	Textures[2]->bind(2);//water
	Textures[3]->bind(3);//sky...
	Textures[4]->bind(4);
	Textures[5]->bind(5);
	Textures[6]->bind(6);
	Textures[7]->bind(7);
	Textures[8]->bind(8);	
	Textures[9]->bind(9);
	Textures[10]->bind(10);//nendoroid f	
	Textures[11]->bind(11);//nendoroid b
	
	glDisable(GL_DEPTH_TEST);
	std::vector<int> buffer_size;
	float box_width = 1.f;
	float box_offset = /*0.7f*/1.f;
	float skyboxVertices[] = 
	{		
		/*//////positions//////*/		
		//front
		box_width,box_width + box_offset,box_width,
		-box_width,box_width + box_offset,box_width,
		-box_width,-box_width + box_offset,box_width,

		-box_width,-box_width + box_offset,box_width,
		box_width,-box_width + box_offset,box_width,
		box_width,box_width + box_offset,box_width,

		//back
		-box_width,box_width + box_offset,-box_width,
		box_width,box_width + box_offset,-box_width,
		box_width,-box_width + box_offset,-box_width,

		box_width,-box_width + box_offset,-box_width,
		-box_width,-box_width + box_offset,-box_width,
		-box_width,box_width + box_offset,-box_width,

		//left
		-box_width,box_width + box_offset,box_width,
		-box_width,box_width + box_offset,-box_width,
		-box_width,-box_width + box_offset,-box_width,

		-box_width,-box_width + box_offset,-box_width,
		-box_width,-box_width + box_offset,box_width,
		-box_width,box_width + box_offset,box_width,

		//right
		box_width,box_width + box_offset,-box_width,
		box_width,box_width + box_offset,box_width,
		box_width,-box_width + box_offset,box_width,

		box_width,-box_width + box_offset,box_width,
		box_width,-box_width + box_offset,-box_width,
		box_width,box_width + box_offset,-box_width,

		//top
		-box_width,box_width + box_offset,-box_width,
		-box_width,box_width + box_offset,box_width,
		box_width,box_width + box_offset,box_width,

		box_width,box_width + box_offset,box_width,
		box_width,box_width + box_offset,-box_width,
		-box_width,box_width + box_offset,-box_width,

		//bottom
		box_width,-box_width + box_offset,-box_width,
		box_width,-box_width + box_offset,box_width,
		-box_width,-box_width + box_offset,box_width,

		-box_width,-box_width + box_offset,box_width,
		-box_width,-box_width + box_offset,-box_width,
		box_width,-box_width + box_offset,-box_width
	};
	float boxsize = 1000.f;
		
	int vt_size = sizeof(skyboxVertices)/sizeof(float)/6;
	int start = 0;
	QVector<GLfloat> skybox_vts;
	skybox->Begin();	
	for (size_t i = 0; i < 6; i++)
	{		
		skybox->shaderProgram->setUniformValue("tex", 3+i);
		start = i * vt_size;
		for (size_t j = 0; j < vt_size; j++)
		{
			skybox_vts << skyboxVertices[start+j] * boxsize;
		}
		for (size_t j = 0; j < 12; j++)
		{
			skybox_vts << uv[j];
		}
		buffer_size.push_back(18);
		buffer_size.push_back(12);
		
		skybox->Render(ProjectionMatrex, ModelViewMatrex, skybox_vts, buffer_size,2,1.f,clock());
		skybox_vts.clear();
		buffer_size.clear();
	}		
	skybox->End();
	glEnable(GL_DEPTH_TEST);
	
	//Draw Miku
	QVector<GLfloat> miku_vts;
	miku_vts
		<< -15.1f << 40.8f << 0.f
		<< 15.1f << 40.8f << 0.f
		<< 15.1f << 3.8f << 0.f
		<< 15.1f << 3.8f << 0.f
		<< -15.1f << 3.8f << 0.f
		<< -15.1f << 40.8f << 0.f;
	buffer_size.push_back(18);
	buffer_size.push_back(12);
	for (size_t i = 0; i < 12; i++)
	{
		miku_vts << uv[i];
	}
	float mikuSize = 0.5f;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1);

	miku->Begin();
	miku->shaderProgram->setUniformValue("tex", 0);
	miku->Render(ProjectionMatrex, ModelViewMatrex, miku_vts, buffer_size,1,1.f,clock());
	miku->End();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	//Draw track and train
	drawStuff();
	//Draw land
	float wave_t  = 0;						
	land->Begin();
	land->shaderProgram->setUniformValue("tex",1);
	QVector<GLfloat> land_vts;
	land_vts
		<< -boxsize << 0 << -boxsize
		<< boxsize << 0 << -boxsize
		<< boxsize << 0 << boxsize
		<< boxsize << 0 << boxsize
		<< -boxsize << 0 << boxsize
		<< -boxsize << 0 << -boxsize;
	for (size_t i = 0; i < 12; i++)
	{
		land_vts << uv[i];
	}
	buffer_size.clear();
	buffer_size.push_back(18);
	buffer_size.push_back(12);
	land->Render(ProjectionMatrex, ModelViewMatrex, land_vts,buffer_size,1,1,0);
	land->End();
	
	//Draw water
	int water_size = 70;
	float min = -boxsize;
	float amplitude = 2.5f;
	float speed = 0.0001f;
	float step = boxsize / water_size * 2;
	float old_height = 0;
	float wy = 8.f;
	float height = 0.f;
	setupFloor();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	water->Begin();
	water->shaderProgram->setUniformValue("tex", 2);
	float ratio = 25;
	float xfrom = 0;
	float zfrom = 0;
	float xto = 0;
	float zto = 0;
	float padding = 0;
	
	QVector<GLfloat> water_vertices;
	
	if (wt == 0)
		wt = clock();

	for (int i = 0; i < water_size; i++)
	{
		wt = clock();		
		for (int j = 0; j < water_size; j++)
		{			
			if(old_height == 0)
				old_height = amplitude * sin(step + wt * speed / 5.f);
			else
				old_height = height;

			height = amplitude * sin(step + wt*((water_size - i)*(j+1))/5.f* speed/5.f);
			xfrom = min + step * j + offset;
			xto = min + step * (j + 1) + offset;
			zfrom = min + step * i;
			zto = min + step * (i + 1);
			//pos
			water_vertices
				<< xfrom  << wy + old_height << zto 
				<< xto  << wy + height << zto 
				<< xto  << wy + height << zfrom;
			water_vertices
				<< xto  << wy + height << zfrom 
				<< xfrom  << wy + old_height << zfrom 
				<< xfrom  << wy + old_height << zto ;
		}		
	}
	for (int i = 0; i < water_size; i++)
	{
		for (int j = 0; j < water_size; j++)
		{
			//uvs
			water_vertices
				<< i / ratio << (j + 1) / ratio
				<< (i + 1) / ratio << (j + 1) / ratio
				<< (i + 1) / ratio << j / ratio

				<< (i + 1) / ratio << j / ratio
				<< i / ratio << j / ratio
				<< i / ratio << (j + 1) / ratio

				<< i / ratio << (j + 1) / ratio
				<< (i + 1) / ratio << (j + 1) / ratio
				<< (i + 1) / ratio << j / ratio

				<< (i + 1) / ratio << j / ratio
				<< i / ratio << j / ratio
				<< i / ratio << (j + 1) / ratio;
		}
	}
	
	buffer_size.clear();
	buffer_size.push_back(18 * water_size * water_size);
	buffer_size.push_back(24 * water_size * water_size);
	water->Render(ProjectionMatrex, ModelViewMatrex, water_vertices, buffer_size, 1, 0.45f,0);

	water_vertices.clear();
	buffer_size.clear();
	
	water_vertices 
		<< min << wy << min << -min << wy << min<< -min << wy << -min
		<< -min << wy << -min<< min << wy << -min<< min << wy << min;
	for (size_t i = 0; i < sizeof(uv)/sizeof(float); i++)
	{
		water_vertices << uv[i];
	}
	buffer_size.push_back(18);
	buffer_size.push_back(12);
	water->Render(ProjectionMatrex, ModelViewMatrex, water_vertices, buffer_size, 1, 0.45f,0);
	water->End();
	water_vertices.clear();
	buffer_size.clear();

	//Draw shadows			
	if (this->camera != 1) 
	{
		glTranslatef(0, shake, 0);
		setupShadows();
		drawStuff(true);
		unsetupShadows();
		if (shake > 0)
			shake = -shake;
		else
			shake = -shake;
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());
	
	// Check whether we use the world camp
	if (this->camera == 0)
	{						
		glMatrixMode(GL_MODELVIEW);		
		glLoadIdentity();
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}else if (this->camera == 1) 
	{
		float wi, he;
		if (aspect >= 1) 
		{
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}
		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		float view_ratio = 4.5f;
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi * view_ratio, wi * view_ratio, -he * view_ratio, he * view_ratio, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	}	
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else if (this->camera == 2) 
	{		
		if (path.size() > 0) 
		{
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);				
		gluPerspective(120, 1, 1, 200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();		
		Pnt3f p1 = path[path_index].points;
		Pnt3f p2 = path[(path_index + 1) % path.size()].points;
		Pnt3f dec = trainEnd - trainStart;
		dec.normalize();		
		gluLookAt
		(
			p1.x, p1.y + 20.0, p1.z,//camera coordinates
			p2.x + dec.x, p2.y + 20.0, p2.z + dec.z,//look for
			path[(path_index)].orients.x, path[(path_index)].orients.y, path[(path_index)].orients.z
		);		
		glPopMatrix();
		update();
		}		
		
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif		
		
	}
}
//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawTrack(bool doingShadows) 
{
	float alpha = 0.3f;
	spline_t spline_type = (spline_t)curve;
	Pnt3f qt0, qt, orient_t, or0, or1;		
	track_path.clear();
	track_orient_cross.clear();
	track_orient.clear();
	Pnt3f track_start_inside, track_start_outside, track_end_inside, track_end_outside;
	Pnt3f temp;
	for (size_t i = 0; i < m_pTrack->points.size(); i++) 
	{
		Pnt3f points1 = m_pTrack->points[i].pos;
		Pnt3f points2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		Pnt3f orient1 = m_pTrack->points[i].orient;
		Pnt3f orient2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		//***********************//
		float distance = sqrt(pow(points1.x - points2.x, 2) + pow(points1.y - points2.y, 2) + pow(points1.z - points2.z, 2));
		unsigned int divide = distance / 8.f;
		if (divide < 4)
		{
			divide = 4;
		}
		ControlPoint q_matrix[4];
		//ControlPoint previous, next;
		
		if (i == 0)
		{
			q_matrix[0] = m_pTrack->points[m_pTrack->points.size() - 2];
			q_matrix[1] = m_pTrack->points[m_pTrack->points.size() - 1];
		}
		else if (i == 1)
		{
			q_matrix[0] = m_pTrack->points[m_pTrack->points.size() - 1];
			q_matrix[1] = m_pTrack->points[0];
		}
		else
		{
			q_matrix[0] = m_pTrack->points[i - 2];
			q_matrix[1] = m_pTrack->points[i - 1];
		}
		//previous = m_pTrack->points[m_pTrack->points.size() - 1];
		//q_matrix[0] = previous;
		//q_matrix[1] = m_pTrack->points[i];
		//q_matrix[2] = m_pTrack->points[(i + 1) % m_pTrack->points.size()];
		q_matrix[2] = m_pTrack->points[(i) % m_pTrack->points.size()];
		q_matrix[3] = m_pTrack->points[(i + 1) % m_pTrack->points.size()];
		//q_matrix[3] = m_pTrack->points[(i + 2) % m_pTrack->points.size()];

		float percent = 1.f / divide;
		float t = 0.f - percent;
		if (spline_type == spline_CardinalCubic) 
		{
			/*qt0 = Pnt3f(q_matrix[3].pos * (-1 / 2.f) + q_matrix[0].pos * (3 / 2.f) + q_matrix[1].pos * (-3 / 2.f) + q_matrix[2].pos * (1 / 2.f)) * pow(t, 3)
				+ (q_matrix[3].pos * (2 / 2.f) + q_matrix[0].pos * (-5 / 2.f) + q_matrix[1].pos * (4 / 2.f) + q_matrix[2].pos * (-1 / 2.f)) * pow(t, 2)
				+ (q_matrix[3].pos * (-1 / 2.f) + q_matrix[0].pos * 0 + q_matrix[1].pos * (1 / 2.f) + q_matrix[2].pos * 0) * pow(t, 1)
				+ (q_matrix[3].pos * 0 + q_matrix[0].pos * (2 / 2.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * 0) * 1;*/

			qt = Pnt3f(q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * (3 / 2.f) + q_matrix[2].pos * (-3 / 2.f) + q_matrix[3].pos * (1 / 2.f)) * pow(t, 3)
				+ (q_matrix[0].pos * (2 / 2.f) + q_matrix[1].pos * (-5 / 2.f) + q_matrix[2].pos * (4 / 2.f) + q_matrix[3].pos * (-1 / 2.f)) * pow(t, 2)
				+ (q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (1 / 2.f) + q_matrix[3].pos * 0) * pow(t, 1)
				+ (q_matrix[0].pos * 0 + q_matrix[1].pos * (2 / 2.f) + q_matrix[2].pos * 0 + q_matrix[3].pos * 0) * 1;
			orient_t = Pnt3f(q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * (3 / 2.f) + q_matrix[2].orient * (-3 / 2.f) + q_matrix[3].orient * (1 / 2.f)) * pow(t, 3)
				+ (q_matrix[0].orient * (2 / 2.f) + q_matrix[1].orient * (-5 / 2.f) + q_matrix[2].orient * (4 / 2.f) + q_matrix[3].orient * (-1 / 2.f)) * pow(t, 2)
				+ (q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (1 / 2.f) + q_matrix[3].orient * 0) * pow(t, 1)
				+ (q_matrix[0].orient * 0 + q_matrix[1].orient * (2 / 2.f) + q_matrix[2].orient * 0 + q_matrix[3].orient * 0) * 1;
		}
		else if (spline_type == spline_CubicB_Spline) 
		{
			/*qt0 = (q_matrix[3].pos * (-1 / 6.f) + q_matrix[0].pos * (3 / 6.f) + q_matrix[1].pos * (-3 / 6.f) + q_matrix[2].pos * (1 / 6.f)) * pow(t, 3)
				+ (q_matrix[3].pos * (3 / 6.f) + q_matrix[0].pos * (-6 / 6.f) + q_matrix[1].pos * (3 / 6.f) + q_matrix[2].pos * 0) * pow(t, 2)
				+ (q_matrix[3].pos * (-3 / 6.f) + q_matrix[0].pos * 0 + q_matrix[1].pos * (3 / 6.f) + q_matrix[2].pos * 0) * pow(t, 1)
				+ (q_matrix[3].pos * (1 / 6.f) + q_matrix[0].pos * (4 / 6.f) + q_matrix[1].pos * (1 / 6.f) + q_matrix[2].pos * 0) * 1;*/

			qt = (q_matrix[0].pos * (-1 / 6.f) + q_matrix[1].pos * (3 / 6.f) + q_matrix[2].pos * (-3 / 6.f) + q_matrix[3].pos * (1 / 6.f)) * pow(t, 3)
				+ (q_matrix[0].pos * (3 / 6.f) + q_matrix[1].pos * (-6 / 6.f) + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 2)
				+ (q_matrix[0].pos * (-3 / 6.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 1)
				+ (q_matrix[0].pos * (1 / 6.f) + q_matrix[1].pos * (4 / 6.f) + q_matrix[2].pos * (1 / 6.f) + q_matrix[3].pos * 0) * 1;
			orient_t = Pnt3f(q_matrix[0].orient * (-1 / 6.f) + q_matrix[1].orient * (3 / 6.f) + q_matrix[2].orient * (-3 / 6.f) + q_matrix[3].orient * (1 / 6.f)) * pow(t, 3)
				+ (q_matrix[0].orient * (3 / 6.f) + q_matrix[1].orient * (-6 / 6.f) + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 2)
				+ (q_matrix[0].orient * (-3 / 6.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 1)
				+ (q_matrix[0].orient * (1 / 6.f) + q_matrix[1].orient * (4 / 6.f) + q_matrix[2].orient * (1 / 6.f) + q_matrix[3].orient * 0) * 1;
		}
		else if (spline_type == spline_Linear) 
		{
			qt0 = points1;
			qt = points2;
			if (t < 0)
				t = 0;
			orient_t = (1.f - t)*orient1 + t * orient2;
		}

		Pnt3f connect_outside, connect_inside;
		for (size_t j = 0; j <= divide; j++)
		{
			if(j == 0)
				qt0 = qt0;
			else
				qt0 = qt;

			or0 = orient_t;
			trainStart = qt0;
			switch (spline_type) 
			{
			case spline_Linear:
				orient_t = (1.f - t)*orient1 + t * orient2;
				break;
			case spline_CardinalCubic:
				orient_t = Pnt3f(q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * (3 / 2.f) + q_matrix[2].orient * (-3 / 2.f) + q_matrix[3].orient * (1 / 2.f)) * pow(t, 3)
					+ (q_matrix[0].orient * (2 / 2.f) + q_matrix[1].orient * (-5 / 2.f) + q_matrix[2].orient * (4 / 2.f) + q_matrix[3].orient * (-1 / 2.f)) * pow(t, 2)
					+ (q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (1 / 2.f) + q_matrix[3].orient * 0) * pow(t, 1)
					+ (q_matrix[0].orient * 0 + q_matrix[1].orient * (2 / 2.f) + q_matrix[2].orient * 0 + q_matrix[3].orient * 0) * 1;
				break;
			case spline_CubicB_Spline:
				orient_t = Pnt3f(q_matrix[0].orient * (-1 / 6.f) + q_matrix[1].orient * (3 / 6.f) + q_matrix[2].orient * (-3 / 6.f) + q_matrix[3].orient * (1 / 6.f)) * pow(t, 3)
					+ (q_matrix[0].orient * (3 / 6.f) + q_matrix[1].orient * (-6 / 6.f) + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 2)
					+ (q_matrix[0].orient * (-3 / 6.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 1)
					+ (q_matrix[0].orient * (1 / 6.f) + q_matrix[1].orient * (4 / 6.f) + q_matrix[2].orient * (1 / 6.f) + q_matrix[3].orient * 0) * 1;
				break;
			}
			t += percent;
			or1 = orient_t;
			switch (spline_type) 
			{
			case spline_Linear:
				qt = (1.f - t)*points1 + t * points2;
				break;
			case spline_CardinalCubic:
				qt = Pnt3f(q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * (3 / 2.f) + q_matrix[2].pos * (-3 / 2.f) + q_matrix[3].pos * (1 / 2.f)) * pow(t, 3)
					+ (q_matrix[0].pos * (2 / 2.f) + q_matrix[1].pos * (-5 / 2.f) + q_matrix[2].pos * (4 / 2.f) + q_matrix[3].pos * (-1 / 2.f)) * pow(t, 2)
					+ (q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (1 / 2.f) + q_matrix[3].pos * 0) * pow(t, 1)
					+ (q_matrix[0].pos * 0 + q_matrix[1].pos * (2 / 2.f) + q_matrix[2].pos * 0 + q_matrix[3].pos * 0) * 1;
				break;
			case spline_CubicB_Spline:
				qt = (q_matrix[0].pos * (-1 / 6.f) + q_matrix[1].pos * (3 / 6.f) + q_matrix[2].pos * (-3 / 6.f) + q_matrix[3].pos * (1 / 6.f)) * pow(t, 3)
					+ (q_matrix[0].pos * (3 / 6.f) + q_matrix[1].pos * (-6 / 6.f) + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 2)
					+ (q_matrix[0].pos * (-3 / 6.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 1)
					+ (q_matrix[0].pos * (1 / 6.f) + q_matrix[1].pos * (4 / 6.f) + q_matrix[2].pos * (1 / 6.f) + q_matrix[3].pos * 0) * 1;
				break;
			}			
			
			trainEnd = qt;
			if (!doingShadows) 
			{
				//glColor3ub(32, 32, 64);
				glColor4f(0, 0, 0, 1);
			}
			else 
			{
				glColor4f(0, 0, 0, alpha);
			}
			if (track == 0) 
			{				
				if (!doingShadows) 
				{
					glColor4f(0, 0, 0, 1);
				}
				else 
				{
					glColor4f(0, 0, 0, alpha);
				}				
				glLineWidth(5);
				glBegin(GL_LINES);
				glVertex3f(qt0.x, qt0.y, qt0.z);
				glVertex3f(qt.x, qt.y, qt.z);
				glEnd();
			}
			Pnt3f cross_t0, cross_t1;
			float track_width = 5.f;
			cross_t0 = Pnt3f(qt.x - qt0.x, qt.y - qt0.y, qt.z - qt0.z) * or0;
			cross_t1 = Pnt3f(qt.x - qt0.x, qt.y - qt0.y, qt.z - qt0.z) * or1;
			cross_t0.normalize();
			cross_t1.normalize();

			cross_t0 = cross_t0 * track_width;//Track width
			//cross_t1 = cross_t1 * 5.f;
			//if (i == 0 && j == 0) 
			//{
			//	head_in = Pnt3f(qt1.x - cross_t0.x, qt1.y - cross_t0.y, qt1.z - cross_t0.z);
			//	head_out = Pnt3f(qt1.x + cross_t0.x, qt1.y + cross_t0.y, qt1.z + cross_t0.z);
			//}
			
			if (track == 1) 
			{				
				glLineWidth(2);								
				orient_t.normalize();
				float tan_v = atan2f(cross_t0.z, cross_t0.x);
				if (!doingShadows) 
				{
					glColor4f(0, 0, 0, 1);
				}
				else 
				{
					glColor4f(0, 0, 0, alpha);
				}
				if (j > 1 && spline_type != spline_Linear)
				{		
					glBegin(GL_LINES);
					glVertex3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
					glVertex3f(connect_inside.x, connect_inside.y, connect_inside.z);
					glVertex3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
					glVertex3f(connect_outside.x, connect_outside.y, connect_outside.z);
					glEnd();
				}
				else if(j == 1 && spline_type == spline_Linear)
				{
					glBegin(GL_LINES);
					glVertex3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
					glVertex3f(connect_inside.x, connect_inside.y, connect_inside.z);
					glVertex3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
					glVertex3f(connect_outside.x, connect_outside.y, connect_outside.z);
					glEnd();
				}
				else if ( (j != divide && j > 1) && spline_type == spline_Linear)
				{
					glBegin(GL_LINES);
					glVertex3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
					glVertex3f(connect_inside.x, connect_inside.y, connect_inside.z);
					glVertex3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
					glVertex3f(connect_outside.x, connect_outside.y, connect_outside.z);
					glEnd();
				}							

				if (!doingShadows) 
				{
					glColor4f(255, 255, 255, 1);
				}
				else 
				{
					glColor4f(255, 255, 255, alpha);
				}
				//Ties
				if (j == divide && spline_type == spline_Linear)
				{

				}
				else
				{
					glBegin(GL_LINES);
					glVertex3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
					glVertex3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
					glEnd();
				}
				
				connect_inside = Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
				connect_outside = Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
			}
			if (j > 0) 
			{
				track_path.push_back(Pnt3f(qt.x, qt.y, qt.z));
				track_orient_cross.push_back(cross_t0);
				track_orient.push_back(orient_t);
			}			
		}//Divide
	}//Controlpoints
	//Generate path
	if (trackupdate) 
	{
		
		
	}
	path.clear();
	trackupdate = false;
	float track_distance = 0.f;
	for (size_t i = 0; i < track_path.size(); i++)
	{
		TrackTrail t1 = TrackTrail(track_path[i], track_orient[i], track_orient_cross[i]);
		TrackTrail t2 = TrackTrail(track_path[(i + 1) % track_path.size()],track_orient[(i + 1) % track_orient.size()],track_orient_cross[(i + 1) % track_orient_cross.size()]);		
		path.push_back(t1);
	}
}
void TrainView::drawStuff(bool doingShadows)
{
	//glTranslatef(0, 8, 0);
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != 2) 
	{
		for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) 
		{
			if (!doingShadows) 
		{
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else{
					glColor3ub(240, 240, 30);					
				}
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	drawTrack(doingShadows);
#ifdef EXAMPLE_SOLUTION
	//drawTrack(this, doingShadows);
	
#endif
	
	t_temp = clock();
	if (isrun && path.size() > 0) 
	{		
		if ((t_temp - current_time) > 1000.f/(train_speed*2)) 
		{
			current_time = t_temp;
			path_index++;
		}
		glPushMatrix();
		drawTrain(path[path_index].points, path[path_index].orients_cross, path[path_index].orients,doingShadows);		
		glPopMatrix();
		if (path_index == path.size() - 1)
			path_index = 0;		
	}
	if (!isrun && path.size() > 0) 
	{
		glPushMatrix();
		drawTrain(path[path_index].points, path[path_index].orients_cross, path[path_index].orients, doingShadows);		
		glPopMatrix();
	}
	
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif	
}
void TrainView::doPick(int mx, int my)
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) 
	{
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}
void TrainView::drawTrain(Pnt3f pos, Pnt3f orient_cross,Pnt3f orient,bool shadow)
{	
	float alpha = 0.3f;
	int height = 2;
	int width = 5;
	int length = 3;
	float angle = 0.f;	
	glPushMatrix();	
	pos.y += 3;
	glTranslatef(pos.x, pos.y, pos.z);

	float w = 10 / 2, h = 10 / 2;

	if (shadow) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}	
	angle = -radiansToDegrees(atan2(orient_cross.z, orient_cross.x));	
	glRotatef(angle, 0, 1, 0);	
	if(angle > 0)
		angle = -radiansToDegrees(acos(orient.y));
	else
		angle = radiansToDegrees(acos(orient.y));	
	glRotatef(-angle, 0, 0, orient.z);	
	//Train body		
	if (!shadow)
		glColor4f(127, 127, 127,1.f);
	else
		glColor4f(127, 127, 127, alpha);
	glBegin(GL_QUADS);
	glVertex3f(-width+1, height, -width + 1 - length);
	glVertex3f(width-1, height, -width + 1 - length);
	glVertex3f(width-1, height + 3, width - 3 );
	glVertex3f(-width+1, height + 3, width - 3 );

	glVertex3f(-width+1, height + 3, width - 3 );
	glVertex3f(-width+1, height , -width + 1 - length);
	glVertex3f(-width+1, height , width - 3);
	glVertex3f(-width+1, height + 3, width - 3 );

	glVertex3f(width-1, height + 3, width - 3 );
	glVertex3f(width-1, height , -width + 1 - length);
	glVertex3f(width-1, height , width - 3 );
	glVertex3f(width-1, height + 3, width - 3 );
	glEnd();
	if (!shadow)
		glColor4f(0, 0, 0,1.f);
	else
		glColor4f(0, 0, 0, alpha);			
	glBegin(GL_QUADS);		
	//Bottom	
	glNormal3f(0, -1, 0);
	glVertex3f(-width,-height, -width - length);
	glVertex3f(width, -height, -width - length);
	glVertex3f(width, -height, width + length);
	glVertex3f(-width, -height, width + length);
	//Top
	glNormal3f(0, 1, 0);
	glVertex3f(-width,height,-width - length);
	glVertex3f(width,height,-width - length);
	glVertex3f(width,height,width + length);
	glVertex3f(-width,height,width + length);
	//Right
	glNormal3f(-1, 0, 0);
	glVertex3f(-width,height,-width - length);
	glVertex3f(-width,-height,-width - length);
	glVertex3f(-width,-height,width + length);
	glVertex3f(-width,height,width + length);
	//Left
	glNormal3f(1, 0, 0);
	glVertex3f(width,height,-width - length);
	glVertex3f(width,-height,-width - length);
	glVertex3f(width,-height,width + length);
	glVertex3f(width,height,width + length);
	//Front
	glNormal3f(0, 0, 1);
	glVertex3f(width,height,width);
	glVertex3f(-width,height,width);
	glVertex3f(-width,-height,width);
	glVertex3f(+width,-height,width);
	//Rear
	glNormal3f(0, 0, -1);
	glVertex3f(width,height,-width);
	glVertex3f(-width,height,-width);
	glVertex3f(-width,-height,-width);
	glVertex3f(width,-height,-width);
	glEnd();
	//Strip
	if (!shadow)
		glColor4f(255, 0, 0,1.f);
	else
		glColor4f(255, 0, 0, alpha);
	glBegin(GL_QUADS);
	//Right
	float space = 0.1f;
	glNormal3f(-1, 0, 0);
	glVertex3f(-width- space, height/2, -width);
	glVertex3f(-width- space, -height/2, -width);
	glVertex3f(-width- space, -height/2, width);
	glVertex3f(-width- space, height/2, width);
	//Left
	glNormal3f(1, 0, 0);
	glVertex3f(width+ space, height/2, -width);
	glVertex3f(width+ space, -height/2, -width);
	glVertex3f(width+ space, -height/2, width);
	glVertex3f(width+ space, height/2, width);
	glEnd();
	if (!shadow)
		glColor4f(255, 255, 0,1.f);
	else
		glColor4f(255, 255, 0, alpha);
	glBegin(GL_QUADS);
	//Bottom
	glNormal3f(0, -1, 0);
	glVertex3f(-2, -0.2, -width -0.5);
	glVertex3f(-1, -0.2, -width - 0.5);
	glVertex3f(-1, -0.2, -width);
	glVertex3f(-2, -0.2, -width);
	////Top
	glNormal3f(0, 1, 0);
	glVertex3f(-2, 0.2, -width - 0.5);
	glVertex3f(-1, 0.2, -width - 0.5);
	glVertex3f(-1, 0.2, -width);
	glVertex3f(-2, 0.2, -width);
	////Front
	glNormal3f(0, 0, 1);
	glVertex3f(-1,0.2, -width - 0.5);
	glVertex3f(-2,0.2, -width - 0.5);
	glVertex3f(-2,-0.2, -width - 0.5);
	glVertex3f(-1,-0.2, -width - 0.5);
	////Left
	glNormal3f(-1, 0,0);
	glVertex3f(-1,0.2, -width - 0.5);
	glVertex3f(-1,-0.2, -width - 0.5);
	glVertex3f(-1,-0.2, -width);
	glVertex3f(-1,0.2, -width);
	////Right
	glNormal3f(1, 0, 0);
	glVertex3f(-2, 0.2, -width - 0.5);
	glVertex3f(-2, -0.2, -width - 0.5);
	glVertex3f(-2, -0.2, -width);
	glVertex3f(-2, 0.2, -width);
	glEnd();
	//Left light
	glBegin(GL_QUADS);
	//Bottom
	glNormal3f(1, -1, 0);
	glVertex3f(1, -0.2, -width - 0.5);
	glVertex3f(2, -0.2, -width - 0.5);
	glVertex3f(2, -0.2, -width);
	glVertex3f(2, -0.2, -width);
	////Top
	glNormal3f(0, 1, 0);
	glVertex3f(1, 0.2, -width -0.5);
	glVertex3f(2, 0.2, -width -0.5);
	glVertex3f(2, 0.2, -width);
	glVertex3f(1, 0.2, -width);
	////Front
	glNormal3f(0, 0, 1);
	glVertex3f(2, 0.2, -width - 0.5);
	glVertex3f(1, 0.2, -width - 0.5);
	glVertex3f(1, -0.2, -width - 0.5);
	glVertex3f(2, -0.2, -width - 0.5);
	////Left
	glNormal3f(-1, 0, 0);
	glVertex3f(2, 0.2, -width - 0.5);
	glVertex3f(2, -0.2, -width - 0.5);
	glVertex3f(2, -0.2, -width);
	glVertex3f(2, 0.2, -width);
	////Right
	glNormal3f(1, 0, 0);
	glVertex3f(1, 0.2, -width - 0.5);
	glVertex3f(1, -0.2, -width - 0.5);
	glVertex3f(1, -0.2, -width);
	glVertex3f(1, 0.2, -width);
	glEnd();

	if (!shadow)
		glColor4f(255, 0, 128,1.f);
	else
		glColor4f(255, 0, 128, alpha);
	//Wheels
	float wheels_distance = 0.5f;
	Pnt3f four_wheels[4] = { 
		Pnt3f(-width - wheels_distance,-height,-width),
		Pnt3f(width + wheels_distance,-height,-width),
		Pnt3f(width + wheels_distance,-height,width),
		Pnt3f(-width - wheels_distance,-height,width)
	};

	glLineWidth(5);	
	glBegin(GL_LINES);
	glVertex3f(-width - wheels_distance,-height,-width);
	glVertex3f(width + wheels_distance,-height,-width);
	glVertex3f(width + wheels_distance,-height,width);
	glVertex3f(-width - wheels_distance,-height,width);
	glEnd();
	float degree = 360.f / 8;
		
	for(Pnt3f w : four_wheels){
		vector<Pnt3f> wheel_points;
		wheel_points.reserve(7);
		glBegin(GL_LINE_LOOP);
		for (size_t i = 1; i < 8; i++) {			
			glVertex3f(w.x , w.y+sin(degree), w.z+cos(degree));						
			degree += 360.f / 8;
			wheel_points.push_back(Pnt3f(w.x, w.y + sin(degree), w.z + cos(degree)));
		}
		glEnd();
		
		for (Pnt3f p : wheel_points) {
			glBegin(GL_LINES);
			glVertex3f(p.x, p.y, p.z);
			glVertex3f(w.x, w.y, w.z);
			glEnd();
		}				
	}	
	if (shadow) 
	{
		glDisable(GL_CULL_FACE);
	}	
	glPopMatrix();

	//Draw nenoroid
	h = 10.f;
	w = 9.f;
	
	QVector<GLfloat> nendoroid_vts;
	nendoroid_vts
		<< -w << h << 2
		<< w << h << 2
		<< w << -h << 2
		<< w << -h << 2
		<< -w << -h << 2
		<< -w << h << 2;
	for (size_t i = 0; i < 12; i++)
	{
		nendoroid_vts << uv[i];
	}
	std::vector<int> buffersize;
	buffersize.push_back(18);
	buffersize.push_back(12);
	glPushMatrix();
	GLfloat mv[16];
	glTranslatef(pos.x, pos.y, pos.z);
	angle = -radiansToDegrees(atan2(orient_cross.z, orient_cross.x));
	glRotatef(angle, 0, 1, 0);
	if (angle > 0)
		angle = -radiansToDegrees(acos(orient.y));
	else
		angle = radiansToDegrees(acos(orient.y));
	glRotatef(-angle, 0, 0, orient.z);
		
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glAlphaFunc(GL_GREATER, 0.1);	
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);		
	nendoroid_front->Begin();
	nendoroid_front->shaderProgram->setUniformValue("tex", 10);
	nendoroid_front->Render(ProjectionMatrex, mv, nendoroid_vts, buffersize,1,1,0);
	nendoroid_front->End();	
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);		
	nendoroid_back->Begin();
	nendoroid_back->shaderProgram->setUniformValue("tex", 11);
	nendoroid_back->Render(ProjectionMatrex, mv, nendoroid_vts, buffersize, 1, 1, 0);
	nendoroid_back->End();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glPopMatrix();
}
