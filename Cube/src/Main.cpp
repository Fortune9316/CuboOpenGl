#define GLEW_STATIC
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include "GLUtils.h"
#include<assert.h>
#include<glm\glm.hpp>
#include<glm\gtx\transform.hpp>
#include<time.h>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\quaternion.hpp>

using namespace glm;
using namespace std;
int g_gl_width = 640;
int g_gl_height = 640;
GLFWwindow *g_window = NULL;
bool isFullScreen = false;

GLuint vs, fs;
GLuint shader_programme;

int roll(int min, int max)
{
	double x = rand() / static_cast<double>(RAND_MAX + 1);
	int result = min + static_cast<int>(x*(max - min));
	return result;
}

void FillColors(GLfloat colors []) {
	int index=0;
	for (int i = 0; i < 12; i++) {
		GLfloat r = static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX);
		r = (floor(r * 10) / 10) + 0.1f;
		GLfloat g = static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX);
		g = (floor(r * 10) / 10) + 0.1f;
		GLfloat b = static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX);		
		b = 1.0f;
		switch (roll(0,3))
		{
		case 0:
			r = 1.0f;
			break;
		case 1:
			g = 1.0f;
			break;
		case 2:
			b = 1.0f;
			break;
		}
		cout << "r: " << r << endl;
		for (int j = 0; j < 3 ; j++) {
			colors[index] = r;
			colors[index +1] = g;
			colors[index + 2] = b;
			index = index + 3;
		}
	}
}


int main()
{
	srand(static_cast <unsigned>(time(0)));
	if (!start_gl()) {
		glfwTerminate();
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	GLfloat points[] =
	{
		-0.5f  , -0.5f ,-0.5f,
	 	-0.5f  ,-0.5f ,0.5f,
		-0.5f  ,0.5f ,0.5f,

		0.5f  , 0.5f ,-0.5f,
		-0.5f  ,-0.5f ,-0.5f,
		-0.5f  ,0.5f ,-0.5f,

		0.5f  , -0.5f ,0.5f,
		-0.5f  ,-0.5f ,-0.5f,
		0.5f  ,-0.5f ,-0.5f,

		0.5f  , 0.5f ,-0.5f,
		0.5f  ,-0.5f ,-0.5f,
		-0.5f  ,-0.5f ,-0.5f,

		-0.5f  , -0.5f ,-0.5f,
		-0.5f  ,0.5f ,0.5f,
		-0.5f  ,0.5f ,-0.5f,

		0.5f  , -0.5f ,0.5f,
		-0.5f  ,-0.5f ,0.5f,
		-0.5f  ,-0.5f ,-0.5f,

		-0.5f  , 0.5f ,0.5f,
		-0.5f  ,-0.5f ,0.5f,
		0.5f  ,-0.5f ,0.5f,

		0.5f  , 0.5f ,0.5f,
		0.5f  ,-0.5f ,-0.5f,
		0.5f  ,0.5f ,-0.5f,

		0.5f  , -0.5f ,-0.5f,
		0.5f  ,0.5f ,0.5f,
		0.5f  ,-0.5f ,0.5f,

		0.5f  , 0.5f ,0.5f,
		0.5f  ,0.5f ,-0.5f,
		-0.5f  ,0.5f ,-0.5f,

		0.5f  , 0.5f ,0.5f,
		-0.5f  ,0.5f ,-0.5f,
		-0.5f  ,0.5f ,0.5f,

		0.5f  , 0.5f ,0.5f,
		-0.5f  ,0.5f ,0.5f,
		0.5f  ,-0.5f ,0.5f,

	
	};
	/*GLfloat colours[] =
	{
		1.0f,0,0,
		0,1.0f,0,
		1.0f,1.0f,0,
	};*/

	GLfloat colours[108];
	FillColors(colours);

	char str_vertex_shader[256 * 1024];
	char str_fragment_shader[256 * 1024];

	//vbo=vertex buffer object
	GLuint vbo_points = 0;
	glGenBuffers(1, &vbo_points);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint vbo_colours;
	glGenBuffers(1, &vbo_colours);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);


	//vao=vertex array objects =conjunto de vbo
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_colours);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);



	parse_file_into_str("shader_vs.glsl", str_vertex_shader, 256 * 1024);
	parse_file_into_str("shader_fs.glsl", str_fragment_shader, 256 * 1024);

	//vs=vertex shader
	vs = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *p = (const GLchar*)str_vertex_shader;
	glShaderSource(vs, 1, &p, NULL);
	glCompileShader(vs);


	// si hay un error se imprimira y se cerrara
	int params = -1;

	glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE) {
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n ",vs);
		print_shader_info_log(vs);
		return 1;
	}

	//fs=fragment shader

	fs = glCreateShader(GL_FRAGMENT_SHADER);

	p = (const GLchar*)str_fragment_shader;
	glShaderSource(fs, 1, &p, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE) {
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n ", fs);
		print_shader_info_log(fs);
		return 1;
	}

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
	if(params!=GL_TRUE){
		//con %i muestra un entero
		fprintf(stderr, "ERROR: could not get shaderprogramme GL index %i \n ", shader_programme);
		print_programme_info_log(shader_programme);
		return 1;
	}
	print_all(shader_programme);
	bool isValid = is_valid(shader_programme);
	assert(isValid);
	//camera -----------------------------------------------------------------------------fov 65 - near 0.1f far 100.0f
	mat4 mp = perspective(radians(65.0f), g_gl_width / g_gl_height*1.0f, 0.1f, 100.0f);
	mat4 mv_r = eulerAngleXYZ(.0F, .0F, .0F);

	mv_r[0][2] = -mv_r[0][2];
	mv_r[1][2] = -mv_r[1][2];
	mv_r[2][2] = -mv_r[2][2];
	vec4 cam_pos = vec4();
	cam_pos.z = -2.0f;

	mat4 mv_t = mat4();
	mv_t[3][0] = -cam_pos.x;
	mv_t[3][1] = -cam_pos.y;
	mv_t[3][2] = -cam_pos.z;

	mat4 mv = mv_r*mv_t;
	mat4 matrix = mp*mv;

	//send matrixP y matrixV to	vertex shader
	unsigned int matrixP_location = glGetUniformLocation(shader_programme, "matrixP");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(matrixP_location, 1, GL_FALSE, &mp[0][0]);
	unsigned int matrixV_location = glGetUniformLocation(shader_programme, "matrixV");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(matrixV_location, 1, GL_FALSE, &mv[0][0]);


	glEnable(GL_CULL_FACE);//cull face
	glCullFace(GL_BACK);//cull back face
	glFrontFace(GL_CW);//GL_CW for counter clock wise


	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	float cam_speed = 0.001f;
	float yaw = 0;
	float pitch = 0;
	float roll = 0;

	vec4 fv = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vec4 ri = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vec4 up = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	
	quat qall = angleAxis(0.0f, vec3(up.x, up.y, up.z));
	vec3 move = vec3(0, 0, 0);

	//--------------
	mat4 rot = rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	float oldTime = 0;
	//--------------

	while (!glfwWindowShouldClose(g_window)) {
		_update_fps_counter(g_window);
		//permite procesar eventos de la ventana
		glfwPollEvents();

		bool cam_moved = false;
		yaw = 0;
		pitch = 0;
		roll = 0;
		move.x = move.y = 0;
		if (glfwGetKey(g_window, GLFW_KEY_LEFT))
		{
			move.x -= cam_speed;
			cam_moved = true;
		}

		if (glfwGetKey(g_window, GLFW_KEY_RIGHT))
		{
			move.x += cam_speed;
			cam_moved = true;
		}

		if (glfwGetKey(g_window, GLFW_KEY_UP))
		{
			move.y += cam_speed;
			cam_moved = true;
		}

		if (glfwGetKey(g_window, GLFW_KEY_DOWN))
		{
			move.y-= cam_speed;
			cam_moved = true;
		}

	

		//-------------
		if (glfwGetKey(g_window, GLFW_KEY_D))
		{
			yaw += 0.01f;
			quat qyaw =angleAxis(radians(yaw),vec3(up.x,up.y,up.z));
			qall = qyaw*qall;
			mv_r = mat4_cast(qall);

			mv_r[0][2] = -mv_r[0][2];
			mv_r[1][2] = -mv_r[1][2];
			mv_r[2][2] = -mv_r[2][2];

			fv = mv_r*vec4(0, 0, 1.0f, 0);
			ri = mv_r*vec4(1.0f, 0, 0, 0);
			up=mv_r*vec4(0, 1.0f, 0, 0);

			cam_moved = true;
		}

		if (glfwGetKey(g_window, GLFW_KEY_A))
		{
			yaw -= 0.01f;
			quat qyaw = angleAxis(radians(yaw), vec3(up.x, up.y, up.z));
			qall = qyaw*qall;
			mv_r = mat4_cast(qall);

			mv_r[0][2] = -mv_r[0][2];
			mv_r[1][2] = -mv_r[1][2];
			mv_r[2][2] = -mv_r[2][2];

			fv = mv_r*vec4(0, 0, 1.0f, 0);
			ri = mv_r*vec4(1.0f, 0, 0, 0);
			up = mv_r*vec4(0, 1.0f, 0, 0);
			cam_moved = true;
		}

		if (glfwGetKey(g_window, GLFW_KEY_W))
		{
			pitch+= 0.01f;
		
			quat qyaw = angleAxis(radians(pitch), vec3(ri.x, ri.y, ri.z));
			qall = qyaw*qall;
			mv_r = mat4_cast(qall);

			mv_r[0][2] = -mv_r[0][2];
			mv_r[1][2] = -mv_r[1][2];
			mv_r[2][2] = -mv_r[2][2];

			fv = mv_r*vec4(0, 0, 1.0f, 0);
			ri = mv_r*vec4(1.0f, 0, 0, 0);
			up = mv_r*vec4(0, 1.0f, 0, 0);
			cam_moved = true;
		}

		if (glfwGetKey(g_window, GLFW_KEY_S))
		{
			pitch -= 0.01f;

			quat qri = angleAxis(radians(pitch), vec3(ri.x, ri.y,ri.z));
			qall = qri*qall;
			mv_r = mat4_cast(qall);

			mv_r[0][2] = -mv_r[0][2];
			mv_r[1][2] = -mv_r[1][2];
			mv_r[2][2] = -mv_r[2][2];

			fv = mv_r*vec4(0, 0, 1.0f, 0);
			ri = mv_r*vec4(1.0f, 0, 0, 0);
			up = mv_r*vec4(0, 1.0f, 0, 0);
			cam_moved = true;
		}

	

		if (cam_moved)
		{
			cam_pos += up*move.y;
			cam_pos += ri*move.x;


			mv_t[3][0] = -cam_pos.x;
			mv_t[3][1] = -cam_pos.y;
			mv_t[3][2] = -cam_pos.z;

			 mv = mv_r*mv_t;
		}



		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_gl_width, g_gl_height);

		glUseProgram(shader_programme);
		glUniformMatrix4fv(matrixP_location, 1, GL_FALSE, &mp[0][0]);
		glUseProgram(shader_programme);
		glUniformMatrix4fv(matrixV_location, 1, GL_FALSE, &mv[0][0]);

		glBindVertexArray(vao);

		//dibujamos el triangulo usando el vao y el shader actual
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//crea una pantalla que quieras ver y cuando lo actives la pantalla actual se borra y lo muestra el que quieres
		glfwSwapBuffers(g_window);
	}
}

