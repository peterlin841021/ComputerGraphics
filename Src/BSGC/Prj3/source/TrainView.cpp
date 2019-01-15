#include <time.h>
#include"TrainView.h"
#include <fstream>

#define VT_WATER false
#define SLEEPER false
#define MIKUPARTS 15
#define TUNNEL 15

int uv_size = 0;
int pos_size = 0;
int normal_size = 0;
int dolphinDir = 1;
bool dirChange = false;
GLuint highmap_textureid = 0;
GLuint normalmap_textureid = 0;
GLuint terrain_textureid = 0;
//FBO
QOpenGLFramebufferObject *fbo;
QOpenGLTexture *fbo_texture;
GLfloat *lightview;
GLfloat lightprojection[16]{ 0 };

void TrainView::changeEffect(size_t effectNum)
{
	this->effectNum = effectNum;	
}
QVector<float> TrainView::unprojectClick()
{
	QVector<float> pos;
	QMatrix4x4 viewMatrix;
	viewMatrix.setToIdentity();
	viewMatrix.lookAt(QVector3D(0, 0, 5), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

	QMatrix4x4 projection;
	projection.setToIdentity();
	projection.perspective(40.0, width() / height(), 0.1, 2000);
	
	QMatrix4x4 viewportMatrix;
	float w2 = width() ;
	float h2 = height();

	viewportMatrix.setToIdentity();
	viewportMatrix.setColumn(0, QVector4D(w2, 0.0f, 0.0f, 0.0f));
	viewportMatrix.setColumn(1, QVector4D(0.0f, h2, 0.0f, 0.0f));
	viewportMatrix.setColumn(2, QVector4D(0.0f, 0.0f, 1.0f, 0.0f));
	viewportMatrix.setColumn(3, QVector4D(w2, h2, 0.0f, 1.0f));

	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();

	QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			modelViewMatrix.data()[i*4+j] = ModelViewMatrex[i * 4 + j];
		}
	}
	modelViewMatrix = modelViewMatrix.transposed();
	QMatrix4x4 modelViewProject = projection * modelViewMatrix;
	QMatrix4x4 inverted = viewportMatrix * modelViewProject;

	inverted = inverted.inverted();

	float posZ;
	float posY = height() - this->mouseYpos - 1.0f;
	
	glReadPixels((int)this->mouseXpos, (int)posY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &posZ);
	QVector3D p(this->mouseXpos, posY, posZ);
	p = p.unproject(modelViewMatrix, projection, QRect(0, 0, width(), height()));
	//qDebug() << 2.0f * posZ - 1.0f;
	//QVector4D clickedPointOnScreen(this->mouseXpos, posY, 0.961635, 1.0f);
	//QVector4D clickedPointIn3DOrgn = inverted * clickedPointOnScreen;
	//clickedPointIn3DOrgn /= clickedPointIn3DOrgn.w();
	//pos << clickedPointIn3DOrgn.x() << clickedPointIn3DOrgn.y() << clickedPointIn3DOrgn.z();
	//printf("Click x/y/z:%f,%f,%f\n",p[0], p[1], p[2]);
	return  pos;
}
struct Model
{
private:
	QVector<GLfloat> values;
	GLuint textureid;
	std::vector<int> bufferoffset;	
public:
	Model()
	{
	
	}
	void setTextureid(GLuint id)
	{
		textureid = id;
	}
	GLuint getTextureid()
	{
		return textureid;
	}
	void setValues(QVector<GLfloat> v)
	{
		values = v;
	}
	QVector<GLfloat> getValues()
	{
		return values;
	}
	void setBufferOffset(std::vector<int> b)
	{
		bufferoffset = b;
	}
	std::vector<int> getBufferOffset()
	{
		return bufferoffset;
	}
};
Model *tire,*airship,*dolphin,*coffeecup;
std::vector<Model*> models;
void loadmodel(string modelname, string texturename, Model *model, QVector<QOpenGLTexture*> *textures);
char *stringToChar(string str);
void generateTextureCube(std::vector<QImage> images, QVector<QOpenGLTexture*> *textures)
{	
	QOpenGLTexture *t = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);	
	t->create();
	t->setSize(images.at(0).width(), images.at(0).height());
	t->setFormat(QOpenGLTexture::RGBAFormat);
	t->allocateStorage();
	t->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	t->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(0).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(1).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(2).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(3).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(4).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(5).bits());
	t->setWrapMode(QOpenGLTexture::ClampToEdge);
	textures->push_back(t);
}
void TrainView::myTimer()
{
	clock_t currentTime = clock();
	float effect_interval = 100.f;
	float model_interval = 100.f;
	if (isrun && (currentTime - train_clock) > train_interval)
	{
		train_clock = currentTime;
		path_index+=train_speed;
		if (path_index >= path.size())
			path_index = 0;
	}
	if ((currentTime - effect_clock) > effect_interval)
	{
		effect_clock = currentTime;
	}
	if ((currentTime - model_clock) > model_interval)
	{
		model_clock = currentTime;
		wholeRotateAngle = (wholeRotateAngle < 360.f) ? wholeRotateAngle += changeAngle : 0.f;
		airshipAngle = (airshipAngle < 360.f) ? airshipAngle += 0.01f : 0.f;		
		if (rightArmAngle < -100.f)
			angleTemp = changeAngle;
		else if (rightArmAngle > 0.f)
			angleTemp = changeAngleOpposite;

		rightArmAngle += angleTemp;		
		shadowShake = (shadowShake < 10.f) ? shadowShake += 1.f : (shadowShake > -10.f) ? shadowShake -= 1.f : shadowShake += 1.f;
		dolphinMove += 1.f*dolphinDir;
		if (dolphinMove > 50)
			dolphinDir = -1;
		else if (dolphinMove < -50)
			dolphinDir = 1;
	}
}
void TrainView::initializeGL()
{		
	trackobj = new Obj();
	trackobj->Init(2);
	trainobj = new Obj();
	trainobj->Init(2);
	miku = new Obj();
	miku->Init(2);
	land = new Obj();
	land->Init(4);
	mountain = new Obj();
	mountain ->Init(4);
	water = new Obj();
	if(VT_WATER)
		water->Init(2);
	else
		water->Init(4);
	skybox = new Obj();
	skybox->Init(2);
	nendoroid = new Obj();
	nendoroid->Init(2);		
	miku3d = new Obj();
	miku3d->Init(2);
	//
	tunnelObj = new Obj();
	tunnelObj->Init(2);
	tireObj = new Obj();
	tireObj->Init(2);
	flyingshipObj = new Obj();
	flyingshipObj->Init(2);
	ferriswheelObj = new Obj();
	ferriswheelObj->Init(2);
	coffeecuplObj = new Obj();
	coffeecuplObj->Init(2);
	initializeTexture();
	Model *mikuhair, *mikuface,
		*mikubody,*mikuskirt,
		*mikuLFM,*mikuLHM,*mikuLH,
		*mikuRFM,*mikuRHM,*mikuRH,
		*mikuLT,*mikuLC,*mikuRT,*mikuRC,
		*scallion,
		*tunnel
		;
	{//Model init
		mikuhair = new Model();
		mikuface = new Model();
		mikubody = new Model();
		mikuskirt = new Model();
		mikuLFM = new Model();
		mikuLHM = new Model();
		mikuLH = new Model();
		mikuRFM = new Model();
		mikuRHM = new Model();
		mikuRH = new Model();
		mikuLT = new Model();
		mikuLC = new Model();
		mikuRT = new Model();
		mikuRC = new Model();
		scallion = new Model();
		//
		tunnel = new Model();
		tire = new Model();
		airship = new Model();
		dolphin = new Model();
		coffeecup = new Model();
	}
	{//Load models
		loadmodel("./src/BSGC/prj3/3dmodel/mikuhair.obj", "./src/BSGC/prj3/3dmodel/mikuhair.png", mikuhair,&Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/mikuface.obj", "./src/BSGC/prj3/3dmodel/mikuface.png", mikuface, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/mikubody.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikubody, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/mikuskirt.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuskirt, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftForeArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLFM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftHindArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLHM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftHand.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLH, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightForeArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRFM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightHindArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRHM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightHand.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRH, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftThigh.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLT, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftCalf.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLC, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightThigh.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRT, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightCalf.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRC, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/scallion.obj", "./src/BSGC/prj3/3dmodel/scallion.png", scallion, &Textures);
		//Tunnel
		loadmodel("./src/BSGC/prj3/3dmodel/tunnel.obj", "./src/BSGC/prj3/3dmodel/tunnel_diffusemap.jpg", tunnel, &Textures);
		//Car tire
		loadmodel("./src/BSGC/prj3/3dmodel/tire.obj", "./src/BSGC/prj3/3dmodel/tire.png", tire, &Textures);
		//Air ship
		loadmodel("./src/BSGC/prj3/3dmodel/airship.obj", "./src/BSGC/prj3/3dmodel/scallion.png", airship, &Textures);				
		//Dolphin
		loadmodel("./src/BSGC/prj3/3dmodel/dolphin.obj", "./src/BSGC/prj3/3dmodel/dolphin.png", dolphin, &Textures);
		//Coffee cup
		loadmodel("./src/BSGC/prj3/3dmodel/coffeecup.obj", "./src/BSGC/prj3/3dmodel/coffeecup.png", coffeecup, &Textures);
	}
	{//Model vector
		models.push_back(mikuhair);
		models.push_back(mikuface);
		models.push_back(mikubody);
		models.push_back(mikuskirt);
		models.push_back(mikuLFM);
		models.push_back(mikuLHM);
		models.push_back(mikuLH);
		models.push_back(mikuRFM);
		models.push_back(mikuRHM);
		models.push_back(mikuRH);
		models.push_back(mikuLT);
		models.push_back(mikuLC);
		models.push_back(mikuRT);
		models.push_back(mikuRC);
		models.push_back(scallion);
		models.push_back(tunnel);
		//models.push_back(tire);
	}	
	fbo = new QOpenGLFramebufferObject(width(), height());
	fbo->setAttachment(QOpenGLFramebufferObject::Depth);
	fbo->bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	fbo->bindDefault();
}
char *stringToChar(string str)
{
	char *c = new char[str.length() + 1];
	return strcpy(c, str.c_str());
}
void loadmodel(string modelname,string texturename, Model *model, QVector<QOpenGLTexture*> *textures)
{
	ifstream in;
	QVector<QVector3D> v,vn,f;
	QVector<QVector2D> vt;
	QVector<GLfloat> model_vts;
	in.open(modelname,ios::in || ios::binary);
	if (!in)
	{
		printf("Can't read file.");
		return;
	}
	char buffer[4096];
	while (in.peek() != EOF)
	{
		in.getline(buffer, 4096);
		char *t = strtok(buffer, " "),*t1,*t2,*t3;
		if (t && strncmp(t, "v", 1) == 0 && strncmp(t, "vt", 2) != 0 && strncmp(t, "vn", 2) != 0)
		{
			float f1 = 0,f2 = 0,f3 = 0;
			t = strtok(NULL, " ");
			f1 = atof(t);
			t = strtok(NULL, " ");
			f2 = atof(t);
			t = strtok(NULL, " ");
			f3 = atof(t);
			v << QVector3D(f1,f2,f3);
		}
		else if (t && strncmp(t, "v", 1) == 0 && strncmp(t, "vt", 2) == 0 && strncmp(t, "vn", 2) != 0)
		{
			float f1 = 0, f2 = 0;
			t = strtok(NULL, " ");
			f1 = atof(t);
			t = strtok(NULL, " ");
			f2 = atof(t);			
			vt << QVector2D(f1, f2);
		}
		else if (t && strncmp(t, "v", 1) == 0 && strncmp(t, "vt", 2) != 0 && strncmp(t, "vn", 2) == 0)
		{
			float f1 = 0, f2 = 0, f3 = 0;
			t = strtok(NULL, " ");
			f1 = atof(t);
			t = strtok(NULL, " ");
			f2 = atof(t);
			t = strtok(NULL, " ");
			f3 = atof(t);
			vn << QVector3D(f1, f2, f3);
		}
		else if (t && strncmp(t, "f", 1) == 0)
		{	
			int i1= 0 , i2 = 0, i3 = 0;
			t1 = strtok(NULL, " ");			
			t2 = strtok(NULL, " ");
			t3 = strtok(NULL, " ");

			t = strtok(t1, "/");
			i1 = atoi(t);
			t = strtok(NULL, "/");
			i2 = atoi(t);
			t = strtok(NULL, "/");
			i3 = atoi(t);
			f << QVector3D(i1,i2,i3);

			t = strtok(t2, "/");
			i1 = atoi(t);
			t = strtok(NULL, "/");
			i2 = atoi(t);
			t = strtok(NULL, "/");
			i3 = atoi(t);
			f << QVector3D(i1, i2, i3);
			
			t = strtok(t3, "/");
			i1 = atoi(t);
			t = strtok(NULL, "/");
			i2 = atoi(t);
			t = strtok(NULL, "/");
			i3 = atoi(t);
			f << QVector3D(i1, i2, i3);
		}
	}
	in.close();
	//Model processing
	for (size_t i = 0; i < f.size(); i++)
	{
		model_vts << v[f[i].x()-1].x() << v[f[i].x()-1].y() << v[f[i].x()-1].z();
	}
	pos_size = model_vts.size();
	for (size_t i = 0; i < f.size(); i++)
	{
		model_vts << vt[f[i].y()-1].x() << vt[f[i].y()-1].y();
	}
	uv_size = (model_vts.size() - pos_size);
	for (size_t i = 0; i < f.size(); i++)
	{
		model_vts << vn[f[i].z() - 1].x() << vn[f[i].z() - 1].y() << vn[f[i].z() - 1].z();;
	}
	normal_size = (model_vts.size() - pos_size - uv_size);
	model->setValues(model_vts);
	QOpenGLTexture *tex = new QOpenGLTexture(QImage(stringToChar(texturename)));
	tex->setMinificationFilter(QOpenGLTexture::Linear);
	tex->setMagnificationFilter(QOpenGLTexture::Linear);
	tex->setWrapMode(QOpenGLTexture::ClampToEdge);
	model->setTextureid(textures->size());
	textures->push_back(tex);
	std::vector<int> bufferoffset;
	bufferoffset.push_back(pos_size);
	bufferoffset.push_back(uv_size);
	bufferoffset.push_back(normal_size);
	model->setBufferOffset(bufferoffset);	
}
void TrainView::initializeTexture()
{	
	miku->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/miku_transparent.png")));
	land->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/underwater.jpg")));
	water->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/water.png")));	
	std::vector<QImage> skybox_imgs;
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_front.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_back.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_left.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_right.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_top.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_bottom.jpg"));
	generateTextureCube(skybox_imgs,&Textures);
	skybox->textureId = Textures.size()-1;	
	nendoroid->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/nendoroid_front.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/nendoroid_back.png")));
	//Highmap
	highmap_textureid = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/heightmap.png")));
	normalmap_textureid = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/water_normal.jpg")));
	terrain_textureid = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/terrain.jpg")));
	mountain->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/mountain.jpg")));
	trackobj->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train1.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train2.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train3.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train4.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train5.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train6.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train7.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/black.png")));	
	ferriswheelObj->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/ferriswheel.png")));	
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
	//train_interval = 1000.f - 999.f * (float)(speed/100.f);
	train_speed = 10 * (float)(speed / 100.f);
	if (speed > 70 && speed < 85)
		train_effect = 5;
	else if(speed > 85)
		train_effect = 4;
	else
		train_effect = 0;
}
void TrainView::paint()
{
	//Update time
	myTimer();
	glEnable(GL_DEPTH);
	if (useFBO)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, 1000, 1000);
		fbo->bind();		
		glMatrixMode(GL_PROJECTION);
		glOrtho(-15.f,15.f,-15.f,15.f,0.1f,50.f);		
		gluLookAt(30.f, 50.f, 560.f,0,0,0,0,1,0);
		//glGetFloatv(GL_MODELVIEW_MATRIX, lightview);
		QMatrix4x4 m;
		m.lookAt(QVector3D(30.f, 50.f, 560.f), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
		lightview = m.data();
		glGetFloatv(GL_PROJECTION_MATRIX, lightprojection);
	}
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (this->camera == 1)
	{
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else 
	{
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };
	std::vector<int> buffer_size;
	float boxsize = 1500;

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
		
	//All texture	
	for (size_t i = 0; i < Textures.size(); i++)
	{
		Textures[i]->bind(i);
	}
	//Draw skybox
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(0, 700, 0);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);	
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	float skyboxVertices[] =
	{
		/*//////positions//////*/
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	for (size_t i = 0; i < sizeof(skyboxVertices) / sizeof(float); i++)
	{
		skyboxVertices[i] *= boxsize;
	}
	int vt_size = sizeof(skyboxVertices) / sizeof(float);
	QVector<GLfloat> skybox_vts;
	skybox->Begin();

	skybox->shaderProgram->setUniformValue("texcube", skybox->textureId);

	for (size_t i = 0; i < vt_size; i++)
	{
		skybox_vts << skyboxVertices[i];
	}

	buffer_size.push_back(vt_size);	
	skybox->Render(GL_TRIANGLES,false,1,ProjectionMatrex, ModelViewMatrex, skybox_vts, buffer_size, 1.f, effect_clock,(effectNum == 0?2: effectNum));
	skybox_vts.clear();
	buffer_size.clear();
	skybox->End();
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	//Draw track and train
	glPushMatrix();
	drawStuff(false);
	glPopMatrix();

	//Draw land
	float horizon = -250.f;
	glPushMatrix();
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	land->Begin();
	land->shaderProgram->setUniformValue("tex", land->textureId);
	land->shaderProgram->setUniformValue("heightmap", terrain_textureid);
	land->shaderProgram->setUniformValue("camerapos", QVector3D(arcball.posx, arcball.posy, arcball.posz));
	QVector<GLfloat> land_vts;
	land_vts
		<< -boxsize << horizon << -boxsize
		<< boxsize << horizon << -boxsize
		<< boxsize << horizon << boxsize
		<< -boxsize << horizon << boxsize;
	buffer_size.clear();
	buffer_size.push_back(12);	
	land->Render(GL_PATCHES,true,0,ProjectionMatrex, ModelViewMatrex, land_vts, buffer_size, 1.f, effect_clock, effectNum);
	land->End();
	glPopMatrix();

	//Draw water
	glPushMatrix();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		float wave_t = 0;
		int water_size = 70;
		float min = -boxsize;
		float amplitude = 2.5f;
		float speed = 0.0001f;
		float step = boxsize / water_size * 2;
		float old_wave_height = 0;
		float wy = 0.f;
		float wave_height = 0.f;

		setupFloor();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		water->Begin();
		water->shaderProgram->setUniformValue("tex", water->textureId);
		water->shaderProgram->setUniformValue("heightmap", highmap_textureid);	
		water->shaderProgram->setUniformValue("texcube", skybox->textureId);
		water->shaderProgram->setUniformValue("camerapos", QVector3D(arcball.posx, arcball.posy, arcball.posz));

		float ratio = 25;
		float xfrom = 0;
		float zfrom = 0;
		float xto = 0;
		float zto = 0;
		float padding = 0;

		QVector<GLfloat> water_vertices;
		water_vertices.clear();
		buffer_size.clear();
		water_vertices
			<< min << wy << min
			<< -min << wy << min
			<< -min << wy << -min
			<< min << wy << -min;
		buffer_size.push_back(12);
		buffer_size.push_back(0);
		water->Render(GL_PATCHES,true,2,ProjectionMatrex, ModelViewMatrex, water_vertices, buffer_size, 0.7, effect_clock*0.001f, effectNum);
		water->End();
		water_vertices.clear();
		buffer_size.clear();
	glPopMatrix();
	glDisable(GL_BLEND);
	//Draw shadows
	glPushMatrix();
		if (this->camera != 1)
		{
			glTranslatef(0, shadowShake, 0);
			setupShadows();
			drawStuff(true);
			unsetupShadows();		
		}
	glPopMatrix();
	glPushMatrix();
		//Draw mountains
		float mountain_h = -140.f;
		float minx = -160.f;
		float minz = -100.f;			
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		mountain->Begin();
		mountain->shaderProgram->setUniformValue("tex", mountain->textureId);
		mountain->shaderProgram->setUniformValue("heightmap", terrain_textureid);
		mountain->shaderProgram->setUniformValue("camerapos", QVector3D(arcball.posx, arcball.posy, arcball.posz));
		QVector<GLfloat> mountain_vts;
		mountain_vts
			<< minx << mountain_h << minz
			<< minx + 400 << mountain_h << minz
			<< minx + 400 << mountain_h << minz + 400
			<< minx << mountain_h << minz + 400;
		buffer_size.clear();
		buffer_size.push_back(12);
		buffer_size.push_back(0);
		mountain->Render(GL_PATCHES,true,0,ProjectionMatrex,ModelViewMatrex, mountain_vts, buffer_size, 1.f, effect_clock, effectNum);
		mountain->End();
	glPopMatrix();
		glLineWidth(5);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		ferriswheelObj->Begin();
		ferriswheelObj->shaderProgram->setUniformValue("tex", ferriswheelObj->textureId);
		minx = 50.f;
		minz = 100.f;
		float ferriswheelHeight = 100.f;
		float ferriswheelGround = 30.f;
		float wheelR = 60.f;
		float width = 50.f;
		QVector<GLfloat> ferriswheel_vts;
		ferriswheel_vts
			<< 0 << ferriswheelHeight << minz
			<< minx << ferriswheelGround << minz
			<< -minx << ferriswheelGround << minz
			<< 0 << ferriswheelHeight << minz + width
			<< minx << ferriswheelGround << minz + width
			<< -minx << ferriswheelGround << minz + width;
		ferriswheel_vts
			<< 0 << 0.25
			<< 1 << 0.25
			<< 1 << 0
			<< 1 << 0
			<< 0 << 0
			<< 0 << 0.25;
		buffer_size.clear();
		buffer_size.push_back(18);
		buffer_size.push_back(12);
		ferriswheelObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, ferriswheel_vts, buffer_size, 1.f, effect_clock, effectNum);
		//Front
		ferriswheel_vts.clear();
		ferriswheel_vts
			<< 0 << ferriswheelHeight << minz+width
			<< minx << ferriswheelGround << minz+width
			<< minx << ferriswheelGround << minz+width
			<< -minx << ferriswheelGround << minz+width
			<< -minx << ferriswheelGround << minz+width
			<< 0 << ferriswheelHeight << minz+width
			//Connect
			<< 0 << ferriswheelHeight << minz+width
			<< 0 << ferriswheelHeight << minz
			<< minx << ferriswheelGround << minz+width
			<< minx << ferriswheelGround << minz
			<< -minx << ferriswheelGround << minz+width
			<< -minx << ferriswheelGround << minz;
		ferriswheel_vts
			<< 0 << 0.25
			<< 1 << 0.25
			<< 1 << 0
			<< 1 << 0
			<< 0 << 0
			<< 0 << 0.25
			<< 0 << 0
			<< 0 << 0.25
			<< 0 << 0
			<< 0 << 0.25
			<< 0 << 0
			<< 0 << 0.25;
		buffer_size.clear();
		buffer_size.push_back(24);
		buffer_size.push_back(18);
		ferriswheelObj->Render(GL_LINES, false, 0, ProjectionMatrex, ModelViewMatrex, ferriswheel_vts, buffer_size, 1.f, effect_clock, effectNum);		
		ferriswheel_vts.clear();
		//Wheel
		ferriswheel_vts.clear();
		buffer_size.clear();
		//ROTATE		
		glTranslatef(0,ferriswheelHeight,0);
		glRotatef(airshipAngle*100,0,0,1);		
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		float centerX = 0;
		float centerY = 0;
		float degree = 45.f;
		Pnt3f last = Pnt3f(centerX + wheelR * sin(0), centerY + wheelR * cos(0),minz);
		for (int i = 0; i < (360/(int)degree); i++)
		{
			ferriswheel_vts
				<< centerX << centerY << minz
				<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz
				<< centerX << centerY << minz + width + 5
				<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz + width+5
				<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz
				<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz +width + 5
				<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz
				<< last.x << last.y << last.z
				<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz + width + 5
				<< last.x << last.y << last.z + width + 5;
			last = Pnt3f(centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)), centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)), minz);
		}
		ferriswheel_vts
			<< last.x << last.y << last.z
			<< centerX + wheelR * sin(0) << centerY + wheelR * cos(0) << minz
			<< last.x << last.y << last.z + width + 5
			<< centerX + wheelR * sin(0) << centerY + wheelR * cos(0) << minz + width + 5;
		for (int i = 0; i < (360 / (int)degree)*10; i++)
		{
			ferriswheel_vts
				<< 0 << 0.5
				<< 1 << 0.5;
		}
		ferriswheel_vts
			<< 0 << 0.5
			<< 1 << 0.5
			<< 0 << 0.5
			<< 1 << 0.5;
		buffer_size.push_back(3* ((360 / (int)degree)*10+4));
		buffer_size.push_back(2* ((360 / (int)degree)*10+4));
		ferriswheelObj->Render(GL_LINES, false, 0, ProjectionMatrex, ModelViewMatrex, ferriswheel_vts, buffer_size, 1.f, effect_clock, effectNum);
		////Box
		//ferriswheel_vts.clear();
		//buffer_size.clear();		
		//glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		//float carW = 20.f;
		//for (int i = 0; i < (360 / (int)degree); i++)
		//{
		//	ferriswheel_vts
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) + carW << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz				
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) + carW << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) - carW << minz
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) - carW << minz
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz + width
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) + carW << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) << minz + width
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) + carW << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) - carW << minz + width
		//		<< centerX + wheelR * sin(2 * 3.14f*i / (360 / (int)degree)) << centerY + wheelR * cos(2 * 3.14f*i / (360 / (int)degree)) - carW << minz + width;
		//}
		//for (int i = 0; i < (360 / (int)degree)*2; i++)
		//{
		//	ferriswheel_vts
		//		<< 0 << 0.25
		//		<< 1 << 0.25
		//		<< 1 << 0
		//		<< 0 << 0;
		//}
		//buffer_size.push_back(3 * ((360 / (int)degree) * 8));
		//buffer_size.push_back(2 * ((360 / (int)degree) * 8));
		//ferriswheelObj->Render(GL_QUADS, false, 0, ProjectionMatrex, ModelViewMatrex, ferriswheel_vts, buffer_size, 1.f, effect_clock, effectNum);
		ferriswheelObj->End();
	glPushMatrix();
	//Draw ferris wheel
		
	glPopMatrix();
	/*fbo->bindDefault();
	
	if (useFBO)
	{
		fbo->release();
		fbo_texture = new QOpenGLTexture(fbo->toImage());

	}*/	
}
void TrainView::paintGL()
{	
	glClearColor(0, 0, 0, 0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	
	//useFBO = true;
	//paint();
	glViewport(0, 0, width(), height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();
	//unprojectClick();
	//Draw shadows
	/*QVector<GLfloat> fbo_vts;
	std::vector<int>buffer_size;
	float frame_size = 200.f;
	fbo_texture->bind(0);
	glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		miku->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		fbo_vts.clear();
		fbo_vts
			<< -frame_size << -frame_size << 300
			<< frame_size << -frame_size << 300
			<< frame_size << frame_size << 300
			<< frame_size << frame_size << 300
			<< -frame_size << frame_size << 300
			<< -frame_size << -frame_size << 300;
		buffer_size.clear();
		buffer_size.push_back(18);
		fbo_vts
			<< 0 << 1
			<< 1 << 1
			<< 1 << 0
			<< 1 << 0
			<< 0 << 0
			<< 0 << 1;
		buffer_size.push_back(12);
		miku->shaderProgram->setUniformValue("tex", 0);
		miku->Render(GL_TRIANGLES, false, 0, lightprojection, lightview, fbo_vts, buffer_size, 1, effect_clock, effectNum);
		
		miku->End();
	glPopMatrix();*/
	useFBO = false;
	paint();
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
			glMatrixMode(GL_PROJECTION);
			gluPerspective(120, 1, 1, 4000);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			Pnt3f dec = trainEnd - trainStart;
			dec.normalize();
			Pnt3f pos = path[path_index].points;
			Pnt3f orient_tt = path[path_index].orients;
			gluLookAt(pos.x, pos.y + 35.0, pos.z,
				pos.x + dec.x, pos.y +35.0, pos.z + dec.z,
				orient_tt.x, orient_tt.y, orient_tt.z);
			update();
		}		
		
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif		
		
	}
}
void TrainView::drawTrack(bool doingShadows) 
{
	float alpha = 0.3f;
	QVector<GLfloat> tracks,sleepers;
	std::vector<int> buffer_size;
	spline_t spline_type = (spline_t)curve;
	Pnt3f qt0, qt, orient_t, or0, or1;		
	track_path.clear();
	track_orient_cross.clear();
	track_orient.clear();
	Pnt3f track_start_inside, track_start_outside, track_end_inside, track_end_outside;
	Pnt3f temp;
	std::vector<Pnt3f> connect_head,connect_tail;
	for (size_t i = 0; i < m_pTrack->points.size(); i++) 
	{
		Pnt3f points1 = m_pTrack->points[i].pos;
		Pnt3f points2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		Pnt3f orient1 = m_pTrack->points[i].orient;
		Pnt3f orient2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		
		float distance = sqrt(pow(points1.x - points2.x, 2) + pow(points1.y - points2.y, 2) + pow(points1.z - points2.z, 2));
		unsigned int divide = distance / 8.f;
		if (divide < 4)
		{
			divide = 4;
		}
		ControlPoint q_matrix[4];		
		
		if (i == 0)
		{
			q_matrix[0] = m_pTrack->points[m_pTrack->points.size() - 1];
			q_matrix[1] = m_pTrack->points[0];
		}
		else if (i == 1)
		{
			q_matrix[0] = m_pTrack->points[0];
			q_matrix[1] = m_pTrack->points[i];
		}
		else
		{
			q_matrix[0] = m_pTrack->points[i - 1];
			q_matrix[1] = m_pTrack->points[i];
		}		
		q_matrix[2] = m_pTrack->points[(i + 1) % m_pTrack->points.size()];
		q_matrix[3] = m_pTrack->points[(i + 2) % m_pTrack->points.size()];
		
		float percent = 1.f / divide;
		float t = 0.f - percent;
		if (spline_type == spline_CardinalCubic) 
		{			
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
			if (j == 0)
			{				
				qt0 = qt0;
			}				
			else
			{
				qt0 = qt;
			}
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
			
			Pnt3f cross_t0, cross_t1;
			float track_width = 5.f;
			cross_t0 = Pnt3f(qt.x - qt0.x, qt.y - qt0.y, qt.z - qt0.z) * or0;
			cross_t1 = Pnt3f(qt.x - qt0.x, qt.y - qt0.y, qt.z - qt0.z) * or1;
			cross_t0.normalize();
			cross_t1.normalize();
			cross_t0 = cross_t0 * track_width;//Track width
			
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
					tracks
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z
						<< connect_inside.x << connect_inside.y << connect_inside.z
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z
						<< connect_outside.x << connect_outside.y << connect_outside.z;					
				}
				else if(j == 1 && spline_type == spline_Linear)
				{					
					tracks
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z
						<< connect_inside.x << connect_inside.y << connect_inside.z
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z
						<< connect_outside.x << connect_outside.y << connect_outside.z;					
				}
				else if ( (j != divide && j > 1) && spline_type == spline_Linear)
				{					
					tracks
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z
						<< connect_inside.x << connect_inside.y << connect_inside.z
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z
						<< connect_outside.x << connect_outside.y << connect_outside.z;					
				}							
				if (j == 1)
				{					
					if (spline_type != spline_Linear)
					{
						connect_head.push_back(Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z));
						connect_head.push_back(Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z));
					}
					else
					{
						connect_head.push_back(connect_inside);
						connect_head.push_back(connect_outside);
					}										
				}
				if (j == divide)
				{
					if (spline_type != spline_Linear)
					{
						connect_tail.push_back(Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z));
						connect_tail.push_back(Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z));
					}
					else
					{
						connect_tail.push_back(connect_inside);
						connect_tail.push_back(connect_outside);
					}					
				}
				
				//Sleepers
				if (j == divide && spline_type == spline_Linear)
				{

				}
				else
				{					
					sleepers
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z					
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z;
				}				
				connect_inside = Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
				connect_outside = Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
			}
			else if (track == 0)
			{
				glLineWidth(5);
				glBegin(GL_LINES);
				glVertex3f(qt0.x, qt0.y, qt0.z);
				glVertex3f(qt.x, qt.y, qt.z);
				glEnd();
			}
			if (j > 0) 
			{
				track_path.push_back(Pnt3f(qt.x, qt.y, qt.z));
				track_orient_cross.push_back(cross_t0);
				track_orient.push_back(orient_t);
			}			
		}//Divide			
	}//Controlpoints
	
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	
	buffer_size.push_back(tracks.size());
	int color_counts = tracks.size();
	for (size_t i = 0; i < color_counts; i++)
	{
		tracks << 0;
	}
	buffer_size.push_back(color_counts);//Colors
	trackobj->Begin();
	if (!doingShadows)
		trackobj->Render(GL_TRIANGLES,false,0,ProjectionMatrex, ModelViewMatrex, tracks, buffer_size,1.f,effect_clock,(effectNum == 0?3: effectNum));
	else
		trackobj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, tracks, buffer_size, 0.4f, effect_clock, effectNum);
	trackobj->End();
	buffer_size.clear();
	tracks.clear();
	//Draw connects
	if (track != 0)
	{
		for (size_t i = 0; i < connect_tail.size() - 1; i += 2)
		{
			tracks
				<< connect_tail[i].x << connect_tail[i].y << connect_tail[i].z
				<< connect_head[(i + 2) % connect_head.size()].x << connect_head[(i + 2) % connect_head.size()].y << connect_head[(i + 2) % connect_head.size()].z
				<< connect_tail[i + 1].x << connect_tail[i + 1].y << connect_tail[i + 1].z
				<< connect_head[(i + 3) % connect_head.size()].x << connect_head[(i + 3) % connect_head.size()].y << connect_head[(i + 3) % connect_head.size()].z;
		}
		buffer_size.push_back(tracks.size());
		color_counts = tracks.size();
		for (size_t i = 0; i < color_counts; i++)
		{
			tracks << 0;
		}
		buffer_size.push_back(color_counts);
		trackobj->Begin();
		if (!doingShadows)
			trackobj->Render(GL_LINES,false,0,ProjectionMatrex, ModelViewMatrex, tracks, buffer_size, 1.f, effect_clock,(effectNum == 0?3: effectNum));
		else
			trackobj->Render(GL_LINES, false, 0,ProjectionMatrex, ModelViewMatrex, tracks, buffer_size, 0.4f, effect_clock, effectNum);
		trackobj->End();

		//Draw sleepers
		buffer_size.clear();
		color_counts = sleepers.size();
		buffer_size.push_back(sleepers.size());
		for (size_t i = 0; i < color_counts; i++)
		{
			sleepers << 1.f;
		}
		buffer_size.push_back(color_counts);
		trackobj->Begin();
		if (SLEEPER)
		{
			if (!doingShadows)
				trackobj->Render(GL_LINES, false, 0,ProjectionMatrex, ModelViewMatrex, sleepers, buffer_size, 1.f, effect_clock, effectNum);
			else
				trackobj->Render(GL_LINES, false, 0,ProjectionMatrex, ModelViewMatrex, sleepers, buffer_size, 0.4f, effect_clock, effectNum);
			trackobj->End();
		}
	}
	
	//Generate path	
	if (trackUpdate)
	{
		path.clear();
		size_t divide = 10;
		for (size_t i = 0; i < track_path.size(); i++)
		{
			TrackTrail t1 = TrackTrail(track_path[i], track_orient[i], track_orient_cross[i]);
			TrackTrail t2 = TrackTrail(track_path[(i + 1) % track_path.size()], track_orient[(i + 1) % track_orient.size()], track_orient_cross[(i + 1) % track_orient_cross.size()]);
			Pnt3f pos_divided = t2.points - t1.points;
				pos_divided.x /= (float)divide;
				pos_divided.y /= (float)divide;
				pos_divided.z /= (float)divide;
			Pnt3f orient_divided = t2.orients - t1.orients;
				orient_divided.x /= (float)divide;
				orient_divided.y /= (float)divide;
				orient_divided.z /= (float)divide;
			Pnt3f cross_divided = t2.orients_cross - t1.orients_cross;
				cross_divided.x /= (float)divide;
				cross_divided.y /= (float)divide;
				cross_divided.z /= (float)divide;
			for (size_t j = 1; j < divide; j++)
			{
				TrackTrail t3 = TrackTrail(t1.points+ pos_divided*j, t1.orients+ orient_divided * j,t1.orients_cross+ cross_divided * j);
				path.push_back(t3);
			}
			//path.push_back(t1);
		}
		trackUpdate = false;
	}
}
void TrainView::drawStuff(bool doingShadows)
{
	draw3DObj(doingShadows);
	//glPushMatrix();		
		glTranslatef(0,10,0);
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
		if (path.size() > 0) 
		{
			int distance = 20;
			//RED
			glPushMatrix();
				drawTrain(path[path_index].points, path[path_index].orients_cross, path[path_index].orients,doingShadows, false, trackobj->textureId);
			glPopMatrix();
			//ORANGE
			glPushMatrix();			
				if(path_index < distance)
					drawTrain(path[path.size() - (distance -path_index)].points, path[path.size() - (distance - path_index)].orients_cross, path[path.size() - (distance - path_index)].orients, doingShadows,true,trackobj->textureId+1);
				else
					drawTrain(path[path_index- distance].points, path[path_index- distance].orients_cross, path[path_index- distance].orients, doingShadows, true, trackobj->textureId+1);	
			glPopMatrix();
			distance += 20;
			//YELLOW
			glPushMatrix();
			if (path_index < distance)
				drawTrain(path[path.size() - (distance - path_index)].points, path[path.size() - (distance - path_index)].orients_cross, path[path.size() - (distance - path_index)].orients, doingShadows, true, trackobj->textureId + 2);
			else
				drawTrain(path[path_index - distance].points, path[path_index - distance].orients_cross, path[path_index - distance].orients, doingShadows, true, trackobj->textureId + 2);
			glPopMatrix();
			distance += 20;
			//GREEN
			glPushMatrix();
			if (path_index < distance)
				drawTrain(path[path.size() - (distance - path_index)].points, path[path.size() - (distance - path_index)].orients_cross, path[path.size() - (distance - path_index)].orients, doingShadows, true, trackobj->textureId + 3);
			else
				drawTrain(path[path_index - distance].points, path[path_index - distance].orients_cross, path[path_index - distance].orients, doingShadows, true, trackobj->textureId + 3);
			glPopMatrix();
			//BLUE
			distance += 20;
			glPushMatrix();
			if (path_index < distance)
				drawTrain(path[path.size() - (distance - path_index)].points, path[path.size() - (distance - path_index)].orients_cross, path[path.size() - (distance - path_index)].orients, doingShadows, true, trackobj->textureId + 4);
			else
				drawTrain(path[path_index - distance].points, path[path_index - distance].orients_cross, path[path_index - distance].orients, doingShadows, true, trackobj->textureId + 4);
			glPopMatrix();
			//INDIGO
			distance += 20;
			glPushMatrix();
			if (path_index < distance)
				drawTrain(path[path.size() - (distance - path_index)].points, path[path.size() - (distance - path_index)].orients_cross, path[path.size() - (distance - path_index)].orients, doingShadows, true, trackobj->textureId + 5);
			else
				drawTrain(path[path_index - distance].points, path[path_index - distance].orients_cross, path[path_index - distance].orients, doingShadows, true, trackobj->textureId + 5);
			glPopMatrix();
			//PURPLE
			distance += 20;
			glPushMatrix();
			if (path_index < distance)
				drawTrain(path[path.size() - (distance - path_index)].points, path[path.size() - (distance - path_index)].orients_cross, path[path.size() - (distance - path_index)].orients, doingShadows, true, trackobj->textureId + 6);
			else
				drawTrain(path[path_index - distance].points, path[path_index - distance].orients_cross, path[path_index - distance].orients, doingShadows, true, trackobj->textureId + 6);
			glPopMatrix();						
		}
	//glPopMatrix();	
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif	
}
void TrainView::draw3DObj(bool doingShadows)
{
	//**********************OTHERS***************************//	
	float right_position[15][3]
	{
		{0, 2.3f, 0},
		{0, 2.5f, 0},

		{0, 0, 0},
		{0, -0.1f, 0},

		{0.5f, 1.8f, 0},
		{1.4f, -0.7f, 0},
		{1.4f, -0.7f, 0},

		{-0.5f, 1.8f, 0},
		{-1.4f, -0.7f, 0},
		{-1.4f, -0.7f, 0},

		{0.5f, -0.3f, 0},
		{-0.05f, -2.2f, 0},

		{-0.5f, -0.3f, 0},
		{0.05f, -2.2f, 0},

		{0, 0, 0}
	};
	float sr = 20;	
	for (size_t i = 0; i < models.size() - 1; i++)
	{
		right_position[i][0] *= sr;
		right_position[i][1] *= sr;
		right_position[i][2] *= sr;
	}
	//Draw 3d models reflect environment map
	{
		glPushMatrix();
		QMatrix4x4 m;
		m.setToIdentity();
		glTranslatef(0, 130, 0);
		glRotatef(wholeRotateAngle, 0, 1, 0);
		if(doingShadows)
			glScalef(1, 1, -1);
		else
			glScalef(1, 1, 1);
		
		m.translate(QVector3D(0, 130, 0));
		m.rotate(wholeRotateAngle, QVector3D(0, 1, 0));
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		miku3d->Begin();
		for (size_t i = 0; i < MIKUPARTS; i++)
		{
			glPushMatrix();
			switch (i)
			{
			case 0://hair
				glTranslatef(right_position[0][0], right_position[0][1], right_position[0][2]);
				m.translate(QVector3D(right_position[0][0], right_position[0][1], right_position[0][2]));
				break;
			case 1:
				glTranslatef(right_position[1][0], right_position[1][1], right_position[1][2]);
				m.translate(QVector3D(right_position[1][0], right_position[1][1], right_position[1][2]));
			case 3://Skirt
				glTranslatef(right_position[3][0], right_position[3][1], right_position[3][2]);
				m.translate(QVector3D(right_position[3][0], right_position[3][1], right_position[3][2]));
				break;
			case 4:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				m.translate(QVector3D(right_position[4][0], right_position[4][1], right_position[4][2]));
				break;
			case 5:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				m.translate(QVector3D(right_position[4][0], right_position[4][1], right_position[4][2]));
				m.translate(QVector3D(right_position[5][0], right_position[5][1], right_position[5][2]));
				break;
			case 6:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				glTranslatef(right_position[6][0], right_position[6][1], right_position[6][2]);
				m.translate(QVector3D(right_position[4][0], right_position[4][1], right_position[4][2]));
				m.translate(QVector3D(right_position[5][0], right_position[5][1], right_position[5][2]));
				m.translate(QVector3D(right_position[6][0], right_position[6][1], right_position[6][2]));
				break;
			case 7:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				break;
			case 8:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				m.translate(QVector3D(right_position[8][0], right_position[8][1], right_position[8][2]));
				break;
			case 9:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				m.translate(QVector3D(right_position[8][0], right_position[8][1], right_position[8][2]));
				m.translate(QVector3D(right_position[9][0], right_position[9][1], right_position[9][2]));
				break;
			case 10://Thigh
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				m.translate(QVector3D(right_position[10][0], right_position[10][1], right_position[10][2]));
				break;
			case 11:
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				glTranslatef(right_position[11][0], right_position[11][1], right_position[11][2]);
				m.translate(QVector3D(right_position[10][0], right_position[10][1], right_position[10][2]));
				m.translate(QVector3D(right_position[11][0], right_position[11][1], right_position[11][2]));
				break;
			case 12:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				m.translate(QVector3D(right_position[12][0], right_position[12][1], right_position[12][2]));
				break;
			case 13:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				glTranslatef(right_position[13][0], right_position[13][1], right_position[13][2]);
				m.translate(QVector3D(right_position[12][0], right_position[12][1], right_position[12][2]));
				m.translate(QVector3D(right_position[13][0], right_position[13][1], right_position[13][2]));
				break;
			case 14:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				m.translate(QVector3D(right_position[8][0], right_position[8][1], right_position[8][2]));
				m.translate(QVector3D(right_position[9][0], right_position[9][1], right_position[9][2]));
				break;
			}
			glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
			miku3d->shaderProgram->setUniformValue("tex", models[i]->getTextureid());
			miku3d->shaderProgram->setUniformValue("texcube", skybox->textureId);
			if (i != 14)
			{
				if (doingShadows)
					miku3d->Render(GL_TRIANGLES, false, 3, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 0.6f, effect_clock, effectNum);
				else
					miku3d->Render(GL_TRIANGLES, false, 3, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 0.3f, effect_clock, effectNum);
			}				
			glPopMatrix();
		}
		miku3d->End();
		glPopMatrix();
	}
	//Draw 3d models
	{
		glPushMatrix();
		glTranslatef(-140, 130, 0);
				
		glRotatef(90.f, 0, 1, 0);
		if(doingShadows)
			glScalef(1, -1, 1);
		else
			glScalef(1, 1, 1);
			
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		miku3d->Begin();
		for (size_t i = 0; i < MIKUPARTS; i++)
		{
			glPushMatrix();
			switch (i)
			{
			case 0://hair
				glTranslatef(right_position[0][0], right_position[0][1], right_position[0][2]);
				break;
			case 1:
				glTranslatef(right_position[1][0], right_position[1][1], right_position[1][2]);
			case 3://Skirt
				glTranslatef(right_position[3][0], right_position[3][1], right_position[3][2]);
				break;
			case 4://LFA
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glRotatef(-30.f, 0, 0, 1);
				break;
			case 5:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glRotatef(-30.f, 0, 0, 1);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				break;
			case 6:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glRotatef(-30.f, 0, 0, 1);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				glTranslatef(right_position[6][0], right_position[6][1], right_position[6][2]);
				break;
			case 7://RFA
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				break;
			case 8:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				break;
			case 9:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				break;
			case 10://Thigh
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				break;
			case 11:
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				glTranslatef(right_position[11][0], right_position[11][1], right_position[11][2]);
				break;
			case 12:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				break;
			case 13:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				glTranslatef(right_position[13][0], right_position[13][1], right_position[13][2]);
				break;
			case 14:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				break;
			}
			glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
			miku3d->shaderProgram->setUniformValue("tex", models[i]->getTextureid());
			if (!doingShadows)
				miku3d->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 1.f, effect_clock, effectNum);
			else
				miku3d->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 0.3f, effect_clock, effectNum);
			glPopMatrix();
		}
		miku3d->End();
		glPopMatrix();
	}
	//3D tunnel
	{
		glPushMatrix();
		glTranslatef(270.f, 45.f, 40.f);
		glRotatef(130.f,0,1,0);
		glRotatef(-10.f, 1, 0, 0);
		if (doingShadows)
			glScalef(0.2f, 0.2f, 0.2f);
		else
			glScalef(0.2f, 0.2f, 0.4f);		
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		tunnelObj->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		tunnelObj->shaderProgram->setUniformValue("tex", models[TUNNEL]->getTextureid());
		if (!doingShadows)
			tunnelObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[TUNNEL]->getValues(), models[TUNNEL]->getBufferOffset(), 1.f, effect_clock, effectNum);
		else
			tunnelObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[TUNNEL]->getValues(), models[TUNNEL]->getBufferOffset(), 0.3f, effect_clock, effectNum);
		tunnelObj->End();
		glPopMatrix();
	}
	//**********************OTHERS***************************//
	//Air ship
	{
		glPushMatrix();
		float r = 200.f;
		glTranslatef(r*cos(airshipAngle), 250.f, r*sin(airshipAngle));
		glRotatef(cos(airshipAngle) * 90, 0, 1, 0);		

		if (doingShadows)
			glScalef(0.5, 0.5, 0.5);
		else
			glScalef(0.5, 0.5, 0.5);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		flyingshipObj->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		flyingshipObj->shaderProgram->setUniformValue("tex", airship->getTextureid());
		if (!doingShadows)
			flyingshipObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, airship->getValues(), airship->getBufferOffset(), 1.f, effect_clock, effectNum);
		else
			flyingshipObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, airship->getValues(), airship->getBufferOffset(), 0.3f, effect_clock, effectNum);
		flyingshipObj->End();
		glPopMatrix();
	}
	//Dolphin
	{
		glPushMatrix();		
		float r = 300.f;
		glTranslatef(r*cos(airshipAngle), -60.f, r*sin(airshipAngle));
		glRotatef(cos(airshipAngle)*500, 0, 1, 0);
		if (doingShadows)
			glScalef(5, 5, 5);
		else
			glScalef(5, 5, 5);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		flyingshipObj->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		flyingshipObj->shaderProgram->setUniformValue("tex", dolphin->getTextureid());
		if (!doingShadows)
			flyingshipObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, dolphin->getValues(), dolphin->getBufferOffset(), 1.f, effect_clock, effectNum);
		else
			flyingshipObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, dolphin->getValues(), dolphin->getBufferOffset(), 0.3f, effect_clock, effectNum);
		flyingshipObj->End();
		glPopMatrix();
	}
	//Coffee cups
	{
		float centerX = 100.f;
		float centerZ = 200.f;
		float R = 50.f;
		float degree = 72.f;
		//srand(time(NULL));
		//int random =  0;		
		for (int i = 0; i < (360 / (int)degree); i++)
		{
			glPushMatrix();				
				glTranslatef(centerX+R * sin(2 * 3.14f*i / (360 / (int)degree)), 30.f, centerZ + R * cos(2 * 3.14f*i / (360 / (int)degree)));
				glRotatef(cos(airshipAngle) * 300, 0, 1, 0);
				glScalef(2, 2, 2);
				glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				coffeecuplObj->Begin();
				glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
				//random = (rand() % 7) + 1;
				coffeecuplObj->shaderProgram->setUniformValue("tex", coffeecup->getTextureid());
				//coffeecuplObj->shaderProgram->setUniformValue("tex", trackobj->textureId+ random);
				if (!doingShadows)
					coffeecuplObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, coffeecup->getValues(), coffeecup->getBufferOffset(), 1.f, effect_clock, effectNum);
				else
					coffeecuplObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, coffeecup->getValues(), coffeecup->getBufferOffset(), 0.3f, effect_clock, effectNum);
				coffeecuplObj->End();
			glPopMatrix();			
		}		
	}
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

	glTranslatef(0, 10, 0);
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
void TrainView::drawTrain(Pnt3f pos, Pnt3f orient_cross,Pnt3f orient,bool doingShadows,bool empty,GLuint tid)
{
	int height = 2;
	int width = 5;
	int length = 3;
	float angle = 0.f;
	std::vector<int> buffersize;
	QVector<float> train_vts;
	train_vts.clear();
		
	pos.y += 6;
	glTranslatef(pos.x, pos.y, pos.z);
	angle = -radiansToDegrees(atan2(orient_cross.z, orient_cross.x));
	glRotatef(angle, 0, 1, 0);
	if(angle > 0)
		angle = -radiansToDegrees(acos(orient.y));
	else
		angle = radiansToDegrees(acos(orient.y));
	glRotatef(-angle, 0, 0, orient.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	float w = 10 / 2, h = 10 / 2;
	if (doingShadows)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}	
	//Train body
	{
		train_vts
			<< -width + 1 << height << -width + 1 - length
			<< width - 1 << height << -width + 1 - length
			<< width - 1 << height + 3 << width - 3
			<< width - 1 << height + 3 << width - 3
			<< -width + 1 << height + 3 << width - 3
			<< -width + 1 << height << -width + 1 - length;
		train_vts
			<< -width + 1 << height + 3 << width - 3
			<< -width + 1 << height << -width + 1 - length
			<< -width + 1 << height << width - 3
			<< -width + 1 << height << width - 3
			<< -width + 1 << height + 3 << width - 3
			<< -width + 1 << height + 3 << width - 3;
		train_vts
			<< width - 1 << height + 3 << width - 3
			<< width - 1 << height << -width + 1 - length
			<< width - 1 << height << width - 3
			<< width - 1 << height << width - 3
			<< width - 1 << height + 3 << width - 3
			<< width - 1 << height + 3 << width - 3;
		//Bottom
		train_vts
			<< -width << -height << -width - length
			<< width << -height << -width - length
			<< width << -height << width + length
			<< width << -height << width + length
			<< -width << -height << width + length
			<< -width << -height << -width - length;
		//Top
		train_vts
			<< -width << height << -width - length
			<< width << height << -width - length
			<< width << height << width + length
			<< width << height << width + length
			<< -width << height << width + length
			<< -width << height << -width - length;
		//Right
		train_vts
			<< -width << height << -width - length
			<< -width << -height << -width - length
			<< -width << -height << width + length
			<< -width << -height << width + length
			<< -width << height << width + length
			<< -width << height << -width - length;
		//Left
		train_vts
			<< width << height << -width - length
			<< width << -height << -width - length
			<< width << -height << width + length
			<< width << -height << width + length
			<< width << height << width + length
			<< width << height << -width - length;
		//Front
		train_vts
			<< width << height << width
			<< -width << height << width
			<< -width << -height << width
			<< -width << -height << width
			<< width << -height << width
			<< width << height << width;
		//Rear
		train_vts
			<< width << height << -width
			<< -width << height << -width
			<< -width << -height << -width
			<< -width << -height << -width
			<< width << -height << -width
			<< width << height << -width;
	}	
	int p = train_vts.size();
	buffersize.push_back(p);
	float blackUv[] = {0,1,1,1,1,0.5,1,0.5,0,0.5,0,1};
	float grayUv[] =  {0,0.5,1,0.5,1,0,1,0,0,0,0,0.5};
	float normal[] = {0,1,0};
	for (size_t i = 0; i < 18*2; i++)
	{
		train_vts << blackUv[i%12];
	}
	for (size_t i = 0; i < 36*2; i++)
	{
		train_vts << grayUv[i%12];
	}
	for (size_t i = 0; i < p; i++)
	{
		train_vts << normal[i%3];
	}
	buffersize.push_back(108);
	buffersize.push_back(p);
	trackobj->Begin();
	trackobj->shaderProgram->setUniformValue("tex", tid);
	if (!doingShadows)
		trackobj->Render(GL_TRIANGLES,false,0,ProjectionMatrex, ModelViewMatrex, train_vts, buffersize, 1.f,effect_clock, (effectNum==0)? train_effect: effectNum);
	else
		trackobj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, train_vts, buffersize, 0.3f, effect_clock, (effectNum == 0) ? train_effect : effectNum);
	trackobj->shaderProgram->setUniformValue("tex", trackobj->textureId+7);
	trackobj->End();
	
	if (doingShadows)
	{
		glDisable(GL_CULL_FACE);
	}
	glTranslatef(0,7.f,0);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	if (!empty)
	{
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
		nendoroid_vts
			<< 0.f << 0.f
			<< 1.f << 0.f
			<< 1.f << 1.f
			<< 1.f << 1.f
			<< 0.f << 1.f
			<< 0.f << 0.f;
		for (size_t i = 0; i < 18; i++)
		{
			nendoroid_vts << normal[i % 3];
		}
		buffersize.clear();
		buffersize.push_back(18);
		buffersize.push_back(12);
		buffersize.push_back(18);

		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
		nendoroid->Begin();
		nendoroid->shaderProgram->setUniformValue("tex", nendoroid->textureId);
		nendoroid->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, nendoroid_vts, buffersize, 1.f, effect_clock, effectNum);
		nendoroid->End();
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		nendoroid->Begin();
		nendoroid->shaderProgram->setUniformValue("tex", nendoroid->textureId + 1);
		nendoroid->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, nendoroid_vts, buffersize, 1.f, effect_clock, effectNum);
		nendoroid->End();
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	
	//Tire
	{
		glPushMatrix();
			glScalef(2, 2, 2);
			glTranslatef(3,-5, -2);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			tireObj->Begin();
			glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
			tireObj->shaderProgram->setUniformValue("tex", tire->getTextureid());
			if (!doingShadows)
				tireObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 1.f, effect_clock, effectNum);
			else
				tireObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 0.3f, effect_clock, effectNum);
			tireObj->End();
		glPopMatrix();
		glPushMatrix();
			glScalef(2, 2, 2);
			glTranslatef(-3, -5, -2);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			tireObj->Begin();
			glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
			tireObj->shaderProgram->setUniformValue("tex", tire->getTextureid());
			if (!doingShadows)
				tireObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 1.f, effect_clock, effectNum);
			else
				tireObj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 0.3f, effect_clock, effectNum);
			tireObj->End();
		glPopMatrix();
		glPushMatrix();
		glScalef(2, 2, 2);
		glTranslatef(-3, -5, 2);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		tireObj->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		tireObj->shaderProgram->setUniformValue("tex", tire->getTextureid());
		if (!doingShadows)
			tireObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 1.f, effect_clock, effectNum);
		else
			tireObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 0.3f, effect_clock, effectNum);
		tireObj->End();
		glPopMatrix();
		glPushMatrix();
		glScalef(2, 2, 2);
		glTranslatef(3, -5, 2);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		tireObj->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		tireObj->shaderProgram->setUniformValue("tex", tire->getTextureid());
		if (!doingShadows)
			tireObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 1.f, effect_clock, effectNum);
		else
			tireObj->Render(GL_TRIANGLE_STRIP, false, 0, ProjectionMatrex, ModelViewMatrex, tire->getValues(), tire->getBufferOffset(), 0.3f, effect_clock, effectNum);
		tireObj->End();
		glPopMatrix();
	}
}

