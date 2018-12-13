#include "GLEW/glew.h"
#include "GLM/glm/glm.hpp"
#include "GLM/glm/gtc/matrix_transform.hpp"
#include "GLM/glm/gtc/type_ptr.hpp"
#include "GLM/glm/gtx/rotate_vector.hpp"
#include <iostream>
#include <vector>
#include "FreeGLUT/freeglut.h"
#include "STB/stb_image.h"
using namespace glm;
using namespace std;

#define ACTION_STATE_IDLE 0
#define ACTION_STATE_MOVE 1
#define ACTION_STATE_JUMP 2
#define ACTION_STATE_ATTACK 3
#define ACTION_STATE_DIE 4

#define MENU_EXIT   4

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


class Game
{
private:

public:
	GLuint sp, sp_particle;
	GLuint vao, vvbo, fbo, depthrbo, vao_fbo, vbo_fbo;
	GLuint lake;
	GLuint particle;
	const size_t defalut_w = 800;
	const size_t defalut_h = 800;
	const unsigned int interval = 100;

	size_t particle_num = 100;
	size_t particle_size = 1;

	int scene_counter = 0;

	bool boxMoveUp = true;
	bool eatAttackUp = false;
	bool electric_resist = true;	
	glm::mat4 projection_matrix;

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
		glm::mat4 modelview;
		vector<vector<glm::vec2>> action;
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


		bool isinjured = false;
		bool left = false;
		bool isjump = false;
		bool isdied = false;
		bool isappear = false;
		GLuint textureidL;
		GLuint textureidR;
		float xpos = 0.f, ypos = 0.f;
		float attack_distance = 0.f;
		float jump_distance = 0.f;
		float scale_ratio = 0.f;
		float footsetps = 0;

		Character(string name, glm::mat4 mv, float *attributes)
		{
			charactername = name;
			reset(mv, attributes);
		}
		void reset(glm::mat4 mv, float *attributes)
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
	struct Particle
	{
		glm::vec3     position;
		glm::vec3     color;
	};
	
	vector<glm::vec3> particles_pos;
	vector<glm::vec3> square_pos;
	vector<glm::vec2> square_uv;
	vector<glm::vec2> origin_left;	
	
	Game();
	void init_shader();
	GLuint generateTexture(const char *image, GLuint id);
	void Render(glm::mat4 pm, glm::mat4 mm,Uniform uniform, int effect, int type, vector<glm::vec3> pos, vector<glm::vec2> uv);
	size_t collisiondetect(glm::vec3 v1, glm::vec3 v2, size_t damage, float attack_distance);
	size_t attackcal(glm::vec3 v1, glm::vec3 v2, size_t damage, float attack_distance);
	vector<vector<glm::vec2>> generate_ani_uv(float origin_w, float origin_h, size_t wpart, size_t hpart);	
};