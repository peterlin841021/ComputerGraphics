#include "..\include\Common.h"
#define MENU_EXIT   4

using namespace glm;
using namespace std;

#define ACTION_STATE_MOVE 1
#define ACTION_STATE_JUMP 2
#define ACTION_STATE_ATTACK 3
#define ACTION_STATE_DIE 4

GLuint sp;
const unsigned int interval = 350;
GLuint vao, vvbo;
const size_t defalut_w = 800;
const size_t defalut_h = 600;


void Render(glm::mat4 pm, glm::mat4 mm,vector<vec3> pos,vector<vec2> uv, vector<int> buffersize, clock_t time, int effect);
GLuint generateTexture(const char *image);

struct Uniform
{
	GLuint mv;
	GLuint pm;
	GLuint time;
	GLuint effect;
	GLuint tex;
};
struct Character 
{
	string charactername;
	mat4 modelview;
	mat4 projection;
	vector<vector<vec2>> action;
	pair<int, int> idle;	
	pair<int, int> move;
	pair<int, int> jump;
	pair<int, int> attack;
	pair<int, int> injured;
	pair<int, int> die;
	size_t hp;
	size_t damage;
	size_t nextframe = 0;
	bool beattack;
	size_t state = 0;
	GLuint textureid;
	float xpos, ypos;
	float attack_distance;
	float jump_distance;	
	int dir;

	Character(string name,mat4 mv,mat4 pm)
	{
		charactername = name;
		modelview = mv;
		projection = pm;
		
		xpos = 0;
		ypos = 0;
		dir = 1;
	}
	void reset()
	{
		hp = 100;
		state = 0;
		xpos = -0.9f;
		ypos = -0.7f;
		dir = 1;
		modelview = mat4(1.0);
		modelview *= translate(mat4(1.0), vec3(xpos, ypos, 0));
		modelview *= rotate(mat4(1.0), 180.f, vec3(0, 1, 0));		
		modelview *= scale(mat4(1.0), vec3(0.25f, 0.25f, 0.25f));
	}
};
Uniform *uniform;
vector<Character*> characters;
vector<vector<vec2>> generate_ani_uv(float origin_w, float origin_h,size_t wpart, size_t hpart)
{
	vector<vector<vec2>> output;
	float xstart = 0.0,ystart = 1.0;
	float xoffset = (origin_w / wpart) / origin_w;
	float yoffset = (origin_h / hpart) / origin_h;
	for (size_t i = 0; i < hpart; i++)
	{
		float x = 0;		
		for (size_t j = 0; j < wpart; j++)
		{
			vector<vec2> v;
			xstart += xoffset;
			v.push_back(vec2(x, ystart));
			v.push_back(vec2(xstart, ystart));
			v.push_back(vec2(xstart, ystart - yoffset));
			v.push_back(vec2(xstart, ystart - yoffset));
			v.push_back(vec2(x, ystart - yoffset));
			v.push_back(vec2(x, ystart));
			x = xstart;
			output.push_back(v);
		}
		ystart -= yoffset;
		xstart = 0;	
	}
	return output;
}

int index = 0;
bool isjump = false;
bool isdied = false;
size_t jumpcounter = 0;
size_t attackcounter = 0;

void init_shader()
{
	sp = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = LoadShaderSource("./vs.glsl");
	char** fsSource = LoadShaderSource("./fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);

	glCompileShader(vs);
	glCompileShader(fs);
	ShaderLog(vs);
	ShaderLog(fs);
	//Attach Shader to program
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);

	glGenBuffers(1, &vvbo);
	glGenVertexArrays(1, &vao);
}
size_t collisiondetect(vec3 v1, vec3 v2, size_t damage,float attack_distance)
{
	float dis = sqrt(pow(v1.x - v2.x,2)+ pow(v1.y - v2.y, 2));	
	if (dis < attack_distance)
		return damage;
	else
		return 0;
}
size_t attackcal(vec3 v1, vec3 v2, size_t damage, float attack_distance)
{
	float dis = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
	float valid_dis = 0.5f;
	if (dis < attack_distance)
	{
		return damage;
	}
	else
	{
		return 0;
	}
}
void My_Init()
{
	vector<vector<vec2>> miku_uv, scene_uv;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	init_shader();
	//Uniform variables
	uniform = new Uniform();
	uniform->pm = glGetUniformLocation(sp, "pm");
	uniform->mv = glGetUniformLocation(sp, "mm");
	uniform->effect = glGetUniformLocation(sp, "effect");
	uniform->time = glGetUniformLocation(sp, "time");

	float character_scale = 0.25f;
	float scene_scale = 1.f;
	const size_t count = 3;
	float ground = -0.7f;
	float leftboundry = -0.9f;
	mat4 pm(1.0);
	mat4 identity(1.0);
	mat4 character_mv(1.0);
	mat4 scene_mv(1.0);
	mat4 monster_mv(1.0);

	character_mv *= translate(identity, vec3(leftboundry, ground, 0));
	character_mv *= rotate(identity,180.f,vec3(0,1,0));
	character_mv *= scale(identity, vec3(character_scale, character_scale, character_scale));
	scene_mv *= scale(identity, vec3(scene_scale, scene_scale, character_scale));
	monster_mv *= translate(identity, vec3(0.7f, ground, 0));
	//monster_mv *= rotate(identity, -180.f, vec3(0, 1, 0));
	monster_mv *= scale(identity, vec3(character_scale, character_scale, character_scale));

	Character* cs[count] =
	{
		new Character("Background",identity,pm),
		new Character("Miku",identity,pm),
		new Character("Origin marhroom",identity,pm)
	};
	const char* texture_images[count] = { "background.png" ,"mikuR.png","origin_mashroom.png" };
	for (size_t i = 0; i < count; i++)
	{
		cs[i]->textureid = generateTexture(texture_images[i]);

		if (i == 0)//Scene
		{
			cs[i]->modelview = scene_mv;
			cs[i]->action = generate_ani_uv(558, 299, 1, 1);
			cs[i]->idle = pair<int, int>(0, 1);
		}
		else if (i == 1)
		{
			cs[i]->modelview = character_mv;
			cs[i]->action = generate_ani_uv(1000, 100, 10, 1);
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->move = pair<int, int>(2, 2);
			cs[i]->jump = pair<int, int>(4, 1);
			cs[i]->attack = pair<int, int>(6, 3);
			cs[i]->die = pair<int, int>(5, 1);
			cs[i]->injured = pair<int, int>(9, 1);
			cs[i]->xpos = leftboundry;
			cs[i]->ypos = ground;
			cs[i]->state = 0;
			cs[i]->beattack = false;
			cs[i]->hp = 100;
			cs[i]->jump_distance = 0.2f;
			cs[i]->attack_distance = 0.4f;
			cs[i]->damage = 1;
		}
		else
		{
			cs[i]->modelview = monster_mv;
			cs[i]->action = generate_ani_uv(611, 100, 6, 1);			
			cs[i]->move = pair<int, int>(0, 3);			
			cs[i]->die = pair<int, int>(3, 3);		
			cs[i]->xpos = 0.7f;
			cs[i]->ypos = ground;
			cs[i]->state = ACTION_STATE_MOVE;
			cs[i]->hp = 5;
			cs[i]->attack_distance = 0.3f;
			cs[i]->damage = 10;
		}
		characters.push_back(cs[i]);
	}
}

GLuint generateTexture(const char *image)
{
	GLuint id = characters.size();
	TextureData *image_data = &(Load_png(image));
	if (!image_data->data)
	{
		printf("Read image fail!\n");
		system("pause");
		return 0;
	}
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_data->width, image_data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return id;
}

void Render(glm::mat4 pm, glm::mat4 mm, vector<vec3> pos, vector<vec2> uv, vector<int> buffersize, clock_t time,int effect)
{	
	glUseProgram(sp);
	glBindVertexArray(vao);	
	float mv_matrix[16] = { 0 };
	float pm_matrix[16] = { 0 };
	int pos_size = buffersize[0];
	int color_size = buffersize[1];
	int total_size = 0;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			mv_matrix[i * 4 + j] = mm[i][j];
		}
	}
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			pm_matrix[i * 4 + j] = pm[i][j];
		}
	}
	for (size_t i = 0; i < buffersize.size(); i++)
	{
		total_size += buffersize[i] * 3;
	}

	float *v = new float[total_size];
	int vec_index = 0;
	for (int i = 0; i < pos_size; i++)
	{
		v[vec_index] = pos[i].x;
		vec_index++;
		v[vec_index] = pos[i].y;
		vec_index++;
		v[vec_index] = pos[i].z;
		vec_index++;
	}	
	for (int i = 0 ; i < color_size; i ++)
	{
		v[vec_index] = uv[i].x;
		vec_index++;
		v[vec_index] = uv[i].y;
		vec_index++;
	}	
	glUniformMatrix4fv(uniform->mv,1, GL_FALSE, mv_matrix);
	glUniformMatrix4fv(uniform->pm, 1, GL_FALSE, pm_matrix);
	glUniform1i(uniform->effect, effect);
	glUniform1f(uniform->time, time);

	glBindBuffer(GL_ARRAY_BUFFER, vvbo);
	glBufferData(GL_ARRAY_BUFFER, total_size * sizeof(float),v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0,0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,0, (void*)(pos_size * 3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0,pos.size());
}
// GLUT callback. Called to draw the scene.

void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);
	
	glUseProgram(sp);
	//Two triangles
	vector<vec3> pos;
	pos.push_back(vec3(-1,1,0));
	pos.push_back(vec3(1, 1, 0));
	pos.push_back(vec3(1, -1, 0));

	pos.push_back(vec3(1, -1, 0));
	pos.push_back(vec3(-1, -1, 0));
	pos.push_back(vec3(-1, 1, 0));
	vector<int> buffer_size;
	buffer_size.push_back(6);
	buffer_size.push_back(6);
	//Time
	clock_t time = clock();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthFunc(GL_LEQUAL);

	//Damage detect
	if (!characters[1]->beattack)
	{
		for (size_t i = 2; i < characters.size(); i++)
		{
			size_t amount = collisiondetect(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 0), characters[i]->damage, characters[i]->attack_distance);
			if (amount > 0 && characters[i] ->state != ACTION_STATE_DIE)
			{
				characters[1]->beattack = true;
				if (characters[1]->hp > 0)
					characters[1]->hp -= amount;
				if (characters[1]->hp == 0)
				{
					isdied = true;
					characters[1]->state = ACTION_STATE_DIE;
				}					
				printf("Miku hP:%d\n", characters[1]->hp);
			}
		}
	}	
	//Attack detect
	if (characters[1]->state == ACTION_STATE_ATTACK && attackcounter == 3)
	{
		for (size_t i = 2; i < characters.size(); i++)
		{
			size_t amount = attackcal(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 1), characters[1]->damage, characters[1]->attack_distance);
			characters[i]->hp -= amount;
			if (amount > 0 && characters[i]->state != ACTION_STATE_DIE)
			{
				if (characters[i]->hp == 0)
					characters[i]->state = 4;
				printf("Monster hp:%d\n", characters[i]->hp);
			}			
		}
	}
	for (size_t i = 0; i < characters.size(); i++)
	{		
		glActiveTexture(characters[i]->textureid);		
		glBindTexture(GL_TEXTURE_2D, characters[i]->textureid);		
		if (i == 0)
		{
			Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[0], buffer_size, time, 0);
		}			
		else if(i == 1)//Miku
		{
			switch (characters[i]->state)
			{
			case ACTION_STATE_MOVE:				
				if (characters[i]->nextframe >= characters[i]->move.second)
				{
					characters[i]->nextframe = 0;
					characters[i]->state = 0;
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->move.first + characters[i]->nextframe], buffer_size, time, 0);
				}
				else
				{
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->move.first + characters[i]->nextframe], buffer_size, time, 0);
					characters[i]->nextframe++;
				}
				break;
			case ACTION_STATE_JUMP:
				if (characters[i]->nextframe >= characters[i]->jump.second)
				{
					characters[i]->nextframe = 0;
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->jump.first + characters[i]->nextframe], buffer_size, time, 0);
				}
				else
				{
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->jump.first + characters[i]->nextframe], buffer_size, time, 0);
					characters[i]->nextframe++;
				}
				if (jumpcounter < 3)
				{
					characters[i]->ypos += characters[i]->jump_distance;
					characters[1]->modelview = mat4(1.0);
					characters[1]->modelview *= translate(mat4(1.0), vec3(characters[1]->xpos, characters[1]->ypos, 0));					
					if (characters[1]->dir == 1)
					{
						characters[1]->modelview *= rotate(mat4(1.0), 180.f, vec3(0, 1, 0));
						characters[1]->modelview *= scale(mat4(1.0), vec3(0.25f, 0.25f, 0.25f));
					}
					else
					{
						characters[1]->modelview *= rotate(mat4(1.0), 270.f, vec3(0, 1, 0));
						characters[1]->modelview *= scale(mat4(1.0), vec3(0.2f, 0.2f, 0.2f));
					}
					jumpcounter++;
				}				
				else if(jumpcounter < 6)
				{
					characters[i]->ypos -= characters[i]->jump_distance;
					characters[1]->modelview = mat4(1.0);
					characters[1]->modelview *= translate(mat4(1.0), vec3(characters[1]->xpos, characters[1]->ypos, 0));
					if (characters[1]->dir == 1)
					{
						characters[1]->modelview *= rotate(mat4(1.0), 180.f, vec3(0, 1, 0));
						characters[1]->modelview *= scale(mat4(1.0), vec3(0.25f, 0.25f, 0.25f));
					}
					else
					{
						characters[1]->modelview *= rotate(mat4(1.0), 270.f, vec3(0, 1, 0));
						characters[1]->modelview *= scale(mat4(1.0), vec3(0.2f, 0.2f, 0.2f));
					}
					jumpcounter ++;
				}
				else if (jumpcounter == 6)
				{
					characters[i]->state = 0;
					isjump = false;
					characters[i]->ypos =-0.7f;					
					jumpcounter = 0;
				}
				break;
			case ACTION_STATE_ATTACK:
				if (characters[i]->nextframe >= characters[i]->attack.second)
				{
					characters[i]->nextframe = 0;
					attackcounter = 0;
					characters[i]->state = 0;
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->attack.first + characters[i]->nextframe], buffer_size, time, 0);
					if (jumpcounter > 0)
					{
						characters[i]->state = ACTION_STATE_JUMP;
					}						
				}
				else
				{
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->attack.first + characters[i]->nextframe], buffer_size, time, 0);
					characters[i]->nextframe++;
					attackcounter++;
				}
				break;
			case ACTION_STATE_DIE:
				Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->die.first], buffer_size, time, 0);				
				break;
			default:				
				Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->idle.first + (characters[i]->nextframe % characters[i]->idle.second)], buffer_size, time, 0);
				break;
			}
			if (characters[i]->beattack && !isdied)
			{
				Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->injured.first], buffer_size, time, 0);
				characters[i]->beattack = false;
			}				
		}
		else 
		{
			switch (characters[i]->state)
			{
			case ACTION_STATE_MOVE:
				if (characters[i]->nextframe >= characters[i]->move.second)
				{
					characters[i]->nextframe = 0;
					characters[i]->state = 1;
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->move.first + characters[i]->nextframe], buffer_size, time, 0);
				}
				else
				{
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->move.first + characters[i]->nextframe], buffer_size, time, 0);
					characters[i]->nextframe++;
				}
				break;
			case ACTION_STATE_ATTACK:
				if (characters[i]->nextframe >= characters[i]->attack.second)
				{
					characters[i]->nextframe = 0;
					characters[i]->state = 1;
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->attack.first + characters[i]->nextframe], buffer_size, time, 0);
				}
				else
				{
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->attack.first + characters[i]->nextframe], buffer_size, time, 0);
					characters[i]->nextframe++;
				}
				break;
			case ACTION_STATE_DIE:
				if (characters[i]->nextframe >= characters[i]->die.second)
				{
					characters[i]->nextframe = characters[i]->die.second-1;
					characters[i]->state = 4;
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->die.first + characters[i]->nextframe], buffer_size, time, 0);
				}
				else
				{
					Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[characters[i]->die.first + characters[i]->nextframe], buffer_size, time, 0);
					characters[i]->nextframe++;
				}				
				break;			
			}
		}
	}	
	glDisable(GL_BLEND);
	glutSwapBuffers();
}
//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
//Kayboard input
void keyboardevent(unsigned char key,int x,int y) 
{	
	float steps = 0.03f;
	float sc = 0.2f, ground =-0.7f;
	switch (key)
	{
	case 'w':
	case 'W':
		if (!isjump && !isdied)
		{
			isjump = true;
			characters[1]->nextframe = 0;
			characters[1]->state = ACTION_STATE_JUMP;
			characters[1]->modelview = mat4(1.0);
			characters[1]->modelview *= translate(mat4(1.0), vec3(characters[1]->xpos, characters[1]->ypos + steps * 2, 0));
			if (characters[1]->dir == 1)
			{
				characters[1]->modelview *= rotate(mat4(1.0), 180.f, vec3(0, 1, 0));
				characters[1]->modelview *= scale(mat4(1.0), vec3(0.25f, 0.25f, 0.25f));
			}
			else
			{
				characters[1]->modelview *= rotate(mat4(1.0), 270.f, vec3(0, 1, 0));
				characters[1]->modelview *= scale(mat4(1.0), vec3(sc, sc, sc));
			}
			characters[1]->ypos += steps * 2;
		}		
		break;
	case 'a':
	case 'A':
		if ((jumpcounter < 3 && isjump || !isjump && jumpcounter == 0) && !isdied)
		{
			characters[1]->state = ACTION_STATE_MOVE;
			if (characters[1]->xpos >= -0.9f)
			{
				characters[1]->modelview = mat4(1.0);
				characters[1]->modelview *= translate(mat4(1.0), vec3(characters[1]->xpos + -steps, characters[1]->ypos, 0));
				characters[1]->modelview *= rotate(mat4(1.0), 270.f, vec3(0, 1, 0));
				characters[1]->modelview *= scale(mat4(1.0), vec3(sc, sc, sc));
				characters[1]->xpos += -steps;
			}
			characters[1]->dir = -1;
			if (isjump)
				characters[1]->state = ACTION_STATE_JUMP;
		}				
		break;
	case 's':
	case 'S':		
		break;
	case 'd':
	case 'D':
		if ((jumpcounter < 3 && isjump || !isjump && jumpcounter == 0) && !isdied)
		{
			characters[1]->state = ACTION_STATE_MOVE;
			if (characters[1]->xpos <= 0.8f)
			{
				characters[1]->modelview = mat4(1.0);
				characters[1]->modelview *= translate(mat4(1.0), vec3(characters[1]->xpos + steps, characters[1]->ypos, 0));
				characters[1]->modelview *= rotate(mat4(1.0), 180.f, vec3(0, 1, 0));
				characters[1]->modelview *= scale(mat4(1.0), vec3(0.25f, 0.25f, 0.25f));
				characters[1]->xpos += steps;
			}
			characters[1]->dir = 1;
			if (isjump)
				characters[1]->state = ACTION_STATE_JUMP;
		}							
		break;
	case 'z':
	case 'Z':
		if (!isdied)
		{
			characters[1]->nextframe = 0;
			characters[1]->state = ACTION_STATE_ATTACK;			
		}	
		break;
	case '+':		
		break;
	case '-':		
		break;
	case 'R':
	case 'r':
		characters[1]->nextframe = 0;
		isdied = false;
		isjump = false;		
		jumpcounter = 0;
		characters[1]->reset();
		break;
	default:		
		break;
	}
	
}
//Timer event
void My_Timer(int val)
{	
	glutPostRedisplay();
	glutTimerFunc(interval, My_Timer, val);
}
//Menu event
void My_Menu(int id)
{
	switch (id)
	{
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.	
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(defalut_w, defalut_h);
	glutCreateWindow("Game"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif	
	DumpInfo();
	//Call custom initialize function
	My_Init();

	//Define Menu
	int menu_main = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//Register GLUT callback functions
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutKeyboardFunc(keyboardevent);
	//glutKeyboardUpFunc(resetaction);
	
	glutTimerFunc(interval, My_Timer, 0);
	// Enter main event loop.
	glutMainLoop();
	return 0;
}
