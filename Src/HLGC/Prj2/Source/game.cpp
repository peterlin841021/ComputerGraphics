#include "../Include/game.h"

#include <time.h>

Game::Game()
{

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
void Game::Render(glm::mat4 pm, glm::mat4 mm, Uniform uniform, int effect, int type, vector<glm::vec3> pos, vector<glm::vec2> uv)
{
	glUniformMatrix4fv(uniform.mv, 1, GL_FALSE, &mm[0][0]);
	glUniformMatrix4fv(uniform.pm, 1, GL_FALSE, &pm[0][0]);

	glUniform1i(uniform.effect, effect);
	glUniform1i(glGetUniformLocation(sp, "tex"), 0);
	glUniform1i(glGetUniformLocation(sp, "water"), 1);
	glUniform1f(uniform.time, clock());
	glBindBuffer(GL_ARRAY_BUFFER, vvbo);
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

		Particle * particle = (Particle *)glMapBufferRange(GL_ARRAY_BUFFER, 0, particle_num * sizeof(Particle), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for (int i = 0; i < particle_num; i++)
		{
			particle[i].position[0] = (random_float() * 2.0f - 1.0f) * 2.0f;
			particle[i].position[2] = random_float();
			particle[i].position[1] = (random_float() * 2.0f - 1.0f) * 2.0f;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUseProgram(sp_particle);
		glUniformMatrix4fv(glGetUniformLocation(sp_particle, "pm"), 1, GL_FALSE, &pm[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(sp_particle, "mm"), 1, GL_FALSE, &mm[0][0]);
		glUniform1i(glGetUniformLocation(sp_particle, "ps"), particle_size);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), NULL);		
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, particle_num);
	}
}
size_t Game::collisiondetect(glm::vec3 v1, glm::vec3 v2, size_t damage, float attack_distance)
{
	float dis = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
	if (dis <= attack_distance)
		return damage;
	else
		return 0;
}
size_t Game::attackcal(glm::vec3 v1, glm::vec3 v2, size_t damage, float attack_distance)
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
typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} TextureData;

TextureData Load_png(const char* path)
{
	TextureData texture;
	int n;
	stbi_uc *data = stbi_load(path, &texture.width, &texture.height, &n, 4);
	if (data != NULL)
	{
		texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
		memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
		// vertical-mirror image data
		for (int i = 0; i < texture.width; i++)
		{
			for (int j = 0; j < texture.height / 2; j++)
			{
				for (size_t k = 0; k < 4; k++) {
					std::swap(texture.data[(j * texture.width + i) * 4 + k], texture.data[((texture.height - j - 1) * texture.width + i) * 4 + k]);
				}
			}
		}
		stbi_image_free(data);
	}
	return texture;
}
char** LoadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");


	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

//Release 2-dimension array
void FreeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}
void ShaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
}
void Game::init_shader()
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
GLuint Game::generateTexture(const char *image, GLuint id)
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
vector<vector<glm::vec2>> Game::generate_ani_uv(float origin_w, float origin_h, size_t wpart, size_t hpart)
{
	vector<vector<glm::vec2>> output;
	float xstart = 0.0, ystart = 1.0;
	float xoffset = (origin_w / wpart) / origin_w;
	float yoffset = (origin_h / hpart) / origin_h;
	for (size_t i = 0; i < hpart; i++)
	{
		float x = 0;
		for (size_t j = 0; j < wpart; j++)
		{
			vector<glm::vec2> v;
			xstart += xoffset;
			v.push_back(glm::vec2(x, ystart));
			v.push_back(glm::vec2(xstart, ystart));
			v.push_back(glm::vec2(xstart, ystart - yoffset));
			v.push_back(glm::vec2(xstart, ystart - yoffset));
			v.push_back(glm::vec2(x, ystart - yoffset));
			v.push_back(glm::vec2(x, ystart));
			x = xstart;
			output.push_back(v);
		}
		ystart -= yoffset;
		xstart = 0;
	}
	return output;
}
