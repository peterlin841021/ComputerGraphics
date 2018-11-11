#include "../include/Common.h"
#include "../include/ViewManager.h"
#include "../include/Scene.h"
#include<string>
#include<iostream>
using namespace std;

#define MENU_EXIT  0
#define MENU_CHEER 1
#define MENU_WALK 2
#define MENU_FLY 3
#define MENU_CLAP 4
#define MENU_BOW 5
#define MENU_LIFT 6
#define MENU_DRAW 7
#define MENU_EXCALIBUR 8 
#define MENU_LOITUMA 9
#define MENU_STAND 10

#define MENU_NORMAL 15
#define MENU_GRAY 16
#define MENU_QUANTIZATION 17
#define MENU_DOG 18
#define MENU_ABSTRACTION 19

using namespace glm;
using namespace std;

int	width = 800;
int   height = 800;
float	aspect;	
int	interval = 100;

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
	glutTimerFunc(interval, My_Timer, val);
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
	case MENU_EXIT:
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
	glutSetMenu(menu_main);
	//glutAddSubMenu("Scale", menu_entry);
	string menus[3] = { "Action","Effect","Exit" };
	glutAddSubMenu(strToChar(menus[0]), menu_act);
	glutAddSubMenu(strToChar(menus[1]), menu_eff);
	glutAddMenuEntry(strToChar(menus[2]), MENU_EXIT);

	string menus_actionItems[10] = {"Cheering","Walking","Flying","Clapping" ,"Bowing" ,"Liftting" ,"Drawing" ,"Excalibur" ,"Loituma","Standing" };
	glutSetMenu(menu_act);
	for (size_t i = 0; i < 10; i++)
	{
		glutAddMenuEntry(strToChar(menus_actionItems[i]),i+1);
	}
	
	string menus_effectItems[5] = {"Normal","Gray","Quantization","DoG","Abstraction"};
	glutSetMenu(menu_eff);
	for (size_t i = 0; i < 5; i++)
	{
		glutAddMenuEntry(strToChar(menus_effectItems[i]),15+i);
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

