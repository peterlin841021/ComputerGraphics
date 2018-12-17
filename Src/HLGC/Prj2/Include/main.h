#include "..\include\Common.h"
#include <map>
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
GLuint sp, sp_particle;
const unsigned int interval = 200;
GLuint vao, vvbo, fbo, depthrbo, vao_fbo, vbo_fbo;
const size_t defalut_w = 800;
const size_t defalut_h = 800;
size_t current_w = 0;
size_t current_h = 0;
size_t particle_num = 100;
size_t particle_size = 30;
size_t particle_density = 100;
size_t stage = 1;
size_t effect = 0;

int scene_counter = 0;
mat4 projection_matrix;
bool boxMoveUp = true;
bool eatAttackUp = false;
bool electric_resist = false;
bool isdark = false;
bool scene_change = false;
GLuint lake;
GLuint particle;
size_t checkpoint = 1;

void Render(mat4 pm, mat4 mm, int effect, int type,bool darken, vector<vec3> pos, vector<vec2> uv);
GLuint generateTexture(const char *image, GLuint id);
static inline float random_float();
struct Particle
{
	glm::vec3     position;
	glm::vec3     color;
};


struct Uniform
{
	GLuint mv;
	GLuint pm;
	GLuint tex;
	GLuint time;
	GLuint effect;
	GLuint darken;
};
struct Character
{
	string charactername;
	mat4 modelview;
	vector<vector<vec2>> action;	
	vector<vector<vec2>> action_die;
	vector<vector<vec2>> action_blade;
	vector<vector<vec2>> action_stub;
	vector<vector<vec2>> action_spur;
	pair<int, int> idle;
	pair<int, int> move;
	pair<int, int> jump;
	vector<pair<int, int>> attack;
	pair<int, int> injured;
	pair<int, int> die;
	size_t nextframe = 0;

	int hp = 0;
	size_t damage = 0;
	size_t state = 0;
	size_t jumpcounter = 0;
	size_t attackcounter = 0;	

	bool isinjured = false;
	bool left = false;
	bool isjump = false;
	bool isdied = false;
	bool isappear = false;
	GLuint textureidL;
	GLuint textureidR;

	GLuint textureidD;//Boss
	GLuint textureidB;
	GLuint textureidS;
	GLuint textureidSP;
	float xpos = 0.f, ypos = 0.f;
	float attack_distance = 0.f;
	float jump_distance = 0.f;
	float scale_ratio = 0.f;
	float footsetps = 0;

	const char* texture_images_L;
	const char* texture_images_R;
	Character(string name, mat4 mv, 
		size_t hp, size_t damage, size_t state, size_t attack_counter, size_t jump_counter,
		float x, float y, float attack_distance, float jump_distance, float footstep,
		const char* tl,const char* tr)
	{
		charactername = name;
		reset(mv,hp,damage,state, attack_counter, jump_counter,x,y, attack_distance, jump_distance, footstep);
		texture_images_L = tl;
		texture_images_R = tr;
	}
	void reset(mat4 mv,size_t h, size_t d, size_t s, size_t ac, size_t jc,
		float x,float y, float ad, float jd, float fs)
	{
		modelview = mv;
		hp = h;
		damage = d;
		state = s;
		attackcounter = ac;
		jumpcounter = jc;
		xpos = x;
		ypos = y;
		attack_distance = ad;
		jump_distance = jd;
		footsetps = fs;
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
vec3 another_move;
vec3 another_pos;