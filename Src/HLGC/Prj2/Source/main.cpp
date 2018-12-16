#include "..\include\main.h"
#define MENU_EXIT   4


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
	if (dis <= attack_distance)
		return damage;
	else
		return 0;
}
size_t attackcal(vec3 v1, vec3 v2, size_t damage, float attack_distance)
{
	float dis = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));	
	if (dis <= attack_distance)
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
	mat4 identity(1.0);	
	float ground = -0.53f;
	float leftboundary = -0.8f;	
	vector<map<int, float>> attribute;
	characters.push_back(new Character("Background", identity,0,0,0,0,0,40,0,0,0,0,"background.png", "background.png"));
	characters.push_back(new Character("Miku", identity,100,5,0,0,0,0,0,0.3f,0.6f,0.1f,"mikuL.png", "mikuR.png"));	
	characters.push_back(new Character("CrimsonBalrog", identity,9999,9999,1,0,0,-500.f,0,0.1f,0,0.1f,"CrimsonBalrogR.png", "CrimsonBalrogR.png"));
	characters.push_back(new Character("Origin marhroom", identity,5,2,1,0,0,0,0,0.15f,0,0.2f,"mashroom.png", "mashroom.png"));
	characters.push_back(new Character("Pig", identity,5,5,1,0,0,0,0,0.15f,0,0.3f,"pig.png","pig.png"));
	characters.push_back(new Character("Wolf", identity,10,10,1,0,0,0,0,0.1f,0,0.1f,"wolf.png", "wolf.png"));
	characters.push_back(new Character("Suu", identity,50,5,1,0,0,0,0,1000.f,0,0.1f, "suu.png","suu.png"));
	characters.push_back(new Character("Magnus", identity,100,10,1,0,0,0,0,0.2f,0,0.1f,"walk.png","walk.png"));
	characters.push_back(new Character("box", identity,0,0,0,0,0,0,0,0,0,0,"box.png", "box.png"));
	characters.push_back(new Character("attackup", identity,0, 0,1, 0, 0, 0, 0, 0, 0, 0, "attackup.png", "attackup.png"));
	characters.push_back(new Character("hpwater", identity, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "hpwater.png", "hpwater.png"));
	characters.push_back(new Character("Particle sys", identity, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "star.png", "star.png"));
	characters.push_back(new Character("Minimap", identity, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "",""));
	characters.push_back(new Character("Hp", identity, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "hp.png", "hp.png"));
	characters.push_back(new Character("Map frame", identity, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "frame.png", "frame.png"));
	characters.push_back(new Character("Lose", identity, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Lose.png", "Lose.png"));
	characters.push_back(new Character("Hint1", identity, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Hint1.png", "Hint1.png"));
	characters.push_back(new Character("Hint2", identity, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Hint2.png", "Hint2.png"));
	for (size_t i = 0; i < characters.size(); i++)
	{
		if (i == 0)//Scene
		{
			characters[i]->modelview *= translate(identity, vec3(0, 0, -2));
			characters[i]->modelview *= scale(identity, vec3(1, 1, 1));			
			characters[i]->action = generate_ani_uv(1596, 599, 2, 1);			
			origin_left = characters[i]->action[0];
			characters[i]->idle = pair<int, int>(0, 1);
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L,i);
			characters[i]->left = true;
			characters[i]->isappear = true;
		}
		else if (i == 1)
		{
			characters[i]->modelview *= translate(identity, vec3(0, ground, -2));
			characters[i]->modelview *= scale(identity, vec3(0.15f, 0.15f, 0.15f));
			characters[i]->action = generate_ani_uv(1100, 100, 11, 1);
			characters[i]->idle = pair<int, int>(0, 1);
			characters[i]->move = pair<int, int>(1, 4);
			characters[i]->jump = pair<int, int>(5, 1);
			characters[i]->die = pair<int, int>(6, 1);
			characters[i]->injured = pair<int, int>(10, 1);
			vector<pair<int, int>> attacks;
			attacks.push_back(pair<int, int>(7, 3));
			characters[i]->attack = attacks;

			characters[i]->left = false;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L,i);
			characters[i]->textureidR = generateTexture(characters[i]->texture_images_R,i+1);
			characters[i]->isappear = true;
		}			
		else if (i == 2)//CrimsonBalrog
		{			
			characters[i]->modelview *= translate(identity, vec3(-0.9f, -0.2f, -2));
			characters[i]->modelview *= scale(identity, vec3(0.5f, 0.5f, 0.5f));			
			characters[i]->action = generate_ani_uv(1280, 231, 4, 1);
			characters[i]->move = pair<int, int>(0, 4);	
			characters[i]->left = false;
			characters[i]->textureidR = generateTexture(characters[i]->texture_images_L,i+1);
		}
		else if (i == 3)//Origin mashroom
		{			
			characters[i]->modelview *= translate(identity, vec3(0.7f, ground, -2));
			characters[i]->modelview *= scale(identity, vec3(0.1f, 0.1f, 0.1f));			
			characters[i]->action = generate_ani_uv(600, 100, 6, 1);
			characters[i]->move = pair<int, int>(0, 3);
			characters[i]->die = pair<int, int>(3, 3);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L,i + 1);
			characters[i]->state = 1;
		}
		else if (i == 4)//Pig
		{			
			characters[i]->modelview *= translate(identity, vec3(0.7f, ground, -2));
			characters[i]->modelview *= scale(identity, vec3(0.1f, 0.1f, 0.1f));
			characters[i]->action = generate_ani_uv(201, 50, 3, 1);
			characters[i]->move = pair<int, int>(0, 3);
			characters[i]->die = pair<int, int>(0, 3);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->state = 1;
		}
		else if (i == 5)//Wolf
		{				
			characters[i]->modelview *= translate(identity, vec3(0.7f, ground + 0.2f, -2));
			characters[i]->modelview *= scale(identity, vec3(0.3f, 0.3f, 0.3f));
			characters[i]->action = generate_ani_uv(3108, 255, 12, 1);
			characters[i]->move = pair<int, int>(0, 4);
			characters[i]->die = pair<int, int>(4, 8);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->state = 1;
		}
		else if (i == 6)//Suu
		{			
			characters[i]->modelview *= translate(identity, vec3(0.7f, ground + 0.1f, -2.f));
			characters[i]->modelview *= scale(identity, vec3(0.3f, 0.3f, 0.3f));			
			characters[i]->action = generate_ani_uv(2040, 166, 12, 1);
			characters[i]->move = pair<int, int>(0, 12);
			characters[i]->die = pair<int, int>(0, 12);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}		
		else if (i == 7)//Magnus
		{			
			characters[i]->modelview *= translate(identity, vec3(0.7f, ground + 0.2f, -2));
			characters[i]->modelview *= scale(identity, vec3(0.3f, 0.3f, 0.3f));
			characters[i]->action = generate_ani_uv(2912, 197, 8, 1);
			characters[i]->move = pair<int, int>(0, 8);				
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;			
		}	
		else if (i == 8)//box
		{ 			
			characters[i]->modelview *= translate(identity, vec3(-0.2, ground-0.05f, -2));
			characters[i]->modelview *= scale(identity, vec3(0.04f, 0.04f, 0.04f));							
			characters[i]->action = generate_ani_uv(558, 299, 1, 1);
			characters[i]->idle = pair<int, int>(0, 1);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}
		else if (i == 9)//reel
		{
			characters[i]->modelview *= translate(identity, vec3(0, ground - 0.05f, -2));
			characters[i]->modelview *= scale(identity, vec3(0.04f, 0.04f, 0.04f));			
			characters[i]->action = generate_ani_uv(558, 299, 1, 1);
			characters[i]->idle = pair<int, int>(0, 1);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}
		else if (i == 10)//Potion
		{ 			
			characters[i]->modelview *= translate(identity, vec3(0.2, ground - 0.05f, -2));
			characters[i]->modelview *= scale(identity, vec3(0.04f, 0.04f, 0.04f));							
			characters[i]->action = generate_ani_uv(558, 299, 1, 1);
			characters[i]->idle = pair<int, int>(0, 1);
			characters[i]->left = true;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}
		else if (i == 12)//Minimap
		{				
			characters[i]->modelview *= translate(identity, vec3(0.78f, 0.75f, -1.9f));
			characters[i]->modelview *= scale(identity, vec3(0.1f, 0.1f, 0.1f));
		}
		else if (i == 13)//Hp slider
		{
			characters[i]->modelview *= translate(identity, vec3(-0.5f, 0.8f, -2.f));
			characters[i]->modelview *= scale(identity, vec3(0.3f, 0.08f, 0.08f));
			characters[i]->action = generate_ani_uv(13750, 300, 11, 1);
			characters[i]->move = pair<int, int>(0, 11);
			characters[i]->state = 1;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = true;
		}
		else if (i == 14)//Map frame
		{
			characters[i]->modelview *= translate(identity, vec3(0.8f, 0.77f, -1.95f));
			characters[i]->modelview *= scale(identity, vec3(0.12f, 0.14f, 0.12f));
			characters[i]->action = generate_ani_uv(4500, 3000, 1, 1);
			characters[i]->move = pair<int, int>(0, 1);
			characters[i]->state = 1;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = true;
		}
		else if (i == 15)//Lose
		{
			characters[i]->modelview *= translate(identity, vec3(0, 0, -1.8f));
			characters[i]->modelview *= scale(identity, vec3(0.5f, 0.5f, 0.5f));
			characters[i]->action = generate_ani_uv(500, 313, 1, 1);
			characters[i]->move = pair<int, int>(0, 1);
			characters[i]->state = 1;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}
		else if (i == 16)//Hint1
		{
			characters[i]->modelview *= translate(identity, vec3(0.15f, 0.5f, -1.8f));
			characters[i]->modelview *= scale(identity, vec3(0.3f, 0.3f, 0.3f));
			characters[i]->action = generate_ani_uv(600, 338, 1, 1);
			characters[i]->move = pair<int, int>(0, 1);
			characters[i]->state = 1;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}
		else if (i == 17)//Hint2
		{
			characters[i]->modelview *= translate(identity, vec3(0.05f, 0.4f, -1.8f));
			characters[i]->modelview *= scale(identity, vec3(0.2f, 0.2f, 0.2f));
			characters[i]->action = generate_ani_uv(1024, 700, 1, 1);
			characters[i]->move = pair<int, int>(0, 1);
			characters[i]->state = 1;
			characters[i]->textureidL = generateTexture(characters[i]->texture_images_L, i + 1);
			characters[i]->isappear = false;
		}
	}
	//****//
	lake = generateTexture("lake.jpg", characters.size());
	particle = generateTexture("s.png", characters.size()+1);	
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
	uniform->tex = glGetUniformLocation(sp, "tex");
	uniform->effect = glGetUniformLocation(sp, "effect");
	uniform->time = glGetUniformLocation(sp, "time");
	uniform->darken = glGetUniformLocation(sp, "darken");
	particle_dir = vec3(-1, 0, 0);
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
GLuint generateTexture(const char *image, GLuint id)
{	
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

void Render(mat4 pm, mat4 mm, int effect, int type,bool darken,vector<vec3> pos, vector<vec2> uv)
{
	glUseProgram(sp);
	glUniformMatrix4fv(uniform->mv,1, GL_FALSE, &mm[0][0]);
	glUniformMatrix4fv(uniform->pm, 1, GL_FALSE, &pm[0][0]);
	
	glUniform1i(uniform->effect, effect);
	glUniform1i(uniform->tex, 0);
	glUniform1i(glGetUniformLocation(sp, "water"), 1);	
	glUniform1f(uniform->time, clock());
	if(darken)
		glUniform1i(uniform->darken, 1);
	else
		glUniform1i(uniform->darken, 0);

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
		glBufferData(GL_ARRAY_BUFFER, particle_num * sizeof(Particle), NULL, GL_STATIC_DRAW);

		Particle * star = (Particle *)glMapBufferRange(GL_ARRAY_BUFFER, 0, particle_num * sizeof(Particle), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);		
		for (int i = 0; i < particle_num; i++)
		{			
			star[i].position[0] = (random_float() * 2.0f - 1.0f);
			star[i].position[2] = random_float();
			star[i].position[1] = (random_float() * 2.0f - 1.0f);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUseProgram(sp_particle);
		if (darken)
			glUniform1i(glGetUniformLocation(sp_particle, "darken"),1);
		else
			glUniform1i(glGetUniformLocation(sp_particle, "darken"),0);
		glUniformMatrix4fv(glGetUniformLocation(sp_particle, "pm"), 1, GL_FALSE, &pm[0][0]);		
		glUniformMatrix4fv(glGetUniformLocation(sp_particle, "mm"), 1, GL_FALSE, &mm[0][0]);
		glUniform1i(glGetUniformLocation(sp_particle, "ps"), particle_size);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), NULL);
		glEnable(GL_POINT_SPRITE);		
		glEnable(GL_PROGRAM_POINT_SIZE);		
		glDrawArrays(GL_POINTS, 0, particle_num);		
	}
}

void My_Display()
{
	glBindVertexArray(vao);
	//glUseProgram(sp);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Injure detect
	if (!characters[1]->isinjured)
	{
		if (stage == 1)
		{
			for (size_t i = 2; i < characters.size() - 1; i++)
			{
				size_t amount = collisiondetect(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 0), characters[i]->damage, characters[i]->attack_distance);
				if (!characters[i]->isappear)continue;
				if (amount > 0 && !characters[1]->isdied)
				{
					characters[1]->isinjured = true;
					if (characters[1]->hp > 0)
						characters[1]->hp -= amount;
					characters[13]->nextframe = (100 - characters[1]->hp) / 10;					
					if (characters[1]->hp <= 0)
					{
						characters[1]->isdied = true;
						characters[1]->state = ACTION_STATE_DIE;
						characters[13]->nextframe = 10;
						characters[15]->isappear = true;
						isdark = true;
						break;
					}
					printf("Miku hP:%d\n", characters[1]->hp);
				}
			}
		}		
	}
	//Attack detect
	if (characters[1]->state == ACTION_STATE_ATTACK && characters[1]->attackcounter == 2 && characters[1]->state != ACTION_STATE_DIE)
	{
		if (stage == 1)
		{
			for (size_t i = 2; i < characters.size(); i++)
			{
				if (!characters[i]->isappear)continue;
				size_t amount = attackcal(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 0), characters[1]->damage, characters[1]->attack_distance);

				if (amount > 0)
				{
					characters[i]->hp -= amount;
					if (characters[i]->hp == 0)
					{
						characters[i]->state = ACTION_STATE_DIE;
					}
					printf("Monster hp:%d\n", characters[i]->hp);
				}
			}
		}
		else if(stage == 2)
		{
			if (characters[1]->xpos > 4.5f && characters[1]->xpos < 4.6f)
			{				
				characters[1]->isinjured = true;
				characters[1]->hp -= characters[1]->damage;
				characters[13]->nextframe = (100 - characters[1]->hp) / 10;				
				if (characters[1]->hp <= 0)
				{
					characters[1]->isdied = true;
					characters[1]->state = ACTION_STATE_DIE;
					characters[13]->nextframe = 10;
					characters[15]->isappear = true;					
				}
			}
		}		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);//Clear FBO
		
	//Draw all characters
	for (size_t i = 0; i < characters.size(); i++)
	{
		if (!characters[i]->isappear)continue;
		glActiveTexture(GL_TEXTURE0);
		if (i == 0)
		{			
			glBindTexture(GL_TEXTURE_2D, characters[0]->textureidL);
			Render(projection_matrix, characters[0]->modelview,0,0, isdark,square_pos, characters[0]->action[0]);
		}
		else
		{
			if (i == 1)
			{
				//EVENT TRIGER
				if (characters[i]->xpos / characters[0]->xpos > 0.5f)//SUU
				{					
					if (characters[6]->state != ACTION_STATE_DIE)
					{
						if (!characters[6]->isappear)
						{
							characters[6]->isappear = true;
							characters[6]->xpos = characters[i]->xpos + characters[i]->footsetps * 15;
							characters[6]->attack_distance = characters[6]->xpos - characters[1]->xpos;
						}						
						//printf("suu pos :%f\n", characters[6]->xpos);
					}
				}
				if (characters[i]->xpos / characters[0]->xpos > 0.4f && characters[i]->xpos / characters[0]->xpos < 0.45f)//Hint1
				{
					if (!characters[16]->isappear)
					{
						characters[16]->isappear = true;
						isdark = true;
					}
				}
				if (characters[i]->xpos / characters[0]->xpos > 0.1f)//Mashroom
				{
					if (characters[3]->state != ACTION_STATE_DIE)
					{
						if (!characters[3]->isappear)
						{
							characters[3]->isappear = true;
							characters[3]->hp = 5;
							characters[3]->xpos = characters[i]->xpos + (characters[i]->footsetps) * 15;
						}
						printf("Mashroom xpos :%f\n", characters[3]->xpos);
					}
				}
				if (characters[i]->xpos / characters[0]->xpos > 0.2f)//Pig
				{
					if (characters[4]->state != ACTION_STATE_DIE)
					{
						if (!characters[4]->isappear)
						{
							characters[4]->isappear = true;
							characters[4]->xpos = characters[i]->xpos + (characters[i]->footsetps) * 15;
						}
						printf("Pig xpos :%f\n", characters[4]->xpos);
					}									
				}
				if (characters[i]->xpos / characters[0]->xpos > 0.3f )//Wolf
				{
					if (characters[5]->state != ACTION_STATE_DIE)
					{
						if (!characters[5]->isappear)
						{
							characters[5]->isappear = true;
							characters[5]->xpos = characters[i]->xpos + (characters[i]->footsetps) * 15;
						}
						printf("Wolf xpos :%f\n", characters[5]->xpos);
					}
				}
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
					if (characters[i]->nextframe == characters[i]->move.second - 1)
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
					if (characters[i]->nextframe == characters[i]->jump.second - 1)
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
					else if (characters[1]->jumpcounter < 6)
					{
						characters[i]->ypos -= characters[i]->jump_distance;
						characters[1]->modelview *= translate(mat4(1.0), vec3(0, -characters[i]->jump_distance, 0));
						characters[i]->jumpcounter++;
					}
					else if (characters[1]->jumpcounter == 6)
					{
						characters[i]->state = 0;
						characters[1]->isjump = false;
						characters[i]->ypos = -0.7f;
						characters[1]->jumpcounter = 0;
					}
					break;
				case ACTION_STATE_ATTACK:
					action_index = characters[i]->attack[0].first + characters[i]->nextframe;
					if (characters[i]->nextframe == characters[i]->attack[0].second - 1)
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
					characters[i]->modelview = mat4(1.0);
					characters[i]->modelview *= translate(mat4(1.0), vec3(0, -0.57f, -2));
					characters[i]->modelview *= scale(mat4(1.0), vec3(0.15f, 0.15f, 0.15f));
					action_index = characters[i]->die.first;
					break;
				}
				if (electric_resist)
					Render(projection_matrix, characters[i]->modelview, 7, 0, isdark, square_pos, characters[i]->action[action_index]);
				else
					Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[action_index]);
				mat4 another_mv = mat4(1.0);
				if (stage == 2)
				{					
					//another_mv = characters[i]->modelview;
					another_mv *= translate(mat4(1.0),another_pos);
					another_mv *= scale(mat4(1.0), vec3(0.15f, 0.15f, 0.15f));
					if(characters[i]->left)
						glBindTexture(GL_TEXTURE_2D, characters[i]->textureidR);
					else
						glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
					Render(projection_matrix, another_mv, 0, 0, isdark, square_pos, characters[i]->action[action_index]);
				}
				if (characters[i]->isinjured && !characters[1]->isdied)
				{					
					if (stage == 2)
					{
						if (characters[i]->left)
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidR);
						else
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
						Render(projection_matrix, another_mv, 0, 0, isdark, square_pos, characters[i]->action[10]);
					}
					//Real
					if (characters[i]->left)
						glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
					else
						glBindTexture(GL_TEXTURE_2D, characters[i]->textureidR);					
					Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[10]);
					characters[i]->isinjured = false;
				}
			}
			else//Monsters
			{
				//STAGE1
				if (stage == 1)
				{
					if (i == 3)//Mashroom
					{
						if (characters[i]->isappear)
						{
							size_t action_index = 0;
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
							switch (characters[i]->state)
							{
							case ACTION_STATE_MOVE:
								//Move auto
								characters[3]->xpos -= (characters[3]->footsetps / 6);
								characters[3]->modelview *= translate(mat4(1.0), vec3(-characters[3]->footsetps, 0, 0));
								if (characters[3]->xpos < -2)
								{
									characters[3]->isappear = false;
									characters[3]->state = ACTION_STATE_DIE;
								}
								action_index = characters[i]->move.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->move.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 1;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							case ACTION_STATE_DIE:
								action_index = characters[i]->die.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->die.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 4;
									characters[i]->isappear = false;
									characters[8]->isappear = true;
									//electric_resist = true;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							}
							if (characters[i]->state == ACTION_STATE_DIE)
								Render(projection_matrix, characters[i]->modelview, 1, 0, isdark, square_pos, characters[i]->action[action_index]);
							else
								Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[action_index]);
						}
					}
					if (i == 4)//Pig
					{
						if (characters[i]->isappear)
						{
							size_t action_index = 0;
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
							switch (characters[i]->state)
							{
							case ACTION_STATE_MOVE:
								//Move auto
								characters[4]->xpos -= (characters[4]->footsetps / 4);
								characters[4]->modelview *= translate(mat4(1.0), vec3(-characters[4]->footsetps, 0, 0));
								if (characters[4]->xpos < -2)
								{
									characters[4]->isappear = false;
									characters[4]->state = ACTION_STATE_DIE;
								}
								action_index = characters[i]->move.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->move.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 1;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							case ACTION_STATE_DIE:
								action_index = characters[i]->die.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->die.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 4;
									characters[i]->isappear = false;
									characters[9]->isappear = true;
									//characters[1]->damage += 10;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							}
							if (characters[i]->state == ACTION_STATE_DIE)
								Render(projection_matrix, characters[i]->modelview, 1, 0, isdark, square_pos, characters[i]->action[action_index]);
							else
								Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[action_index]);
						}
					}
					if (i == 5)//Wolf
					{
						if (characters[i]->isappear)
						{
							size_t action_index = 0;
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
							switch (characters[i]->state)
							{
							case ACTION_STATE_MOVE:
								//Move auto
								characters[5]->xpos -= (characters[5]->footsetps / 4);
								characters[5]->modelview *= translate(mat4(1.0), vec3(-characters[5]->footsetps, 0, 0));
								if (characters[5]->xpos < -2)
								{
									characters[5]->isappear = false;
									characters[5]->state = ACTION_STATE_DIE;
								}
								action_index = characters[i]->move.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->move.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 1;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							case ACTION_STATE_DIE:
								action_index = characters[i]->die.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->die.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 4;
									characters[i]->isappear = false;
									characters[10]->isappear = true;
									//characters[1]->hp = 100;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							}
							if (characters[i]->state == ACTION_STATE_DIE)
								Render(projection_matrix, characters[i]->modelview, 1, 0, isdark, square_pos, characters[i]->action[action_index]);
							else
								Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[action_index]);
						}
					}
					if (i == 6)//Suu
					{
						if (characters[i]->isappear)
						{
							if (electric_resist)
								characters[i]->attack_distance = 0.05f;
							else
								characters[i]->attack_distance = characters[6]->xpos - characters[1]->xpos;
							size_t action_index = 0;
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
							switch (characters[i]->state)
							{
							case ACTION_STATE_MOVE:
								action_index = characters[i]->move.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->move.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 1;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							case ACTION_STATE_DIE:
								action_index = characters[i]->die.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->die.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 4;
									characters[i]->isappear = false;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;
							}
							if (characters[i]->state == ACTION_STATE_DIE)
								Render(projection_matrix, characters[i]->modelview, 1, 0, isdark, square_pos, characters[i]->action[action_index]);
							else
								Render(projection_matrix, characters[i]->modelview, 8, 0, isdark, square_pos, characters[i]->action[action_index]);
							//Draw electrified particles						
							glBindTexture(GL_TEXTURE_2D, particle);
							Render(projection_matrix, mat4(1.0)*rotate(mat4(1.0), rad2deg(90.f), vec3(0, 0, 1)), 0, 1, isdark, square_pos, square_uv);
						}
					}
					if (i > 7 && i < 11)//Items
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
						Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[0]);
					}
				}
				//STAGE2
				if (stage == 2)
				{
					if (i > 7 && i < 11)//Items
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
						Render(projection_matrix, characters[i]->modelview, 0, 0,isdark, square_pos, characters[i]->action[0]);
					}					
				}
				//BOSS
				if (stage == 3)
				{
					if (i == 7)//Magnus
					{
						if (characters[i]->isappear)
						{
							size_t action_index = 0;
							switch (characters[i]->state)
							{
							case ACTION_STATE_MOVE:
								action_index = characters[i]->move.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->move.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 1;
								}
								else
								{
									characters[i]->nextframe++;
								}
								break;							
							case ACTION_STATE_ATTACK:
								action_index = characters[i]->attack[0].first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->attack[0].second - 1)
								{
									characters[i]->nextframe = 0;
									characters[1]->attackcounter = 0;
									characters[i]->state = 1;									
								}
								else
								{
									characters[i]->nextframe++;
									characters[1]->attackcounter++;
								}
								break;														
							case ACTION_STATE_DIE:
								action_index = characters[i]->die.first + characters[i]->nextframe;
								if (characters[i]->nextframe == characters[i]->die.second - 1)
								{
									characters[i]->nextframe = 0;
									characters[i]->state = 4;
									characters[i]->isappear = false;
								}
								else
								{
									characters[i]->nextframe++;
								}								
								break;
							}
							glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
							Render(projection_matrix, characters[i]->modelview, 0, 0, isdark, square_pos, characters[i]->action[action_index]);
						}//Appear?
					}
				}								
			}			
		}
	}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);				
		mat4 reflection(1.0),identity(1.0),origin(1.0);
		origin *= translate(identity, vec3(0, 0.6f, -2));
		reflection *= translate(identity,vec3(0,-1.4f,-2));
		reflection *= rotate(identity, deg2rad(180.f), vec3(0, 1, 0));
		reflection *= rotate(identity, deg2rad(180.f), vec3(0, 0, 1));
		//HP
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characters[13]->textureidL);
		Render(projection_matrix, characters[13]->modelview, 0, 0, isdark, square_pos, characters[13]->action[(characters[13]->nextframe)% characters[13]->action.size()]);
		//Frame
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characters[14]->textureidL);
		Render(projection_matrix, characters[14]->modelview, 0, 0, isdark, square_pos, square_uv);
		//Lose
		if (characters[15]->isappear)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, characters[15]->textureidL);			
			Render(projection_matrix, characters[15]->modelview, 0, 0, false, square_pos, square_uv);
		}
		//Hint1
		if (characters[16]->isappear)
		{
			characters[16]->isappear = false;
			isdark = false;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, characters[16]->textureidL);
			Render(projection_matrix, characters[16]->modelview, 0, 0, false, square_pos, square_uv);
		}
		if (characters[17]->isappear)
		{
			glBindTexture(GL_TEXTURE_2D, characters[17]->textureidL);
			Render(projection_matrix, characters[17]->modelview, 0, 0, false, square_pos, characters[17]->action[0]);
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lake);
		//Reflection
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characters[12]->textureidL);
		Render(projection_matrix, reflection, 11, 0, isdark,square_pos, square_uv);
		//Origin
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characters[12]->textureidL);
		Render(projection_matrix, origin, 0, 0, isdark, square_pos, square_uv);
		//Mini
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characters[12]->textureidL);
		Render(projection_matrix, characters[12]->modelview, 0, 0,false, square_pos, square_uv);				
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
void keyboardevent(unsigned char key,int x,int y) 
{
	float sc = 0.2f, ground =-0.53f;
	float viewportAspect = (float)current_h / (float)current_w;
	mat4 identity(1.0), mv(1.0);	
	switch (key)
	{
	case 'w':
	case 'W':		
		particle_dir = vec3(0,1,0);
		break;
	case 'd':
	case 'D':
		particle_dir = vec3(1, 0, 0);		
		break;
	case 's':
	case 'S':
		particle_dir = vec3(0, -1, 0);		
		break;
	case 'a':
	case 'A':
		particle_dir = vec3(-1, 0, 0);		
		break;
	case 'z':
	case 'Z':
		//electric_resist = !electric_resist;
		if (!characters[1]->isdied && characters[1]->attackcounter == 0)
		{
			characters[1]->nextframe = 0;
			characters[1]->state = ACTION_STATE_ATTACK;			
		}	
		break;
	case 'x':
	case 'X':
		//Use items
		if (characters[8]->isappear)
		{
			characters[8]->isappear = false;
			electric_resist = true;
		}
		else if (!characters[8]->isappear && characters[9]->isappear)
		{
			characters[9]->isappear = false;
			characters[1]->damage += 5;
		}
		else if (!characters[8]->isappear && !characters[9]->isappear && characters[10]->isappear)
		{
			characters[10]->isappear = false;
			characters[1]->hp = 100;
			characters[13]->nextframe = 0;
		}
		break;
	case '+':
		if(particle_size < 50)
			particle_size += 10;
		break;
	case '-':
		if (particle_size > 10)
			particle_size -= 10;
		break;
	case 'R':
	case 'r':
		isdark = false;
		stage = checkpoint;
		scene_counter = 0;
		characters[0]->action[0] = origin_left;
		if (stage == 1)
		{
			mv *= translate(identity, vec3(0, -0.53f, -2));
			mv *= scale(identity, vec3(0.15f, 0.15f, 0.15f));
			characters[1]->reset(mv, 100, 5, 0, 0, 0, 0, 0, 0.3f, 0.6f, 0.1f);
			characters[1]->isdied = false;
		}
		else if (stage == 2)
		{
			characters[1]->state = 0;			
			characters[1]->hp = 100;
			characters[1]->xpos = 0;
			characters[1]->modelview = mat4(1.0);
			characters[1]->modelview *= translate(mat4(1.0), vec3(-0.8f, -0.53f, -2));
			characters[1]->modelview *= scale(mat4(1.0), vec3(0.15f, 0.15f, 0.15f));
			characters[1]->isdied = false;
		}
		
		projection_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 1000.0f);
		//Enemy
		characters[6]->isappear = false;
		characters[6]->state = 1;
		characters[6]->xpos = 0;
		characters[6]->nextframe = 0;
		electric_resist = false;

		characters[3]->isappear = false;
		characters[3]->state = 1;
		characters[3]->xpos = 0;
		characters[3]->nextframe = 0;
		characters[3]->modelview = mat4(1.0);
		characters[3]->modelview *= translate(identity, vec3(0.7f, ground, -2));
		characters[3]->modelview *= scale(identity, vec3(0.1f, 0.1f, 0.1f));

		characters[4]->isappear = false;
		characters[4]->state = 1;
		characters[4]->xpos = 0;
		characters[4]->nextframe = 0;
		characters[4]->modelview = mat4(1.0);
		characters[4]->modelview *= translate(identity, vec3(0.7f, ground, -2));
		characters[4]->modelview *= scale(identity, vec3(0.1f, 0.1f, 0.1f));

		characters[5]->isappear = false;
		characters[5]->state = 1;
		characters[5]->xpos = 0;
		characters[5]->nextframe = 0;
		characters[5]->modelview = mat4(1.0);
		characters[5]->modelview *= translate(identity, vec3(0.7f, ground + 0.2f, -2));
		characters[5]->modelview *= scale(identity, vec3(0.3f, 0.3f, 0.3f));

		another_pos = vec3(0.8f, -0.53f, -2.f);

		characters[13]->nextframe = 0;//Hp slider
		characters[8]->isappear = false;//Item1
		characters[9]->isappear = false;//Item2
		characters[10]->isappear = false;//Item3
		characters[15]->isappear = false;//Lose
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
		if ((characters[1]->jumpcounter < 3 && characters[1]->isjump) ||(!characters[1]->isjump && characters[1]->jumpcounter == 0))
		{
			if (!characters[1]->isdied)
			{
				characters[1]->state = ACTION_STATE_MOVE;
				if (stage == 1)
				{
					if (characters[0]->xpos - characters[1]->xpos < 100)
					{
						scene_counter--;
						characters[1]->xpos -= characters[1]->footsetps;
						//Monsters
						if (characters[6]->state != ACTION_STATE_DIE && characters[6]->isappear)
						{
							characters[6]->modelview *= translate(mat4(1.0), vec3(0.1, 0, 0));
							characters[6]->hp = 50;
							characters[6]->attack_distance = 1.f;
							characters[6]->state = 1;
						}
						if (characters[3]->state != ACTION_STATE_DIE && characters[3]->isappear)
						{
							characters[3]->modelview *= translate(mat4(1.0), vec3(characters[3]->footsetps, 0, 0));
							characters[3]->hp = 5;
							characters[3]->state = 1;
						}
						if (characters[4]->state != ACTION_STATE_DIE && characters[4]->isappear)
						{
							characters[4]->modelview *= translate(mat4(1.0), vec3(characters[4]->footsetps, 0, 0));
							characters[4]->hp = 5;
							characters[4]->state = 1;
						}
						if (characters[5]->state != ACTION_STATE_DIE && characters[5]->isappear)
						{
							characters[5]->modelview *= translate(mat4(1.0), vec3(characters[5]->footsetps, 0, 0));
							characters[5]->hp = 5;
							characters[5]->state = 1;
						}
						//Scroll background
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
				}
				if (stage == 2)
				{
					if (characters[1]->xpos > 0.f)//Can not step over
					{
						characters[1]->xpos -= 0.1f;
						characters[1]->modelview *= translate(mat4(1.0), vec3(-0.1f, 0, 0));						
						another_pos.x += 0.015f;
						printf("xpos:%f\n", characters[1]->xpos);
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
			if (stage == 1)
			{
				if (characters[0]->xpos - characters[1]->xpos > 0)
				{
					scene_counter++;
					//printf("xpos:%f\n", characters[1]->xpos);
					if (characters[1]->xpos > 39 && characters[1]->xpos < 40)
					{
						characters[1]->xpos = 0;
						characters[1]->modelview = mat4(1.0);
						characters[1]->modelview *= translate(mat4(1.0), vec3(-0.8f, -0.53f, -2));
						characters[1]->modelview *= scale(mat4(1.0), vec3(0.15f, 0.15f, 0.15f));
						if (characters[8]->isappear)
							characters[8]->isappear = false;
						electric_resist = false;
						if (characters[9]->isappear)
						{
							characters[9]->modelview = mat4(1.0);
							characters[9]->modelview *= translate(mat4(1.0), vec3(-0.8f, -0.58f, -2));
							characters[9]->modelview *= scale(mat4(1.0), vec3(0.04f, 0.04f, 0.04f));
						}
						if (characters[10]->isappear)
						{
							characters[10]->modelview = mat4(1.0);
							characters[10]->modelview *= translate(mat4(1.0), vec3(-0.6f, -0.58f, -2));
							characters[10]->modelview *= scale(mat4(1.0), vec3(0.04f, 0.04f, 0.04f));
						}
						another_pos = vec3(0.8f,-0.53f,-2.f);						
						stage = 2;
						checkpoint = 2;						
					}
					if ((electric_resist && characters[6]->isappear) || (!characters[6]->isappear))
					{
						characters[1]->xpos += characters[1]->footsetps;
						//
						if (characters[6]->state != ACTION_STATE_DIE && characters[6]->isappear)
						{
							characters[6]->modelview *= translate(mat4(1.0), vec3(-0.1, 0, 0));
							printf("Suu xpos :%f\n", characters[6]->xpos);
							printf("Miku xpos :%f\n", characters[1]->xpos);
						}
						if (characters[3]->state != ACTION_STATE_DIE && characters[3]->isappear)
						{
							characters[3]->modelview *= translate(mat4(1.0), vec3(-characters[3]->footsetps, 0, 0));
							characters[3]->hp = 5;
							characters[3]->state = 1;
						}
						if (characters[4]->state != ACTION_STATE_DIE && characters[4]->isappear)
						{
							characters[4]->modelview *= translate(mat4(1.0), vec3(-characters[4]->footsetps, 0, 0));
							characters[4]->hp = 5;
							characters[4]->state = 1;
						}
						if (characters[5]->state != ACTION_STATE_DIE && characters[5]->isappear)
						{
							characters[5]->modelview *= translate(mat4(1.0), vec3(-characters[5]->footsetps, 0, 0));
							characters[5]->hp = 10;
							characters[5]->state = 1;
						}
						//Scroll background
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
				}
			}
			if (stage == 2)
			{
				if (characters[1]->xpos <= 4.5f)//Can not step over
				{
					characters[1]->xpos += 0.1f;
					characters[1]->modelview *= translate(mat4(1.0), vec3(0.1f, 0,0));
					printf("xpos:%f\n", characters[1]->xpos);					
					another_pos.x -= 0.015f;
					if (characters[1]->xpos < 4.f && characters[1]->xpos > 3.f)
					{
						characters[17]->isappear = true;//Show hint2
						isdark = true;
					}
					else
					{
						characters[17]->isappear = false;
						isdark = false;
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
		if (stage == 2)
		{
			if (characters[1]->xpos <= 0)
			{
				stage = 3;
				checkpoint = 3;
				characters[7]->isappear = true;				
			}			
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


