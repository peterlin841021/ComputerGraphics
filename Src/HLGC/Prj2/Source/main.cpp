#include "..\include\Common.h"
#define MENU_EXIT   4

using namespace glm;
using namespace std;

#define ACTION_STATE_IDLE 0
#define ACTION_STATE_MOVE 1
#define ACTION_STATE_JUMP 2
#define ACTION_STATE_ATTACK 3
#define ACTION_STATE_DIE 4

#define HP 0
#define DAMAGE 1
#define STATE 2
#define ATTACK_COUNTER 3
#define JUMP_COUNTER 4
#define XPOS 5
#define YPOS 6
#define ATTACK_DISTANCE 7
#define JUMP_DISTANCE 8
#define FOOTSTEP 9

float currentTime;
GLuint sp,sp_particle;
const unsigned int interval = 100;
GLuint vao, vvbo,fbo,depthrbo,vao_fbo,vbo_fbo;
const size_t defalut_w = 800;
const size_t defalut_h = 800;
size_t current_w = 0;
size_t current_h = 0;
size_t particle_num = 100;
int scene_counter = 0;
mat4 projection_matrix;
bool boxMoveUp = true;
bool eatAttackUp = false;
void Render(mat4 pm,mat4 mm, int effect,int type,vector<vec3> pos, vector<vec2> uv);
static inline float random_float();
struct star_t
{
	glm::vec3     position;
	glm::vec3     color;
};
GLuint generateTexture(const char *image);
GLuint generateEmptyTexture();
GLuint lake;
GLuint particle;
struct Uniform
{
	GLuint mv;
	GLuint pm;
	GLuint time;
	GLuint effect;
};
struct Character 
{
	string charactername;
	mat4 modelview;	
	vector<vector<vec2>> action;
	pair<int, int> idle;	
	pair<int, int> move;
	pair<int, int> jump;
	vector<pair<int, int>> attack;
	pair<int, int> injured;
	pair<int, int> die;
	size_t nextframe = 0;

	size_t hp = 0;
	size_t damage = 0;
	size_t state = 0;
	size_t jumpcounter = 0;
	size_t attackcounter = 0;
	size_t footsetps = 0;

	bool isinjured = false;
	bool left = false;
	bool isjump = false;
	bool isdied = false;

	GLuint textureidL;
	GLuint textureidR;
	float xpos = 0.f, ypos= 0.f;	
	float attack_distance= 0.f;
	float jump_distance= 0.f;
	float scale_ratio= 0.f;
	
	
	Character(string name, mat4 mv, float *attributes)
	{
		charactername = name;		
		reset(mv, attributes);
	}	
	void reset(mat4 mv, float *attributes)
	{		
		modelview = mv;
		hp = attributes[HP];
		damage = attributes[DAMAGE];
		state = attributes[STATE];
		attackcounter = attributes[ATTACK_COUNTER];
		jumpcounter = attributes[JUMP_COUNTER];		
		xpos = attributes[XPOS];
		ypos = attributes[YPOS];
		attack_distance = attributes[ATTACK_DISTANCE];
		jump_distance = attributes[JUMP_DISTANCE];
		footsetps = attributes[FOOTSTEP];
		left = false;
		isinjured = false;
		isjump = false;
		isdied = false;		
		nextframe = 0;		
	}
};
Uniform *uniform;
vector<Character*> characters;
vector<vec3> particles_pos;
vector<vec3> square_pos;
vector<vec2> square_uv;
vector<vec2> origin_left;
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
void init_shader()
{
	sp = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = LoadShaderSource("./vs.glsl");
	char** fsSource = LoadShaderSource("./fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);

	FreeShaderSource(vsSource);
	FreeShaderSource(fsSource);
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
		
	glBindBuffer(GL_ARRAY_BUFFER, vvbo);
	glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), NULL, GL_STATIC_DRAW);
	
	//******************************//
	glGenFramebuffers(1, &fbo);

	sp_particle = glCreateProgram();
	vsSource = LoadShaderSource("./Point_Sprite.vs.glsl");
	fsSource = LoadShaderSource("./Point_Sprite.fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);
	FreeShaderSource(vsSource);
	FreeShaderSource(fsSource);
	glCompileShader(vs);
	glCompileShader(fs);

	ShaderLog(vs);
	ShaderLog(fs);	
	glAttachShader(sp_particle, vs);
	glAttachShader(sp_particle, fs);
	glLinkProgram(sp_particle);
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
	const size_t count = 13;		
	mat4 pm(1.0);
	mat4 identity(1.0);
	mat4 character_mv(1.0);
	mat4 scene_mv(1.0);
	mat4 monster_mv(1.0);
	mat4 box_mv(1.0);
	mat4 attackup_mv(1.0);
	mat4 hpwater_mv(1.0);

	std::vector<float*> character_attributes;
	character_attributes.reserve(count);
	float ground = -0.33f;
	float leftboundary = -0.8f;
	float *attributes = new float[10];
	float *none = new float[10]{0};
	//*Scene 0*//
		attributes[HP] = 0;
		attributes[DAMAGE] = 0;
		attributes[STATE] = 0;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;		
		attributes[XPOS] = 2500;
		attributes[YPOS] = 1.6f;//0.9-(-0.7)
		attributes[ATTACK_DISTANCE] = 0;
		attributes[JUMP_DISTANCE] = 0;
		attributes[FOOTSTEP] = 25;
	character_attributes.push_back(attributes);
	attributes = new float[10];//Reset
	//*Miku 1*//
		attributes[HP] = 1000;
		attributes[DAMAGE] = 0;
		attributes[STATE] = 0;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;		
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0.4f;
		attributes[JUMP_DISTANCE] = 0.6f;
		attributes[FOOTSTEP] = 2;
	character_attributes.push_back(attributes);
	attributes = new float[10];//Reset
	//*Particle sys 2*//
		attributes[HP] = 1;
		attributes[DAMAGE] = 1;
		attributes[STATE] = 1;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;		
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0;
		attributes[JUMP_DISTANCE] = 0;
		attributes[FOOTSTEP] = 0;
	character_attributes.push_back(attributes);
	//*Minimap 3*//
		attributes[HP] = 0;
		attributes[DAMAGE] = 0;
		attributes[STATE] = 1;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;		
		attributes[XPOS] = 0.9f;
		attributes[YPOS] = 0.9f;
		attributes[ATTACK_DISTANCE] = 0;
		attributes[JUMP_DISTANCE] = 0;
		attributes[FOOTSTEP] = 0;
	character_attributes.push_back(attributes);
	//*Items 4*//
		attributes[HP] = 0;
		attributes[DAMAGE] = 0;
		attributes[STATE] = 1;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;		
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0;
		attributes[JUMP_DISTANCE] = 0;
		attributes[FOOTSTEP] = 0;
	character_attributes.push_back(attributes);
	Character* cs[count] =
	{
		new Character("Background",identity,
			character_attributes[0]),
		new Character("Miku",identity,
			character_attributes[1]),
		new Character("CrimsonBalrog",identity,
			none),
		new Character("Origin marhroom",identity,
			none),
		new Character("Pig",identity,
			none),
		new Character("Wolf",identity,
			none),
		new Character("Suu",identity,
			none),		
		new Character("Magnus",identity,
			none),		
		new Character("box",identity,
			character_attributes[4]),
		new Character("attackup",identity,
			character_attributes[4]),
		new Character("hpwater",identity,
			character_attributes[4]),
		new Character("Particle sys",identity,
			character_attributes[2]),
		new Character("Minimap",identity,
			character_attributes[3])
	};
	const char* texture_images[count+1] = { 
		"background.png",
		"mikuL.png",
		"mikuR.png",
		"CrimsonBalrogR.png",
		"mashroom.png",
		"pig.png",
		"suu.png",
		"wolf.png",
		"walk.png",		
		"box.png",
		"attackup.png",
		"hpwater.png",
		"star.png"
	};		
	for (size_t i = 0; i < count; i++)
	{
		if (i == 0)//Scene
		{
			scene_mv *= translate(identity, vec3(0, 0.2f, -2));
			scene_mv *= scale(identity, vec3(1, 1, 1));
			cs[i]->modelview = scene_mv;
			cs[i]->action = generate_ani_uv(1596, 599, 2, 1);
			//*******//
			origin_left = cs[i]->action[0];
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->textureidL = generateTexture(texture_images[i]);
			cs[i]->left = true;
		}
		else if (i == 1)
		{
			character_mv *= translate(identity, vec3(0, ground, -2));
			character_mv *= scale(identity, vec3(0.15f, 0.15f, 0.15f));
			cs[i]->modelview = character_mv;
			cs[i]->action = generate_ani_uv(1100, 100, 11, 1);
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->move = pair<int, int>(1, 4);
			cs[i]->jump = pair<int, int>(5, 1);
			cs[i]->die = pair<int, int>(6, 1);
			cs[i]->injured = pair<int, int>(10, 1);
			vector<pair<int, int>> attacks;
			attacks.push_back(pair<int, int>(7, 3));
			cs[i]->attack = attacks;

			cs[i]->left = false;
			cs[i]->textureidL = generateTexture(texture_images[i]);
			cs[i]->textureidR = generateTexture(texture_images[i + 1]);
		}			
		//else if (i == 2)//CrimsonBalrog
		//{
		//	monster_mv = mat4(1.0);
		//	monster_mv *= translate(identity, vec3(-0.9f, -0.2f, -2));
		//	monster_mv *= scale(identity, vec3(0.5f, 0.5f, 0.5f));
		//	cs[i]->modelview = monster_mv;
		//	cs[i]->action = generate_ani_uv(1280, 231, 4, 1);
		//	cs[i]->move = pair<int, int>(0, 4);	
		//	cs[i]->left = false;
		//	cs[i]->textureidR = generateTexture(texture_images[i+1]);
		//}
		//else if (i == 3)//Origin mashroom
		//{
		//	monster_mv = mat4(1.0);
		//	monster_mv *= translate(identity, vec3(3.f, -1.f, -2));
		//	monster_mv *= scale(identity, vec3(0.2f, 0.2f, 0.2f));
		//	cs[i]->modelview = monster_mv;
		//	cs[i]->action = generate_ani_uv(600, 100, 6, 1);
		//	cs[i]->move = pair<int, int>(0, 3);
		//	cs[i]->die = pair<int, int>(3, 3);
		//	cs[i]->left = true;
		//	cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		//}
		//else if (i == 4)//Pig
		//{
		//	monster_mv = mat4(1.0);
		//	monster_mv *= translate(identity, vec3(3.f, 0.f, -2));
		//	monster_mv *= scale(identity, vec3(1.f, 1.f, 1.f));
		//	cs[i]->modelview = monster_mv;
		//	cs[i]->action = generate_ani_uv(201, 50, 3, 1);
		//	cs[i]->move = pair<int, int>(0, 3);				
		//	cs[i]->left = true;
		//	cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		//}
		//else if (i == 5)//Suu
		//{
		//	monster_mv = mat4(1.0);
		//	monster_mv *= translate(identity, vec3(0, 0.5f, -2));
		//	monster_mv *= scale(identity, vec3(0.8f, 0.8f, 0.8f));
		//	cs[i]->modelview = monster_mv;
		//	cs[i]->action = generate_ani_uv(2040, 166, 12, 1);
		//	cs[i]->move = pair<int, int>(0, 4);
		//	cs[i]->die = pair<int, int>(4, 8);
		//	cs[i]->left = true;
		//	cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		//}
		//else if (i == 6)//Wolf
		//{	
		//	monster_mv = mat4(1.0);
		//	monster_mv *= translate(identity, vec3(0.f, 1.f, -2));
		//	monster_mv *= scale(identity, vec3(5.f, 5.f, 5.f));
		//	cs[i]->modelview = monster_mv;
		//	cs[i]->action = generate_ani_uv(3108, 255, 12, 1);
		//	cs[i]->move = pair<int, int>(0, 12);
		//	cs[i]->left = true;
		//	cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		//}
		//else if (i == 7)//Magnus
		//{
		//	monster_mv = mat4(1.0);
		//	monster_mv *= translate(identity, vec3(0, 0, -2));
		//	monster_mv *= scale(identity, vec3(1.f, 1.f, 1.f));
		//	cs[i]->modelview = monster_mv;
		//	cs[i]->action = generate_ani_uv(2912, 197, 8, 1);
		//	cs[i]->move = pair<int, int>(0, 8);				
		//	cs[i]->left = true;
		//	cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		//}	
		else if (i == 8) 
		{ 
			monster_mv = mat4(1.0);
			box_mv *= translate(identity, vec3(-0.2, ground-0.05f, -2));
			box_mv *= scale(identity, vec3(0.04f, 0.04f, 0.04f));
				
			cs[i]->modelview = box_mv;
			cs[i]->action = generate_ani_uv(558, 299, 1, 1);
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->left = true;
			cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		}
		else if (i == 9) 
		{
			attackup_mv *= translate(identity, vec3(0, ground - 0.05f, -2));
			attackup_mv *= scale(identity, vec3(0.04f, 0.04f, 0.04f));
			cs[i]->modelview = attackup_mv;
			cs[i]->action = generate_ani_uv(558, 299, 1, 1);
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->left = true;
			cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		}
		else if (i == 10) 
		{ 			
			hpwater_mv *= translate(identity, vec3(0.2, ground - 0.05f, -2));
			hpwater_mv *= scale(identity, vec3(0.04f, 0.04f, 0.04f));
				
			cs[i]->modelview = hpwater_mv;
			cs[i]->action = generate_ani_uv(558, 299, 1, 1);
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->left = true;
			cs[i]->textureidL = generateTexture(texture_images[i + 1]);
		}
		else if (i == 12)//Minimap
		{				
			cs[i]->modelview *= translate(identity, vec3(0.8f, 0.8f, -1.9f));
			cs[i]->modelview *= scale(identity, vec3(0.2f, 0.2f, 0.2f));
		}		
		characters.push_back(cs[i]);
	}
	//****//
	
	for (size_t i = 0; i < particle_num; i++)
	{
		particles_pos.push_back(vec3( (random_float() * 2.0f - 1.0f) * 100.f, (random_float() * 2.0f - 1.0f) * 100.f, random_float()));
	}
	//Position of square
	square_pos.push_back(vec3(-1, 1, 0));
	square_pos.push_back(vec3(1, 1, 0));
	square_pos.push_back(vec3(1, -1, 0));
	square_pos.push_back(vec3(1, -1, 0));
	square_pos.push_back(vec3(-1, -1, 0));
	square_pos.push_back(vec3(-1, 1, 0));
	//UV of square
	square_uv.push_back(vec2(0, 1));
	square_uv.push_back(vec2(1, 1));
	square_uv.push_back(vec2(1, 0));
	square_uv.push_back(vec2(1,0));
	square_uv.push_back(vec2(0, 0));	
	square_uv.push_back(vec2(0, 1));		
	//*******************//	
	init_shader();
	//Uniform variables
	uniform = new Uniform();
	uniform->pm = glGetUniformLocation(sp, "pm");
	uniform->mv = glGetUniformLocation(sp, "mm");
	uniform->effect = glGetUniformLocation(sp, "effect");
	uniform->time = glGetUniformLocation(sp, "time");	
	lake = generateTexture("lake.jpg");
	particle = generateTexture("star.png");
}

static unsigned int seed = 0x13371337;
static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
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
GLuint generateEmptyTexture()
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, defalut_w, defalut_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	return id;
}
GLuint generateReflection()
{
	GLuint id;
	return id;
}
void Render(mat4 pm, mat4 mm, int effect, int type, vector<vec3> pos, vector<vec2> uv)
{
	glUniformMatrix4fv(uniform->mv,1, GL_FALSE, &mm[0][0]);
	glUniformMatrix4fv(uniform->pm, 1, GL_FALSE, &pm[0][0]);
	
	glUniform1i(uniform->effect, effect);
	glUniform1i(glGetUniformLocation(sp,"tex"), 0);
	glUniform1i(glGetUniformLocation(sp, "water"), 1);
	glUniform1f(uniform->time, currentTime);	
	glBindBuffer(GL_ARRAY_BUFFER,vvbo);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	
	vector<float> vtx;//Puts all data with float type
	for (size_t i = 0; i < pos.size(); i++)
	{
		vtx.push_back(pos[i].x);
		vtx.push_back(pos[i].y);
		vtx.push_back(pos[i].z);
	}
	
	for (size_t i = 0; i < uv.size(); i++)
	{
		vtx.push_back(uv[i].x);
		vtx.push_back(uv[i].y);
	}	
	memcpy(ptr, vtx.data(), vtx.size() * sizeof(float));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(pos.size() * 3 * sizeof(float)));
	
	if (type == 0)
	{		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, pos.size());
	}		
	else if (type == 1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vvbo);
		glBufferData(GL_ARRAY_BUFFER, particle_num * sizeof(star_t), NULL, GL_STATIC_DRAW);

		star_t * star = (star_t *)glMapBufferRange(GL_ARRAY_BUFFER, 0, particle_num * sizeof(star_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		
		for (int i = 0; i < particle_num; i++)
		{
			star[i].position[0] = (random_float() * 2.0f - 1.0f) * 2.0f;			
			star[i].position[2] = random_float();
			star[i].position[1] = (random_float() * 2.0f - 1.0f) * 2.0f;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUseProgram(sp_particle);		
		glUniformMatrix4fv(glGetUniformLocation(sp_particle, "pm"), 1, GL_FALSE, &pm[0][0]);		
		glUniformMatrix4fv(glGetUniformLocation(sp_particle, "mm"), 1, GL_FALSE, &mm[0][0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), NULL);		
		//glEnableVertexAttribArray(0);		
		glEnable(GL_POINT_SPRITE);		
		glEnable(GL_PROGRAM_POINT_SIZE);		
		glDrawArrays(GL_POINTS, 0, particle_num);		
	}
}

void My_Display()
{	
	//Time	
	float f_timer_cnt = glutGet(GLUT_ELAPSED_TIME);
	currentTime = f_timer_cnt * 0.001f;	
	currentTime *= 0.1f;
	currentTime -= floor(currentTime);
	//glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
	glUseProgram(sp);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Injure detect
	if (!characters[1]->isinjured)
	{
		for (size_t i = 2; i < characters.size() - 1; i++)
		{
			size_t amount = collisiondetect(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 0), characters[i]->damage, characters[i]->attack_distance);
			if (amount > 0 && characters[i]->state != ACTION_STATE_DIE)
			{
				characters[1]->isinjured = true;
				if (characters[1]->hp > 0)
					characters[1]->hp -= amount;
				if (characters[1]->hp == 0)
				{
					characters[1]->isdied = true;
					characters[1]->state = ACTION_STATE_DIE;
				}
				printf("Miku hP:%d\n", characters[1]->hp);
			}
		}
	}
	//Attack detect
	if (characters[1]->state == ACTION_STATE_ATTACK && characters[1]->attackcounter == 3)
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
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);//Clear FBO
	
	for (size_t i = 0; i < characters.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		if (i == 0)
		{			
			glBindTexture(GL_TEXTURE_2D, characters[0]->textureidL);
			Render(projection_matrix, characters[0]->modelview,0,0,square_pos, characters[0]->action[0]);
		}
		else if(i == 1)
		{
			//Miku directions			
			if (characters[i]->left)
			{				
				glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
			}
			else
			{				
				glBindTexture(GL_TEXTURE_2D, characters[i]->textureidR);
			}
			size_t action_index = 0;
			switch (characters[i]->state)
			{
			case ACTION_STATE_IDLE:
				action_index = characters[i]->idle.first;				
				break;
			case ACTION_STATE_MOVE:
				action_index = characters[i]->move.first + characters[i]->nextframe;			
				if (characters[i]->nextframe == characters[i]->move.second -1)
				{
					characters[i]->nextframe = 0;
					characters[i]->state = 0;
				}
				else
				{					
					characters[i]->nextframe++;
				}
				break;
			case ACTION_STATE_JUMP:
				action_index = characters[i]->jump.first + characters[i]->nextframe;
				if (characters[i]->nextframe == characters[i]->jump.second -1)
				{
					characters[i]->nextframe = 0;					
				}
				else
				{					
					characters[i]->nextframe++;
				}
					if (characters[i]->jumpcounter < 3)
					{
						characters[i]->ypos += characters[i]->jump_distance;
						characters[1]->modelview *= translate(mat4(1.0), vec3(0, characters[i]->jump_distance, 0));					
						characters[1]->jumpcounter++;
					}				
					else if(characters[1]->jumpcounter < 6)
					{
						characters[i]->ypos -= characters[i]->jump_distance;
						characters[1]->modelview *= translate(mat4(1.0), vec3(0, -characters[i]->jump_distance, 0));				
						characters[i]->jumpcounter ++;
					}
					else if (characters[1]->jumpcounter == 6)
					{
						characters[i]->state = 0;
						characters[1]->isjump = false;
						characters[i]->ypos =-0.7f;				
						characters[1]->jumpcounter = 0;
					}
				break;
			case ACTION_STATE_ATTACK:
				action_index = characters[i]->attack[0].first + characters[i]->nextframe;
				if (characters[i]->nextframe == characters[i]->attack[0].second-1)
				{
					characters[i]->nextframe = 0;
					characters[1]->attackcounter = 0;
					characters[i]->state = 0;					
					if (characters[1]->jumpcounter > 0)
					{
						characters[i]->state = ACTION_STATE_JUMP;
					}						
				}
				else
				{					
					characters[i]->nextframe++;
					characters[1]->attackcounter++;
				}
				break;
			case ACTION_STATE_DIE:
				action_index = characters[i]->die.first;				
				break;
			}			
			Render(projection_matrix, characters[i]->modelview,0,0,square_pos, characters[i]->action[action_index]);			
			if (characters[i]->isinjured && !characters[1]->isdied)
			{
				Render(projection_matrix, characters[i]->modelview,0,0,square_pos, characters[i]->action[10]);
				characters[i]->isinjured = false;
			}				
		}				
		else if (i == 8 || i == 9 || i == 10)
		{			
			if (characters[i]->ypos == 5.0f) 
			{
				boxMoveUp = false;
			}
			else if (characters[i]->ypos == 0.0f) 
			{
				boxMoveUp = true;
			}
			if (boxMoveUp) 
			{
				characters[i]->ypos += 1.f;
				characters[i]->modelview *= translate(mat4(1.0), vec3(0, 0.08f, 0));
			}
			else 
			{
				characters[i]->ypos -= 1.f;
				characters[i]->modelview *= translate(mat4(1.0), vec3(0, -0.08f, 0));
			}			
			glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
			Render(projection_matrix, characters[i]->modelview,0,0,square_pos, characters[i]->action[0]);
		}		
	}
	//Draw particles
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particle);	
	Render(projection_matrix, mat4(1.0)*translate(mat4(1.0), vec3(-5, 0, 0)), 0, 1, square_pos, square_uv);
	
	glUseProgram(sp);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);		
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);*/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characters[12]->textureidL);
		
		mat4 reflection(1.0),identity(1.0);
		reflection *= translate(identity,vec3(0,-1.4f,-2));
		reflection *= rotate(identity, deg2rad(180.f), vec3(0, 1, 0));
		reflection *= rotate(identity, deg2rad(180.f), vec3(0, 0, 1));
		reflection *= scale(identity, vec3(1.2, 1.2, 1));
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lake);
		//Reflection
		Render(projection_matrix, reflection, 11, 0, square_pos, square_uv);
		//Origin
		Render(projection_matrix, characters[0]->modelview, 0, 0, square_pos, square_uv);
		//Mini		
		Render(projection_matrix, characters[12]->modelview, 0, 0, square_pos, square_uv);
		
		
	glDisable(GL_BLEND);
	
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);	
}
//Call to resize the window
void My_Reshape(int width, int height)
{	
	glViewport(0, 0, width, height);
	current_w = width;
	current_h = height;
	float viewportAspect = (float)current_h / (float)current_w;
	projection_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 1000.0f);		
	glDeleteTextures(1, &characters[12]->textureidL);
	glGenTextures(1, &characters[12]->textureidL);
	glBindTexture(GL_TEXTURE_2D, characters[12]->textureidL);
	
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, current_w, current_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, characters[12]->textureidL,0);
}
//Kayboard input
mat4 camrea_mv(1.0);
void keyboardevent(unsigned char key,int x,int y) 
{
	float sc = 0.2f, ground =-0.7f;
	float viewportAspect = (float)current_h / (float)current_w;
	mat4 identity(1.0), mv(1.0);
	float *attributes = new float[10]{0};
	switch (key)
	{
	case 'w':
	case 'W':
		projection_matrix *= translate(mat4(1.0), vec3(0, 0, 1));
		break;
	case 'd':
	case 'D':		
		projection_matrix *= translate(mat4(1.0), vec3(1, 0, 0));
		break;
	case 's':
	case 'S':
		projection_matrix *= translate(mat4(1.0), vec3(0, 0, -1));
		break;
	case 'a':
	case 'A':
		projection_matrix *= translate(mat4(1.0), vec3(-1, 0, 0));
		break;
	case 'z':
	case 'Z':
		if (!characters[1]->isdied && characters[1]->attackcounter == 0)
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
		attributes[HP] = 1000;
		attributes[DAMAGE] = 1;
		attributes[STATE] = 0;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;		
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0.4f;
		attributes[JUMP_DISTANCE] = 0.6f;
		attributes[FOOTSTEP] = 2;
		mv *= translate(identity,vec3(0, -0.33f,-2));
		mv *= scale(identity, vec3(0.15f, 0.15f,0.15f));
		characters[1]->reset(mv, attributes);		
		projection_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 1000.0f);		
		break;
	default:		
		break;
	}	
}
void specialkeyevent(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		characters[1]->attackcounter = 0;
		characters[1]->left = true;
		if ((characters[1]->jumpcounter < 3 && characters[1]->isjump) ||
			(!characters[1]->isjump && characters[1]->jumpcounter == 0))
		{
			if (!characters[1]->isdied)
			{
				characters[1]->state = ACTION_STATE_MOVE;
				float d = (float)characters[1]->footsetps / characters[0]->footsetps;
				if (characters[0]->xpos - characters[1]->xpos < 2500)
				{
					scene_counter--;
					characters[1]->xpos -= characters[1]->footsetps;
					if (scene_counter < -1)
					{
						scene_counter = 0;
						for (size_t i = 0; i < 6; i++)
						{
							characters[0]->action[0][i].x -= 0.01f;
							if (characters[0]->action[0][0].x < origin_left[0].x)
							{
								characters[0]->action[0][0].x = characters[0]->action[1][0].x;
								characters[0]->action[0][1].x = characters[0]->action[1][1].x;
								characters[0]->action[0][2].x = characters[0]->action[1][2].x;
								characters[0]->action[0][3].x = characters[0]->action[1][3].x;
								characters[0]->action[0][4].x = characters[0]->action[1][4].x;
								characters[0]->action[0][5].x = characters[0]->action[1][5].x;
								break;
							}
						}
					}
				}
				if (characters[1]->isjump)
					characters[1]->state = ACTION_STATE_JUMP;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		characters[1]->attackcounter = 0;
		characters[1]->left = false;
		if ((characters[1]->jumpcounter < 3 && characters[1]->isjump || !characters[1]->isjump && characters[1]->jumpcounter == 0) && !characters[1]->isdied)
		{
			characters[1]->state = ACTION_STATE_MOVE;
			float d = (float)characters[1]->footsetps / characters[0]->footsetps;
			if (characters[0]->xpos - characters[1]->xpos > 0)
			{
				scene_counter++;
				characters[1]->xpos += characters[1]->footsetps;
				if (scene_counter > 1)
				{
					scene_counter = 0;
					for (size_t i = 0; i < 6; i++)
					{
						characters[0]->action[0][i].x += 0.01f;
						if (characters[0]->action[0][1].x > characters[0]->action[1][1].x)
						{
							characters[0]->action[0][0].x = origin_left[0].x;
							characters[0]->action[0][1].x = origin_left[1].x;
							characters[0]->action[0][2].x = origin_left[2].x;
							characters[0]->action[0][3].x = origin_left[3].x;
							characters[0]->action[0][4].x = origin_left[4].x;
							characters[0]->action[0][5].x = origin_left[5].x;
							break;
						}
					}
				}
			}
			if (characters[1]->isjump)
				characters[1]->state = ACTION_STATE_JUMP;
		}
		break;
	case GLUT_KEY_UP:
		characters[1]->attackcounter = 0;
		if (!characters[1]->isjump && !characters[1]->isdied)
		{
			characters[1]->isjump = true;
			characters[1]->nextframe = 0;
			characters[1]->state = ACTION_STATE_JUMP;
		}
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
	glutSpecialFunc(specialkeyevent);
	glutTimerFunc(interval, My_Timer, 0);
	// Enter main event loop.
	glutMainLoop();
	return 0;
}


