/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries separate legal notices                              |
|******************************************************************************|
| This is just a file holding some commonly-used "utility" functions to keep   |
| the main file a bit easier to read. You can might build up something like    |
| this as learn more GL. Note that you don't need much code here to do good GL.|
| If you have a big object-oriented engine then maybe you can ask yourself if  |
| it is really making life easier.                                             |
\******************************************************************************/
#include "GLUtils.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#define GL_LOG_FILE "gl.log"
#define MAX_SHADER_LENGTH 262144

/*--------------------------------LOG FUNCTIONS-------------------------------*/
bool restart_gl_log() {
	FILE* file = fopen(GL_LOG_FILE, "w");
	if (!file) {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE log file %s for writing\n",
			GL_LOG_FILE
			);
		return false;
	}
	time_t now = time(NULL);
	char* date = ctime(&now);
	fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
	fclose(file);
	return true;
}

bool gl_log(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
			);
		return false;
	}
	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);
	fclose(file);
	return true;
}

/* same as gl_log except also prints to stderr */
bool gl_log_err(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
			);
		return false;
	}
	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);
	va_start(argptr, message);
	vfprintf(stderr, message, argptr);
	va_end(argptr);
	fclose(file);
	return true;
}

/*--------------------------------GLFW3 and GLEW------------------------------*/
bool start_gl() {
	gl_log("starting GLFW %s %s", glfwGetVersionString() , "new text");

	if (!glfwInit())
	{
		gl_log_err("GLFW Fail Init");
		return false;
	}
	gl_log("starting GLFW\n%s\n", glfwGetVersionString());
	/*
	Definimos que version de OpenGL usaremos
	*/
	/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	*/
	g_window = NULL;
	/*
	Si queremos que nuestra aplicación sea Full Screen
	*/
	if (isFullScreen)
	{
		GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);

		if (pMode != NULL)
		{
			g_window = glfwCreateWindow(pMode->width, pMode->height, "OpenGL Window", pMonitor, NULL);
		}
	}
	else
	{
		g_window = glfwCreateWindow(g_gl_width, g_gl_height, "OpenGL Window", NULL, NULL);
	}

	if (g_window == NULL)
	{
		gl_log_err("Create Window Fail Init");
		return false;
	}
	// Indica que el contexto OpenGL será utilizada en la ventana GLFW
	glfwMakeContextCurrent(g_window);

	// register the error call-back and when change size function that we wrote, above
	glfwSetErrorCallback(glfw_error_callback);
	glfwSetWindowSizeCallback(g_window, glfw_window_size_callback);


	glfwSetKeyCallback(g_window, glfwOnKey);
	// Permite que extensiones no estandares sean posibles utilizarlas,
	// las funcioanlidades permitidas dependerá de la tarjeta de video que tengas
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		gl_log_err("Glew Initialize Fail");
		return false;
	}
	const GLubyte* renderer;
	const GLubyte* version;

	renderer = glGetString(GL_RENDERER); // get renderer string
	version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	gl_log("renderer: %s\nversion: %s\n", renderer, version);
	log_gl_params();
	return true;
}

void glfw_error_callback(int error, const char* description) {
	fputs(description, stderr);
	gl_log_err("%s\n", description);
}
// a call-back function
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	g_gl_width = width;
	g_gl_height = height;
	printf("width %i height %i\n", width, height);
	/* update any perspective matrices used here */
}

void _update_fps_counter(GLFWwindow* window) {
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

/*-----------------------------------SHADERS----------------------------------*/
/* copy a shader from a plain text file into a character array */
bool parse_file_into_str(const char* file_name, char* shader_str, int max_len
	) {
	FILE* file = fopen(file_name, "r");
	if (!file) {
		gl_log_err("ERROR: opening file for reading: %s\n", file_name);
		return false;
	}
	size_t cnt = fread(shader_str, 1, max_len - 1, file);
	if ((int)cnt >= max_len - 1) {
		gl_log_err("WARNING: file %s too big - truncated.\n", file_name);
	}
	if (ferror(file)) {
		gl_log_err("ERROR: reading shader file %s\n", file_name);
		fclose(file);
		return false;
	}
	// append \0 to end of file string
	shader_str[cnt] = 0;
	fclose(file);
	return true;
}

void print_shader_info_log(GLuint shader_index) {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetShaderInfoLog(shader_index, max_length, &actual_length, log);
	printf("shader info log for GL index %i:\n%s\n", shader_index, log);
	gl_log("shader info log for GL index %i:\n%s\n", shader_index, log);
}

bool create_shader(const char* file_name, GLuint* shader, GLenum type) {
	gl_log("creating shader from %s...\n", file_name);
	char shader_string[MAX_SHADER_LENGTH];
	parse_file_into_str(file_name, shader_string, MAX_SHADER_LENGTH);
	*shader = glCreateShader(type);
	const GLchar* p = (const GLchar*)shader_string;
	glShaderSource(*shader, 1, &p, NULL);
	glCompileShader(*shader);
	// check for compile errors
	int params = -1;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		gl_log_err("ERROR: GL shader index %i did not compile\n", *shader);
		print_shader_info_log(*shader);
		return false; // or exit or something
	}
	gl_log("shader compiled. index %i\n", *shader);
	return true;
}

void print_programme_info_log(GLuint sp) {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetProgramInfoLog(sp, max_length, &actual_length, log);
	printf("program info log for GL index %u:\n%s", sp, log);
	gl_log("program info log for GL index %u:\n%s", sp, log);
}

bool is_programme_valid(GLuint sp) {
	glValidateProgram(sp);
	GLint params = -1;
	glGetProgramiv(sp, GL_VALIDATE_STATUS, &params);
	if (GL_TRUE != params) {
		gl_log_err("program %i GL_VALIDATE_STATUS = GL_FALSE\n", sp);
		print_programme_info_log(sp);
		return false;
	}
	gl_log("program %i GL_VALIDATE_STATUS = GL_TRUE\n", sp);
	return true;
}

bool create_programme(GLuint vert, GLuint frag, GLuint* programme) {
	*programme = glCreateProgram();
	gl_log(
		"created programme %u. attaching shaders %u and %u...\n",
		*programme,
		vert,
		frag
		);
	glAttachShader(*programme, vert);
	glAttachShader(*programme, frag);
	// link the shader programme. if binding input attributes do that before link
	glLinkProgram(*programme);
	GLint params = -1;
	glGetProgramiv(*programme, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		gl_log_err(
			"ERROR: could not link shader programme GL index %u\n",
			*programme
			);
		print_programme_info_log(*programme);
		return false;
	}
	assert(is_programme_valid(*programme));
	// delete shaders here to free memory
	glDeleteShader(vert);
	glDeleteShader(frag);
	return true;
}

GLuint create_programme_from_files(
	const char* vert_file_name, const char* frag_file_name
	) {
	GLuint vert, frag, programme;
	assert(create_shader(vert_file_name, &vert, GL_VERTEX_SHADER));
	assert(create_shader(frag_file_name, &frag, GL_FRAGMENT_SHADER));
	assert(create_programme(vert, frag, &programme));
	return programme;
}


void log_gl_params() {
	int i;
	int v[2];
	unsigned char s = 0;
	GLenum params[] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_VIEWPORT_DIMS,
		GL_STEREO,
	};
	const char* names[] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_VIEWPORT_DIMS",
		"GL_STEREO",
	};
	gl_log("GL Context Params:\n");
	// integers - only works if the order is 0-10 integer return types
	for (i = 0; i < 10; i++) {
		int v = 0;
		glGetIntegerv(params[i], &v);
		gl_log("%s %i\n", names[i], v);
	}
	// others
	v[0] = v[1] = 0;
	glGetIntegerv(params[10], v);
	gl_log("%s %i %i\n", names[10], v[0], v[1]);
	glGetBooleanv(params[11], &s);
	gl_log("%s %i\n", names[11], (unsigned int)s);
	gl_log("-----------------------------\n");
}



void glfwOnKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	/* Cerrar la pantall cuando se presiona la tecla ESC*/
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

/* print absolutely everything about a shader - only useful if you get really
stuck wondering why a shader isn't working properly */
void print_all(GLuint sp) {
	int params = -1;
	int i;

	printf("--------------------\nshader programme %i info:\n", sp);
	glGetProgramiv(sp, GL_LINK_STATUS, &params);
	printf("GL_LINK_STATUS = %i\n", params);

	glGetProgramiv(sp, GL_ATTACHED_SHADERS, &params);
	printf("GL_ATTACHED_SHADERS = %i\n", params);

	glGetProgramiv(sp, GL_ACTIVE_ATTRIBUTES, &params);
	printf("GL_ACTIVE_ATTRIBUTES = %i\n", params);

	for (i = 0; i < params; i++) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveAttrib(sp, i, max_length, &actual_length, &size, &type,
			name);
		if (size > 1) {
			int j;
			for (j = 0; j < size; j++) {
				char long_name[64];
				int location;

				sprintf(long_name, "%s[%i]", name, j);
				location = glGetAttribLocation(sp, long_name);
				printf("  %i) type:%s name:%s location:%i\n",
					i, GL_type_to_string(type), long_name, location);
			}
		}
		else {
			int location = glGetAttribLocation(sp, name);
			printf("  %i) type:%s name:%s location:%i\n",
				i, GL_type_to_string(type), name, location);
		}
	}

	glGetProgramiv(sp, GL_ACTIVE_UNIFORMS, &params);
	printf("GL_ACTIVE_UNIFORMS = %i\n", params);
	for (i = 0; i < params; i++) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveUniform(sp, i, max_length, &actual_length, &size, &type,
			name);
		if (size > 1) {
			int j;
			for (j = 0; j < size; j++) {
				char long_name[64];
				int location;

				sprintf(long_name, "%s[%i]", name, j);
				location = glGetUniformLocation(sp, long_name);
				printf("  %i) type:%s name:%s location:%i\n",
					i, GL_type_to_string(type), long_name, location);
			}
		}
		else {
			int location = glGetUniformLocation(sp, name);
			printf("  %i) type:%s name:%s location:%i\n",
				i, GL_type_to_string(type), name, location);
		}
	}

	print_programme_info_log(sp);
}

/* validate shader */
bool is_valid(GLuint sp) {
	int params = -1;

	glValidateProgram(sp);
	glGetProgramiv(sp, GL_VALIDATE_STATUS, &params);
	printf("program %i GL_VALIDATE_STATUS = %i\n", sp, params);
	if (GL_TRUE != params) {
		print_programme_info_log(sp);
		return false;
	}
	return true;
}

const char* GL_type_to_string(GLenum type) {
	switch (type) {
	case GL_BOOL: return "bool";
	case GL_INT: return "int";
	case GL_FLOAT: return "float";
	case GL_FLOAT_VEC2: return "vec2";
	case GL_FLOAT_VEC3: return "vec3";
	case GL_FLOAT_VEC4: return "vec4";
	case GL_FLOAT_MAT2: return "mat2";
	case GL_FLOAT_MAT3: return "mat3";
	case GL_FLOAT_MAT4: return "mat4";
	case GL_SAMPLER_2D: return "sampler2D";
	case GL_SAMPLER_3D: return "sampler3D";
	case GL_SAMPLER_CUBE: return "samplerCube";
	case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
	default: break;
	}
	return "other";
}

