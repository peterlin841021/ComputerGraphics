#include "../include/Common.h"
#include "../include/ViewManager.h"
#include "../include/Scene.h"
#include<string>
#include<iostream>
using namespace std;
using namespace glm;
using namespace std;

int	width = 800;
int height = width;
float aspect;	
float interval = 70;// 1/FPS

Scene *scene;

void My_Init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	scene = new Scene();
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->Render();
    glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	scene->GetCamera()->SetWindowSize(width, height);
	glViewport(0, 0, width, height);
}

//Timer event
void My_Timer(int val)
{
	scene->Update(interval);
	glutPostRedisplay();
	glutTimerFunc((unsigned int)interval, My_Timer, val);
	My_Display();
}

//Mouse event
void My_Mouse(int button, int state, int x, int y)
{
	scene->MouseEvent(button, state, x, y);

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			//printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			//printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		//printf("Mouse %d is pressed\n", button);
	}
}
float speed = 1.f;
//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	if (key == '-')
	{
		if (interval <= 200) 
		{
			interval += 10;
			speed -= 0.5f;
			printf("SPEED:%f\n", speed);
		}			
	}
	if (key == '+')
	{
		if (interval > 10)
		{
			interval -= 10;
			speed += 0.5f;
			printf("SPEED:%f\n", speed);
		}			
	}
		
	scene->KeyBoardEvent(key);
}

//Special key event
void My_SpecialKeys(int key, int x, int y)
{
	scene->KeyBoardEvent(key);
}

//Menu event
void My_Menu(int id)
{
	scene->MenuEvent(id);

	switch(id)
	{
	case 0:
		exit(0);
		break;
	default:
		break;
	}
}

void My_Mouse_Moving(int x, int y) {
	scene->GetCamera()->mouseMoveEvent(x, y);
}
char *strToChar(string str)
{
	char *c = new char[str.length() + 1];
	return strcpy(c, str.c_str());
}
int main(int argc, char *argv[])
{
#ifdef __APPLE__
    //Change working directory to source code path
    chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("MikuWorld"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif

	//Print debug information 
	Common::DumpInfo();

	//Call custom initialize function
	My_Init();
	
	//定義選單
	////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int menu_act = glutCreateMenu(My_Menu);
	int menu_eff = glutCreateMenu(My_Menu);
	int menu_miku_eff = glutCreateMenu(My_Menu);
	int menu_ground_eff = glutCreateMenu(My_Menu);
	int menu_sky_eff = glutCreateMenu(My_Menu);
	int menu_scallion_eff = glutCreateMenu(My_Menu);
	int menu_hair_eff = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);	
	string menus[8] = { "Exit","Action","Effect","Miku effect","Ground effect","Sky effect","Scallion effect","Miku_HairColor effect"};

	glutAddSubMenu(strToChar(menus[1]), menu_act);
	glutAddSubMenu(strToChar(menus[2]), menu_eff);
	glutAddSubMenu(strToChar(menus[3]), menu_miku_eff);
	glutAddSubMenu(strToChar(menus[4]), menu_ground_eff);
	glutAddSubMenu(strToChar(menus[5]), menu_sky_eff);
	glutAddSubMenu(strToChar(menus[6]), menu_scallion_eff);
	glutAddSubMenu(strToChar(menus[7]), menu_hair_eff);
	
	glutAddMenuEntry(strToChar(menus[0]), 0);//EXIT
	int menu_offset[8] = { 0,10,22,34,46,58,70,78};
	string menus_actionItems[10] = {"Cheering","Walking","Flying","Clapping" ,"Bowing" ,"Swing skirt" ,"Drawing sword " ,"Final attack: excalibur" ,"Loituma dance","Standing" };
	glutSetMenu(menu_act);
	for (size_t i = 0; i < 10; i++)
	{
		glutAddMenuEntry(strToChar(menus_actionItems[i]), menu_offset[0] + i + 1);
	}
	
	int effect_items = 12;
	int haircuts = 6;
	string menus_effectItems[12] = {"Normal","Gray","Quantization","DoG","Abstraction","Meanfilter","Thresholddither","Mosaic","Emboss","Voronoi","Nebula","Glory"};
	string menus_colorItems[6] = { "White","Pink","Green","Purple","Black","Blond"};

	glutSetMenu(menu_eff);
	for (size_t i = 0; i < effect_items; i++)
	{
		glutAddMenuEntry(strToChar(menus_effectItems[i]), menu_offset[1] + i);
	}
	glutSetMenu(menu_miku_eff);
	for (size_t i = 0; i < effect_items; i++)
	{
		glutAddMenuEntry(strToChar(menus_effectItems[i]), menu_offset[2] + i);
	}
	glutSetMenu(menu_ground_eff);
	for (size_t i = 0; i < effect_items; i++)
	{
		glutAddMenuEntry(strToChar(menus_effectItems[i]), menu_offset[3] + i);
	}
	glutSetMenu(menu_sky_eff);
	for (size_t i = 0; i < effect_items; i++)
	{
		glutAddMenuEntry(strToChar(menus_effectItems[i]), menu_offset[4] + i);
	}
	glutSetMenu(menu_scallion_eff);
	for (size_t i = 0; i < effect_items; i++)
	{
		glutAddMenuEntry(strToChar(menus_effectItems[i]), menu_offset[5] + i);
	}
	glutSetMenu(menu_hair_eff);
	for (size_t i = 0; i < haircuts; i++)
	{
		glutAddMenuEntry(strToChar(menus_colorItems[i]), menu_offset[6] + i);
	}
	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////

	//Register GLUT callback functions
	////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(16, My_Timer, 0); 
	glutPassiveMotionFunc(My_Mouse_Moving);           
	glutMotionFunc(My_Mouse_Moving);     
	////////////////////

	//進入主迴圈
	glutMainLoop();

	return 0;
}

