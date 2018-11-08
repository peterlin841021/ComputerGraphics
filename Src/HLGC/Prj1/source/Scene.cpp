#include "../Include/Scene.h"
#include<vector>
#include<string>

#define HAIR 0 
#define FACE 1
#define BODY 2
#define SKIRT 3
#define LEFT_FORE_ARM 4
#define LEFT_HIND_ARM 5
#define LEFT_HAND 6
#define RIGHT_FORE_ARM 7
#define RIGHT_HIND_ARM 8
#define RIGHT_HAND 9
#define LEFT_THIGH 10
#define LEFT_CALF 11
#define RIGHT_THIGH 12
#define RIGHT_CALF 13
#define SCALLION 14

using namespace glm;
using namespace std;
ViewManager* Scene::camera = new ViewManager();//Camera

std::vector<mat4> origin, action;

const vec3 hair(0, 0.f, 0);
const vec3 face(0, 2.5f, 0);
const vec3 body(0, 0, 0);
const vec3 skirt(0, -0.1f, 0);
const vec3 leftForeArm(0.5f, 1.8f, 0);
const vec3 leftHindArm(1.6f, -0.8f, 0);
const vec3 leftHand(2.8f, -1.3f, 0);
const vec3 rightForeArm(-0.5f, 1.8f, 0);
const vec3 rightHindArm(-1.6f, -0.8f, 0);
const vec3 rightHand(-2.8f, -1.3f, 0);
const vec3 scallion(0, 0, 0);
const vec3 leftThigh(0.5f, -0.3f, 0);
const vec3 leftCalf(-0.05f, -2.2f, 0);
const vec3 rightThigh(-0.5f, -0.3f, 0);
const vec3 rightCalf(0.05f, -2.2f, 0);
const vec3 scale_ratio(0.05f, 0.05f, 0.05f);

int index = 0;//Motion present

bool isCheer;
bool isWalk;
bool isFly;
bool isClap;
bool isBow;
bool isLift;
bool scallion_use = true;
bool isMiku = true;

float cheerangles[20] = {0.0};
float walkangles[40] = { 0.0 };
float flyangles[10] = {0};
float flyheight = 0.1f;
float clapangles[40] = {0.0};
float bowangles[16] = {0};
float takeoffangles[15] = { 0 };

void Stand() 
{
	mat4 identity(1.0);
	float armangle = 0.8f;

	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);	
	action[SKIRT] *= translate(action[BODY], skirt);	
	action[LEFT_FORE_ARM] *= translate(action[BODY], vec3(leftForeArm.x, leftForeArm.y,leftForeArm.z));
	action[LEFT_FORE_ARM] *= rotate(identity, armangle, vec3(0, 0, -1));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);		
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);					
	action[RIGHT_FORE_ARM] *= translate(action[BODY], vec3(rightForeArm.x, rightForeArm.y, rightForeArm.z));
	action[RIGHT_FORE_ARM] *= rotate(identity, armangle, vec3(0, 0, 1));	
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);		
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);					
	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);	
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);	
	if (isMiku)
	{
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);		
	}
		
	for (size_t i = 0; i < action.size(); i++)
	{		
		action[i] *= scale(identity, scale_ratio);
	}
	for (size_t i = 0; i < action.size(); i++)
	{
		origin.push_back(action[i]);
	}
}
void Cheer(float angle)
{
	mat4 identity(1.0);	
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;	
	if(isMiku)
		action[SCALLION] = identity;

	action[LEFT_FORE_ARM] *= translate(identity, vec3(leftForeArm.x, leftForeArm.y, leftForeArm.z));
	action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(-1, 0, 0));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);	
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);			

	action[RIGHT_FORE_ARM] *= translate(identity, vec3(rightForeArm.x, rightForeArm.y, rightForeArm.z));
	action[RIGHT_FORE_ARM] *= rotate(identity, angle, vec3(-1, 0, 0));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);	
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);	
	if (isMiku)
	{
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
		action[SCALLION] *= scale(identity, scale_ratio);
	}	
	for (size_t i = LEFT_FORE_ARM; i <= RIGHT_HAND; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}	
}
void Walk(float angle)
{	
	mat4 identity(1.0);
	float armin = 0.8f;
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[LEFT_THIGH] = identity;
	action[LEFT_CALF] = identity;
	action[RIGHT_THIGH] = identity;
	action[RIGHT_CALF] = identity;
	action[SKIRT] = identity;

	//ARM
	action[LEFT_FORE_ARM] *= translate(identity, leftForeArm);
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(-1, 0, 0));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);	
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
			
	action[RIGHT_FORE_ARM] *= translate(identity, rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	action[RIGHT_FORE_ARM] *= rotate(identity, angle, vec3(1, 0, 0));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);	
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);		
	
	//LEG
	float kneeAngle = 0;
	action[LEFT_THIGH] *= translate(identity, leftThigh);
	action[LEFT_THIGH] *= rotate(identity, angle/4, vec3(1, 0, 0));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	//if (angle <= 2.0  && angle >= 1.8f || angle <= -1.8 && angle >= -2.f)
	if ((angle <= 2.f && angle >=1.9f) || (angle > -2.f && angle <= -1.9f ))
		kneeAngle = 0;
	else
		kneeAngle = angle / 4 < 0 ? angle / 4 + 0.4f : angle / 4  + 0.4f;

	action[LEFT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));

	action[RIGHT_THIGH] *= translate(identity, rightThigh);	
	action[RIGHT_THIGH] *= rotate(identity, -angle/4, vec3(1, 0, 0));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);	
	if ((angle <= 2.f && angle >= 1.9f) || (angle > -2.f && angle <= -1.9f))
		kneeAngle = 0;
	else
		kneeAngle = -angle / 4 < -2.f ? -angle / 4 - 0.4f : -angle / 4 + 0.4f;
	action[RIGHT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));

	action[SKIRT] *= translate(identity, skirt);
	kneeAngle = ((angle / 6) < -angle / 6 ? angle / 6 : -(angle / 6));
	action[SKIRT] *= rotate(identity, kneeAngle,vec3(1, 0, 0));

	for (size_t i = SKIRT; i <= RIGHT_CALF; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
void Fly(float angle) 
{
	mat4 identity(1.0);
	for (size_t i = 0; i < action.size(); i++)
	{
		action[i] = identity;
	}
	
	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);			
	

	action[LEFT_FORE_ARM] *= translate(identity, vec3(leftForeArm.x, leftForeArm.y, leftForeArm.z));
	action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(0, 0, -1));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);

	action[RIGHT_FORE_ARM] *= translate(identity, vec3(rightForeArm.x, rightForeArm.y, rightForeArm.z));
	action[RIGHT_FORE_ARM] *= rotate(identity, angle, vec3(0, 0, 1));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
	if(isMiku)
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);

	//LEG
	float kneeAngle = 0;
	float leg = angle / 10.f;
	action[LEFT_THIGH] *= translate(identity, leftThigh);
	action[LEFT_THIGH] *= rotate(identity, leg, vec3(1, 0, 0));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	//action[LEFT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));
	action[RIGHT_THIGH] *= translate(identity, rightThigh);
	action[RIGHT_THIGH] *= rotate(identity, -leg, vec3(1, 0, 0));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	//action[RIGHT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));
	action[SKIRT] *= translate(identity, skirt);
	action[SKIRT] *= rotate(identity, leg*2, vec3(1, 0, 0));

	for (size_t i = 0; i < action.size(); i++)
	{
		action[i] *= translate(identity, vec3(0, flyheight, 0));
	}
	flyheight += 0.01f;
	if (flyheight >= 0.5f)
		flyheight = 0;
	for (size_t i = 0; i < action.size(); i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
void Clap(float angle,bool prepare)
{
	mat4 identity(1.0);	
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[LEFT_FORE_ARM] *= translate(identity, leftForeArm);
	
	if (prepare)
	{	
		action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(0, 1, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);				
		if (angle < -1.2f)
		{
			action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
			action[LEFT_HAND] *= rotate(identity, 5.f, vec3(1, 0, 0));
		}
		else
		{
			action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
		}
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 0, 0));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
	}
	
	action[RIGHT_FORE_ARM] *= translate(identity, rightForeArm);
	
	if (prepare)
	{	
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(0, 1, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		
		if (angle < -1.2f)
		{	
			action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
			action[RIGHT_HAND] *= rotate(identity, 5.f, vec3(1, 0, 0));
		}	
		else
		{
			action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
		}
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 0, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
	}
	
	for (size_t i = LEFT_FORE_ARM; i <= RIGHT_HAND; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
void Bow(float angle, bool prepare)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	action[HAIR] = identity;
	action[FACE] = identity;
	action[BODY] = identity;
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[SCALLION] = identity;

	action[BODY] *= translate(identity, body);	
	if (angle > 0.f)
	{
		action[BODY] *= rotate(identity, angle * 0.6f, vec3(1, 0, 0));		
	}
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);

	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	if (prepare)
	{
		if (angle > 0.7f)
		{
			action[LEFT_FORE_ARM] *= rotate(identity, -0.2f, vec3(1, 0, 0));
			action[RIGHT_FORE_ARM] *= rotate(identity, -0.2f, vec3(1, 0, 0));
		}
		action[LEFT_FORE_ARM] *= rotate(identity, -0.5f, vec3(0, 1, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, 0.5f, vec3(0, 1, 1));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
		action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
		if (angle > 0.7f)
		{						
			action[LEFT_HAND] *= rotate(identity, angle, vec3(1, 0, 0));
			action[RIGHT_HAND] *= rotate(identity, angle, vec3(1, 0, 0));
			action[LEFT_HAND] *= rotate(identity, -0.5f, vec3(0, 0, 1));
			action[RIGHT_HAND] *= rotate(identity, 0.5f, vec3(0, 0, 1));
		}				
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(0, 1, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(0, 1, 1));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
		action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
	}
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = HAIR; i <= RIGHT_HAND; i++)
	{
		if (i == SKIRT)continue;
		action[i] *= scale(identity, scale_ratio);
	}
	action[SCALLION] *= scale(identity, scale_ratio);
}
void Lift(float angle, bool prepare)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	action[HAIR] = identity;
	action[FACE] = identity;	
	action[BODY] = identity;
	action[SKIRT] = identity;
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;

	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));	
	if (prepare)
	{
		float a = -0.2f;
		action[LEFT_FORE_ARM] *= rotate(identity, a + angle, vec3(0, 0, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -a - angle, vec3(0, 0, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);

		
		action[RIGHT_HIND_ARM] *= rotate(identity, angle*4 , vec3(-1, 0, 1));
		action[LEFT_HIND_ARM] *= rotate(identity, angle*4, vec3(-1, 0, -1));

		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
		action[LEFT_HAND] *= rotate(identity, 0.8f, vec3(1, 0, 0));
		action[RIGHT_HAND] *= rotate(identity, 0.8f, vec3(1, 0, 0));

		action[LEFT_HAND] *= rotate(identity, -0.8f, vec3(0, 0, 1));
		action[RIGHT_HAND] *= rotate(identity, 0.8f, vec3(0, 0, 1));
		action[SKIRT] *= rotate(identity, -angle*1.6f, vec3(1, 0, 0));
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, angle*1.5f, vec3(0, 1, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 1, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);

		action[LEFT_HIND_ARM] *= rotate(identity, angle, vec3(0, 1, 1));
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle, vec3(0, 1, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
		//printf("%f\n", angle);
	}

	for (size_t i = HAIR; i <= RIGHT_HAND; i++)
	{
		/*if (i == SKIRT )continue;*/
		action[i] *= scale(identity, scale_ratio);
	}
}
char *stringToChar(string str)
{
	char *c = new char[str.length() + 1];
	return strcpy(c, str.c_str());
}
void Scene::initMiku()
{
	string parts_objs[] =
	{
		"mikuhair","mikuface","mikubody","mikuskirt",
		"mikuleftforearm","mikulefthindarm","mikulefthand","mikurightforearm","mikurighthindarm","mikurighthand",
		"mikuleftthigh","mikuleftcalf","mikurightthigh","mikurightcalf"
	};
	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		parts_objs[i].append(".obj");
	}
	string parts_textures[] =
	{
		"mikuhair","mikuface","mikubody","scallion"
	};
	for (size_t i = 0; i < sizeof(parts_textures) / sizeof(string); i++)
	{
		parts_textures[i].append(".png");		
	}
	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		if (i == 0 || i == 1)
		{
			models.push_back(new BaseModel(stringToChar(parts_objs[i]), stringToChar(parts_textures[i])));
		}
		else
		{
			models.push_back(new BaseModel(stringToChar(parts_objs[i]), stringToChar(parts_textures[2])));
		}		
	}
	models.push_back(new BaseModel(stringToChar("scallion.obj"), stringToChar(parts_textures[3])));
	mat4 identity(1.0);
	for (size_t i = 0; i < models.size(); i++)
	{
		action.push_back(identity);
	}
	action.push_back(identity);
}
void Scene::initOthers()
{
	string parts_objs[] =
	{
		"none","head","body","none",
		"leftarm","none","none","rightarm","none","none",
		"leftleg","none","rightleg","none"
	};

	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		parts_objs[i].append(".obj");
	}
	string  parts_textures[] =
	{
		"robot"
	};

	for (size_t i = 0; i < sizeof(parts_textures) / sizeof(string); i++)
	{
		parts_textures[i].append(".png");
	}

	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		models.push_back(new BaseModel(stringToChar(parts_objs[i]), stringToChar(parts_textures[0])));
	}
	//Transformation matrix
	mat4 identity(1.0);
	for (size_t i = 0; i < models.size(); i++)
	{
		action.push_back(identity);
	}
}
Scene::Scene()
{
	//Initialize	
	camera = new ViewManager();
	if (isMiku)
	{
		initMiku();
	}
	else
	{
		initOthers();
	}
	Stand();	
}
void Scene::MouseEvent(int button, int state, int x, int y)
{
	camera->mouseEvents(button, state, x, y);
}

void Scene::KeyBoardEvent(int key)
{

}

void Scene::KeyBoardEvent(unsigned char key)
{
	camera->keyEvents(key);
	float cheer_v = 0.3f;
	float walk_v = 0.2f;
	float fly_v = 0.2f;
	float clap_v = 0.1f;
	float bow_v = 0.1f;
	float take_v = 0.1f;
	float start = 0.f;
	mat4 identity(1.0);//Flying
	switch (key)
	{
	case 'z':
	case 'Z':		
		break;
	case 'x':
	case 'X':
		//models[1]->Rotate(glm::vec3(0, 1, 0), 0.1f);
		break;
	case 'c':
	case 'C':	
		//models[1]->Translate(glm::vec3(-0.1, 0, 0));
		break;
	case 'v':
	case 'V':
		//models[1]->Translate(glm::vec3(0.1, 0, 0));
		break;
	case 'r':
	case 'R':		
		//mv = matrix_stack.top();
		break;	
	case 'w':
	case 'W':		
		break;
	case 'f':
	case 'F':		
		break;
	case 'b':
	case 'B':		
		break;
	case 't':
	case 'T':	
		break;
	case 32:
		action = origin;
		isFly = false;
		isWalk = false;
		isCheer = false;
		isClap = false;	
		isBow = false;
		isLift = false;
		break;
	default:
		break;
	}
}

void Scene::MenuEvent(int item)
{	
	float cheer_v = 0.3f;
	float walk_v = 0.2f;
	float fly_v = 0.2f;
	float clap_v = 0.1f;
	float bow_v = 0.1f;
	float take_v = 0.1f;
	float start = 0.f;
	mat4 identity(1.0);
	action = origin;
	switch (item)
	{
	case 0:
		break;
	case 1:
		index = 0;
		for (size_t i = 0; i < 10; i++)
		{
			cheerangles[i] = start;
			start += cheer_v;
		}
		for (size_t i = 10; i < 20; i++)
		{
			cheerangles[i] = start;
			start -= cheer_v;
		}
		isCheer = !isCheer;
		break;
	case 2:
		index = 0;
		for (size_t i = 0; i < 10; i++)
		{
			walkangles[i] = start;
			start += walk_v;
		}
		for (size_t i = 10; i < 30; i++)
		{
			walkangles[i] = start;
			start -= walk_v;
		}
		for (size_t i = 30; i < 40; i++)
		{
			walkangles[i] = start;
			start += walk_v;
		}
		isWalk = !isWalk;
		break;
	case 3:
		flyheight = 0.f;
		action = origin;
		index = 0;
		start = 0.5f;
		for (size_t i = 0; i < 5; i++)
		{
			flyangles[i] = start;
			start -= fly_v;
		}
		for (size_t i = 5; i < 10; i++)
		{
			flyangles[i] = start;
			start += fly_v;
		}
		isFly = !isFly;
		break;
	case 4:
		index = 0;
		for (size_t i = 0; i < 8; i++)
		{
			clapangles[i] = start;
			start += clap_v;
		}
		start = 0.f;
		//up
		for (size_t i = 8; i < 24; i++)
		{
			clapangles[i] = start;
			start -= clap_v;
		}
		for (size_t i = 24; i < 28; i++)
		{
			clapangles[i] = start;
			start += clap_v;
		}
		for (size_t i = 28; i < 32; i++)
		{
			clapangles[i] = start;
			start -= clap_v;
		}
		for (size_t i = 32; i < 36; i++)
		{
			clapangles[i] = start;
			start += clap_v;
		}
		for (size_t i = 36; i < 40; i++)
		{
			clapangles[i] = start;
			start -= clap_v * 1.3f;
		}
		isClap = !isClap;
		break;
	case 5:
		index = 0;
		for (size_t i = 0; i < 6; i++)
		{
			bowangles[i] = start;
			start -= bow_v;
		}
		
		start = 0.f;
		for (size_t i = 6; i < 16; i++)
		{
			bowangles[i] = start;
			start += bow_v;
		}
		isBow = !isBow;
		break;
	case 6:
		index = 0;
		for (size_t i = 0; i < 5; i++)
		{
			takeoffangles[i] = start;
			start -= take_v / 2;
		}
		start = 0;
		for (size_t i = 5; i < 15; i++)
		{
			takeoffangles[i] = start;
			start += take_v * 0.5f;
		}
		isLift = !isLift;
		break;
	case 7:
		action = origin;
		isFly = false;
		isWalk = false;
		isCheer = false;
		isClap = false;
		isBow = false;
		isLift = false;
		break;
	}
}

void Scene::Render()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	for (size_t i = 0; i < models.size(); i++)
	{		
		if(i == SCALLION && scallion_use)
			models[i]->Render(action[SCALLION]);
		else if (i == SCALLION) continue;
		else
			models[i]->Render(action[i]);
	}
	glDisable(GL_DEPTH_TEST);
}

void Scene::Update(float dt)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);		
	if (isCheer)
	{
		Cheer(cheerangles[index]);
		index++;
		if (index == sizeof(cheerangles) / sizeof(float))
			index = 0;
	}
	else if (isWalk)
	{
		Walk(walkangles[index]);		
		index++;
		if (index == sizeof(walkangles) / sizeof(float))
			index = 0;
	}
	else if (isFly) 
	{
		Fly(flyangles[index]);
		index++;
		if (index == sizeof(flyangles) / sizeof(float))
			index = 0;		
	}
	else if (isClap) 
	{		
		if(index < 10)
			Clap(clapangles[index],false);
		else if(index < 28)
			Clap(clapangles[index], true);
		else if(index > 28)
			Clap(clapangles[index], true);		
		index++;
		if (index == sizeof(clapangles) / sizeof(float))
		{
			isClap = false;
			mat4 identity(1.0);
			action[LEFT_HAND] *= rotate(identity, -0.3f, vec3(1, 0, 0));
			action[LEFT_HAND] *= rotate(identity,0.65f,vec3(0,1,0));
			action[RIGHT_HAND] *= rotate(identity, -0.3f, vec3(1, 0, 0));
			action[RIGHT_HAND] *= rotate(identity,-0.65f, vec3(0, 1, 0));
			index = 0;
		}
	}
	else if(isBow)
	{
		if(index < 6)
			Bow(bowangles[index],false);
		else if(index >= 6)
			Bow(bowangles[index], true);
		index++;
		if (index == sizeof(bowangles) / sizeof(float)) 
		{
			index = 0;
			isBow = false;				
		}		
	}
	else if(isLift)
	{
		if (index < 5)
			Lift(takeoffangles[index],false);
		else if(index >= 5)
			Lift(takeoffangles[index], true);
		index++;
		if (index == sizeof(takeoffangles) / sizeof(float))
		{
			index = 0;
			mat4 identity(1.0);
			isLift = false;
			
		}
	}
	for (size_t i = 0; i < models.size(); i++)
	{		
		if (i == SCALLION && scallion_use)
			models[i]->Render(action[SCALLION]);
		else if (i == SCALLION) continue;
		else
			models[i]->Render(action[i]);
	}
	glDisable(GL_DEPTH_TEST);
}

std::vector<BaseModel*> Scene::GetModels()
{
	return models;
}