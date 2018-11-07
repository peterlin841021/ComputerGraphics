#include"quiz.h"
float rotatedangle = 0.f;
float hindarmangle = 0,plamangle =0;
float scenerotateangle = 0;
struct ModelOBJ {
	std::vector<float> vs;
	std::vector<int> fs;
	ModelOBJ() {
		
	}
	ModelOBJ(std::vector<float> v, std::vector<int> f) {
		vs = v;
		fs = f;
	}
};
ModelOBJ miku;

void load3Dmodel(char *modelpath) {
	FILE  *file;
	file = fopen(modelpath, "r");
	int count = 0;
	if (file != NULL) {
		char buffer[128];
		while (true) {
			int response = fscanf(file, "%s", buffer);
			if (response == EOF) {
				break;
			}
			else {
				if (strcmp(buffer, "v") == 0)
				{
					float x, y, z;
					fscanf(file, "%f %f %f", &x, &y, &z);
					//vec3 v(x,y,z);
					miku.vs.push_back(x);
					miku.vs.push_back(y);
					miku.vs.push_back(z);
				}
				if (strcmp(buffer, "f") == 0)
				{
					int x1, y1, z1,
						x2, y2, z2,
						x3, y3, z3;
					fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3);
					miku.fs.push_back(x1 - 1);
				/*	miku.fs.push_back(y1 - 1);
					miku.fs.push_back(z1 - 1);*/
					miku.fs.push_back(x2 - 1);
				/*	miku.fs.push_back(y2 - 1);
					miku.fs.push_back(z2 - 1);*/
					miku.fs.push_back(x3 - 1);
					/*miku.fs.push_back(y3 - 1);
					miku.fs.push_back(z3 - 1);*/
				}
			}
		}
		fclose(file);
	}
	else {
		printf("Can not read model!");
	}
}

GLchar* readShaderSource(string path) 
{	
	ifstream in;
	in.open(path, ios::binary);
	if (in) 
	{
		GLchar *source;
		in.seekg(0, in.end);
		long fileSize = in.tellg();
		in.seekg(0, in.beg);

		source = new char[fileSize];
		in.read(source, fileSize);
		in.close();
		source[fileSize] = '\0';
		return source;
	}
	else {
		printf("%s\n", "Read file fail!");
		return NULL;
	}	
}

mat4 GetProjectionMatrix(float aspect, bool isortho, float zoom = 1.0)
{
	mat4 projectionMatrix;
	float nearVal;
	float farVal;
	nearVal = 10.f;
	farVal = 200.0f;
	if (isortho) {
		float size = 1.5f * zoom;
		projectionMatrix = glm::ortho(-aspect * size, aspect * size, -size, size, nearVal, farVal);
	}
	else {		
		projectionMatrix = perspective(radians(50.f * zoom), aspect, 0.1f, 100.0f);
	}
	return projectionMatrix;
}

void reset(unsigned char keycode, int x, int y) 
{
	switch (keycode) 
	{
	case 'r':
	case 'R':
		scenerotateangle = 0;
		break;
	}
}
void drawOBJ() 
{
	glUseProgram(program);
	glBindVertexArray(triangle_vao);
	mat4 identity(1.0);
	mat4 mm = translate(identity, vec3(0, 0, -3));
	modelviewmatrix = mm;
	float scale_v = 0.005f;
	modelviewmatrix = scale(modelviewmatrix,vec3(scale_v, scale_v, scale_v));
	modelviewmatrix = rotate(modelviewmatrix, radians(scenerotateangle), vec3(0, 1, 0));
	int size = miku.fs.size();
	float *vs = new float[size];
	float *colors = new float[size];	
	for (int i = 0; i < size; i++) 
	{
		vs[i] = miku.vs[miku.fs[i]];
		colors[i] = 0.5f;
	}
	//glEnable(GL_ALPHA);
	drawTriangle(vs, colors, size, size,modelviewmatrix,projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, size);
	//glDisable(GL_ALPHA);
}
void shader_init() 
{
	glewInit();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar *vscode = readShaderSource("./Src/HLGC/shader/triangle.vs.glsl");
	GLchar *fscode = readShaderSource("./Src/HLGC/shader/triangle.fs.glsl");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	if(vscode != NULL && fscode != NULL) 
	{
		glShaderSource(vs, 1, &vscode, NULL);
		glShaderSource(fs, 1, &fscode, NULL);
		glCompileShader(vs);
		glCompileShader(fs);
		program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glGenVertexArrays(1, &triangle_vao);
		glBindVertexArray(triangle_vao);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);						
		mv_location = glGetUniformLocation(program, "mm");
		proj_location = glGetUniformLocation(program, "pm");
	}
	else 
	{
		printf("Initialize shader fail\n");
		system("pause");
	}
}
void drawArm() {
	glUseProgram(program);
	glBindVertexArray(triangle_vao);
	//Transformation
	mat4 identity(1.0);
	mat4 mm = translate(identity, vec3(1.f, 1.5f, -4.0f));
	mm = rotate(mm, degrees(scenerotateangle), vec3(0, 1, 0));
	mm = rotate(mm,degrees(-15.f), vec3(1, 0, 0));
	mm = rotate(mm, degrees(-15.f), vec3(0, 0, 1));
	modelviewmatrix = mm;
	float color_spade[][3]{ {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f}};
	float forearm[]
	{
		//front
		-1.f,-1.f,0.5f,
		-0.5f,0.f,0.5f,
		-0.3f,-0.2f,0.5f,

		-0.3f,-0.2f,0.5f,
		-0.5f,-1.f,0.5f,
		-1.f,-1.f,0.5f,
		//back
		-1.f,-1.f,0.f,
		-0.5f,0.f,0.f,
		-0.3f,-0.2f,0.f,

		-0.3f,-0.2f,0.f,
		-0.5f,-1.f,0.f,
		-1.f,-1.f,0.f,
		//left
		-1.f,-1.f,0.5f,
		-1.f,-1.f,0.f,
		-0.5f,0.f,0.f,

		-0.5f,0.f,0.f,
		-0.5f,0.f,0.5f,
		-1.f,-1.f,0.5f,
		//right
		-0.5f,-1.f,0.5f,
		-0.5f,-1.f,0.f,
		-0.3f,-0.2f,0.f,

		-0.3f,-0.2f,0.f,
		-0.3f,-0.2f,0.5f,
		-0.5f,-1.f,0.5f
	};
	float hindarm[]
	{
		//front
		-0.45f,0.1f,0.5f,
		-0.3f,0.5f,0.5f,
		-0.1f,0.5f,0.5f,

		-0.1f,0.5f,0.5f,
		-0.25f,-0.1f,0.5f,
		-0.45f,0.1f,0.5f,

		////back
		-0.45f,0.1f,0.f,
		-0.3f,0.5f,0.f,
		-0.1f,0.5f,0.f,

		-0.1f,0.5f,0.f,
		-0.25f,-0.1f,0.f,
		-0.45f,0.1f,0.f,
		//left
		-0.45f,0.1f,0.f,
		-0.45f,0.1f,0.5f,
		-0.3f,0.5f,0.5f,

		-0.3f,0.5f,0.5f,
		-0.3f,0.5f,0.f,
		-0.45f,0.1f,0.f,
		//right
		-0.25f,-0.1f,0.f,
		-0.25f,-0.1f,0.5f,
		-0.1f,0.5f,0.5f,

		-0.1f,0.5f,0.5f,
		-0.1f,0.5f,0.f,
		-0.25f,-0.1f,0.f,
		//top
		-0.3f,0.5f,0.f,
		-0.1f,0.5f,0.f,
		-0.3f,0.5f,0.5f,

		-0.3f,0.5f,0.5f,
		-0.1f,0.5f,0.5f,
		-0.3f,0.5f,0.f
	};
	float palm[]
	{		
		//front
		-0.28f,0.55f,0.f,
		-0.28f,0.75,0.f,
		-0.08f,0.75,0.f,

		-0.08f,0.75,0.f,
		-0.08f,0.55f,0.f,
		-0.28f,0.55f,0.f,
		//back
		-0.28f,0.55f,0.5f,
		-0.28f,0.75,0.5f,
		-0.08f,0.75,0.5f,

		-0.08f,0.75,0.5f,
		-0.08f,0.55f,0.5f,
		-0.28f,0.55f,0.5f,
		//left
		-0.28f,0.55f,0.f,
		-0.28f,0.55f,0.5f,
		-0.28f,0.75,0.5f,

		-0.28f,0.75,0.5f,
		-0.28f,0.75,0.f,
		-0.28f,0.55f,0.f,
		//right
		-0.08f,0.55f,0.f,
		-0.08f,0.55f,0.5f,
		-0.08f,0.75,0.5f,

		-0.08f,0.75,0.5f,
		-0.08f,0.75,0.f,
		-0.08f,0.55f,0.f,
		//top
		-0.28f, 0.75, 0.5f,
		-0.08f, 0.75, 0.5f,
		-0.08f, 0.75f, 0.0f,

		-0.08f, 0.75f, 0.0f,
		-0.28f, 0.75, 0.0f,
		-0.28f, 0.75, 0.5f
	};		
	
	int forearm_vertex_size = sizeof(forearm)/ sizeof(float);
	int hindarm_vertex_size = sizeof(hindarm) / sizeof(float);
	int palm_vertex_size = sizeof(palm) / sizeof(float);
	float *pos_data = new float[forearm_vertex_size];
	float *color_data = new float[forearm_vertex_size];
	for (int i = 0; i < forearm_vertex_size; i++)
	{
		pos_data[i] = forearm[i];
	}
	for (int i = 0; i < forearm_vertex_size; i++)
	{
		color_data[i] = color_spade[0][i % 3];
	}
	drawTriangle(pos_data, color_data, forearm_vertex_size, forearm_vertex_size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, forearm_vertex_size);

	pos_data = new float[hindarm_vertex_size];
	color_data = new float[hindarm_vertex_size];
	//matrix_stack.push(modelviewmatrix);
	modelviewmatrix = translate(modelviewmatrix,vec3(-0.05,0.15,0));
	modelviewmatrix = rotate(modelviewmatrix,radians(hindarmangle),vec3(0,0,1));
	modelviewmatrix = translate(modelviewmatrix, vec3(cos(hindarmangle/90.f)*0.05, sin(hindarmangle / 90.f)*0.1, 0));
	for (int i = 0; i < hindarm_vertex_size; i++)
	{
		pos_data[i] = hindarm[i];
	}
	for (int i = 0; i < hindarm_vertex_size; i++)
	{
		color_data[i] = color_spade[1][i % 3];
	}
	drawTriangle(pos_data, color_data, hindarm_vertex_size, hindarm_vertex_size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, hindarm_vertex_size);

	pos_data = new float[palm_vertex_size];
	color_data = new float[palm_vertex_size];
	modelviewmatrix = translate(modelviewmatrix, vec3(0.0, 0.1f, 0));
	modelviewmatrix = rotate(modelviewmatrix, radians(plamangle), vec3(0, 0, 1));
	for (int i = 0; i < palm_vertex_size; i++)
	{
		pos_data[i] = palm[i];
	}
	for (int i = 0; i < palm_vertex_size; i++)
	{
		color_data[i] = color_spade[2][i % 3];
	}
	drawTriangle(pos_data, color_data, palm_vertex_size, palm_vertex_size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, palm_vertex_size);
}
void drawTriangle(float *vertex, float *color, int vertex_size, int color_size, mat4 mm, mat4 pm) 
{	
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &pm[0][0]);
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, &mm[0][0]);
		
	GLfloat *data = new GLfloat[vertex_size + color_size];
	for (int i = 0; i < vertex_size; i++)
	{
		data[i] = vertex[i];
	}
	
	for (int i = 0; i < color_size; i++)
	{
		data[vertex_size + i] = color[i];
	}
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (vertex_size + color_size) * sizeof(GLfloat), data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT) * vertex_size));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
int signal = 1;
void timer(int v) {
	rotatedangle += signal * v;
	hindarmangle += signal * v;
	plamangle += signal * v ;
	if (rotatedangle > 90){
		rotatedangle = 90;
		signal = -1;
	}	
	if (hindarmangle > 45){
		hindarmangle = 45;
		signal = -1;
	}
	if (plamangle > 30){
		plamangle = 30;
		signal = -1;
	}
	if (rotatedangle < 0 || hindarmangle<0 || plamangle<0) {
		signal = 1;
	}		
	glutTimerFunc(100, timer, 5);
	all_display();
}
void rotatescene(int x ,int y) 
{
	if (x < window_width && x > 0)
	{
		scenerotateangle = ((float)x / window_width)*360.f;
	}	
}
void drawrobot() 
{
	glUseProgram(program);
	glBindVertexArray(triangle_vao);
	//Transformation
	mat4 identity(1.0);
	mat4 mm = translate(identity, vec3(0, 0, -4.0f));
	//matrix_stack.push(modelviewmatrix);
	//modelviewmatrix = mm;
	float color_spade[][3]{ {0.0f,1.0f,0.0f},{0.0f,0.0f,0.0f} };
	mm = rotate(mm, radians(45.f+ scenerotateangle),vec3(0,1,0));
	modelviewmatrix = mm;	
	float pos[] = {
		//Head
		-0.2f,0.8f,0,
		0.2f,0.8f,0,
		0.2f,0.6f,0,

		0.2f,0.6f,0,
		-0.2f,0.6f,0,
		-0.2f,0.8f,0,
		//Right eye
		0.1,0.7,0,
		0.05,0.7,0,
		0.05,0.65,0,

		0.05,0.65,0,
		0.1,0.65,0,
		0.1,0.7,0,
		//left eye
		-0.1,0.7,0,
		-0.05,0.7,0,
		-0.05,0.65,0,

		-0.05,0.65,0,
		-0.1,0.65,0,
		-0.1,0.7,0,
		//Body
		-0.3f,0.6f,0,
		0.3f,0.6f,0,
		0.3f,0.0f,0,

		0.3f,0.0f,0,
		-0.3f,0.0f,0,
		-0.3f,0.6f,0,
		//Right Arm 
		0.31,0.6,0,
		0.35,0.6,0,
		0.35,0.1,0,

		0.35,0.1,0,
		0.31,0.1,0,
		0.31,0.6,0,
		//Left Arm 
		-0.31,0.6,0,
		-0.35,0.6,0,
		-0.35,0.1,0,

		-0.35,0.1,0,
		-0.31,0.1,0,
		-0.31,0.6,0,
		//Right leg 
		0.2,0.0,0,
		0.1,0.0,0,
		0.1,-0.4,0,

		0.1,-0.4,0,
		0.2,-0.4,0,
		0.2,0.0,0,
		//Left leg 
		-0.2,0.0,0,
		-0.1,0.0,0,
		-0.1,-0.4,0,

		-0.1,-0.4,0,
		-0.2,-0.4,0,
		-0.2,0.0,0
	};
	int headpart = 18;
	int righteye = 18;
	int lefteye = 18;
	int bodypart = 18;
	int rightarm = 18;
	int leftarm = 18;
	int rightleg = 18;
	int leftleg = 18;
	//Head
	int offset = 0;
	int size = headpart;
	float *pos_data = new float[size];
	float *color_data = new float[size];	
	for (int i = 0; i < size; i++)
	{
		pos_data[i] = pos[i];
	}
	for (int i = 0; i < size; i++)
	{
		color_data[i] = color_spade[0][i % 3];
	}
	drawTriangle(pos_data, color_data, size, size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, size);
	//Eye
	offset += size;
	size = righteye + lefteye;
	pos_data = NULL;
	color_data = NULL;
	pos_data = new float[size];
	color_data = new float[size];	
	for (int i = 0; i < size; i++)
	{
		pos_data[i] = pos[offset + i];
	}	
	for (int i = 0; i < size; i++)
	{
		color_data[i] = color_spade[1][i % 3];
	}
	drawTriangle(pos_data, color_data, size, size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, size);
	//Body
	offset += size;
	size = bodypart;
	pos_data = NULL;
	color_data = NULL;
	pos_data = new float[size];
	color_data = new float[size];
	for (int i = 0; i < size; i++)
	{
		pos_data[i] = pos[offset + i];
	}
	for (int i = 0; i < size; i++)
	{
		color_data[i] = color_spade[0][i % 3];
	}
	drawTriangle(pos_data, color_data, size, size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, size);
	//Timer
	//float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	//Arm
	offset += size;
	size = leftarm+rightarm;
	pos_data = NULL;
	color_data = NULL;
	pos_data = new float[size];
	color_data = new float[size];
	//
	matrix_stack.push(modelviewmatrix);
	modelviewmatrix = translate(modelviewmatrix, vec3(-0.05, 0.45, 0));
	modelviewmatrix = rotate(modelviewmatrix,radians(rotatedangle),vec3(1,0,0));		
	modelviewmatrix = translate(modelviewmatrix, vec3(cos(rotatedangle/180) * 0.02f, sin(rotatedangle/180) * -0.2f, 0));
	for (int i = 0; i < size; i++)
	{
		pos_data[i] = pos[offset + i];
	}
	for (int i = 0; i < size; i++)
	{
		color_data[i] = color_spade[1][i % 3];
	}
	drawTriangle(pos_data, color_data, size, size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, size);
	modelviewmatrix = matrix_stack.top();
	//Leg
	offset += size;
	size = leftleg + rightleg;
	pos_data = NULL;
	color_data = NULL;
	pos_data = new float[size];
	color_data = new float[size];
	matrix_stack.push(modelviewmatrix);
	modelviewmatrix = rotate(modelviewmatrix, radians(-5.f), vec3(0, 0, 1));
	modelviewmatrix = translate(modelviewmatrix, vec3(-0.05, 0, 0));
	for (int i = 0; i < size; i++)
	{
		pos_data[i] = pos[offset + i];
	}
	for (int i = 0; i < size; i++)
	{
		color_data[i] = color_spade[1][i % 3];
	}
	drawTriangle(pos_data, color_data, size, size, modelviewmatrix, projectionmatrix);
	glDrawArrays(GL_TRIANGLES, 0, size);
	modelviewmatrix = matrix_stack.top();

}
void all_display() {
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	float f = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &f);	
	//drawrobot();
	//drawArm();
	drawOBJ();
	glutSwapBuffers();
}
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(100, 100);
	int window_id = glutCreateWindow("Quiz");
	shader_init();	
	projectionmatrix = GetProjectionMatrix(window_width/window_height,false,1.0);
	char path[] = "./src/hlgc/3dmodel/miku_body.obj";
	load3Dmodel(path);
	glutDisplayFunc(all_display);
	glutTimerFunc(100,timer,5);
	glutKeyboardFunc(reset);
	glutMotionFunc(rotatescene);
	glutMainLoop();
	return 0;
}