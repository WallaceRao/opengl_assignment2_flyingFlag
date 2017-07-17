#include <stdio.h>  
#include<math.h>
#include <GL/glew.h>
#include <glm/glm.hpp> 
#include <GLFW/glfw3.h>
#include <SOIL.h>	
#include"textFile.h"

namespace GLMAIN {
	GLuint	texture[3];			// Storage For Textures
	GLFWwindow* window;			// Storage For glfw window
	GLuint vao;					// Storage For vao
	int poleBegin;			// Storage For the index of the flag pole start point
	GLint phaseLoc, mvpLoc, samplerLoc;  // Storage For locations in shaders
}

#define flagDivisions 32  // The flag is divided into 32*32 squres,

int LoadGLTextures()		// Load images And Convert To Textures							
{
	glGenTextures(1, &GLMAIN::texture[0]);					// Create The Texture for flag
	glBindTexture(GL_TEXTURE_2D, GLMAIN::texture[0]);
	int width, height;
	int soilForceChannels = SOIL_LOAD_RGB;
	unsigned char *image = SOIL_load_image("flag.png",
		&width, &height, 0, soilForceChannels);

	glTexImage2D(GL_TEXTURE_2D, // texture target
		0, // level of detail (mipmap)
		GL_RGB, // internal data format, defines
				// number of colour components
		width, // the image width
		height, // the image height
		0, // unused parameter, must be 0
		GL_RGB, // pixel data format
		GL_UNSIGNED_BYTE,// data type used for pixels
		image); // the image data
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &GLMAIN::texture[1]);					// Create The Texture for surface
	glBindTexture(GL_TEXTURE_2D, GLMAIN::texture[1]);
	unsigned char  *imageSurface = SOIL_load_image("surface.jpg",
		&width, &height, 0, soilForceChannels);

	glTexImage2D(GL_TEXTURE_2D, // texture target
		0, // level of detail (mipmap)
		GL_RGB, // internal data format, defines
				// number of colour components
		width, // the image width
		height, // the image height
		0, // unused parameter, must be 0
		GL_RGB, // pixel data format
		GL_UNSIGNED_BYTE,// data type used for pixels
		imageSurface); // the image data

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &GLMAIN::texture[2]);					// Create The Texture for flag pole
	glBindTexture(GL_TEXTURE_2D, GLMAIN::texture[2]);
	unsigned char  *imagePole = SOIL_load_image("pole.jpg",
		&width, &height, 0, soilForceChannels);

	glTexImage2D(GL_TEXTURE_2D, // texture target
		0, // level of detail (mipmap)
		GL_RGB, // internal data format, defines  number of colour components
		width, // the image width
		height, // the image height
		0, // unused parameter, must be 0
		GL_RGB, // pixel data format
		GL_UNSIGNED_BYTE,// data type used for pixels
		imagePole); // the image data
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void display(void) // Display method, draw flag, flag pole and a surface based on vao.
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	static float flagWave = 0.0f; // the flag waving speed
	flagWave += 40.0f;
	if (flagWave > 360.0)
		flagWave = 0.0f;
	float phase = (flagWave / 180.0) * 3.14159;
	if(GLMAIN::phaseLoc != -1)
		glUniform1f(GLMAIN::phaseLoc, phase);
	static float viewAngleX = 0.0f;
	// rotate 1 degree per refresh
	viewAngleX += 1.0;
	if (viewAngleX > 360.0) viewAngleX = 0.0f;
	float rotateX = (viewAngleX / 180.0) * 3.14159;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 mvp = glm::mat4(1.0f);

	float orthoLeft, orthoRight, orthoTop, orthoBottom, orthoNear, orthoFar;
	orthoLeft = -1.00;
	orthoRight = 1.00;
	orthoBottom = -1.00;
	orthoTop = 1.00;
	orthoNear = -2.5;
	orthoFar = 2.5;

	glm::mat4  projectionMatrix = glm::mat4(2 / (orthoRight - orthoLeft), 0, 0, (orthoRight + orthoLeft) / (orthoLeft - orthoRight),
		0, 2 / (orthoTop - orthoBottom), 0, (orthoTop + orthoBottom) / (orthoBottom - orthoTop),
		0, 0, 2 / (orthoFar - orthoNear), (orthoFar + orthoNear) / (orthoNear - orthoFar),
		0, 0, 0, 1);

	const glm::vec3 camPos(sin(rotateX), cos(rotateX), 0.5);// campera position
	const glm::vec3 lookAt(0.0, 0.0, 0.0);                  // point of interest
	const glm::vec3 camOffset = lookAt - camPos;
	const glm::vec3 camForward = camOffset /
		glm::length(camOffset);
	const glm::vec3 camUp0(-sin(rotateX), -cos(rotateX), 0.0f);
	const glm::vec3 camRight = glm::cross(camForward, camUp0);
	const glm::vec3 camUp = glm::cross(camRight, camForward);

	const glm::mat4 viewRotation(
		camRight.x, camUp.x, camForward.
		x, 0.f, // column 0
		camRight.y, camUp.y, camForward.
		y, 0.f, // column 1
		camRight.z, camUp.z, camForward.
		z, 0.f, // column 2
		0.f, 0.f, 0.f, 1.f);// column 3
	const glm::mat4 viewTranslation(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		camPos.
		x, camPos.
		y, camPos.
		z, 1);
	glm::mat4 viewMatrix = viewRotation * viewTranslation;
	mvp = projectionMatrix * viewMatrix * modelMatrix;
	float mvpFloat[16];
	for (int i = 0; i < 4; i++)
	{
		mvpFloat[i * 4] = mvp[i].x;
		mvpFloat[i * 4 + 1] = mvp[i].y;
		mvpFloat[i * 4 + 2] = mvp[i].z;
		mvpFloat[i * 4 + 3] = mvp[i].w;

	}
	if(GLMAIN::mvpLoc != -1)
		glUniformMatrix4fv(GLMAIN::mvpLoc, 1, false, mvpFloat);
	glBindVertexArray(GLMAIN::vao);

	// Draw the surface
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GLMAIN::texture[1]);
	if (GLMAIN::samplerLoc != -1)
	{
		glUniform1i(GLMAIN::samplerLoc, 1);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw the flag
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLMAIN::texture[0]);
	if (GLMAIN::samplerLoc != -1)
	{
		glUniform1i(GLMAIN::samplerLoc, 0);
	}
	for (int i = 0; i< flagDivisions; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, 4 + 2 * (1 + flagDivisions)*i, 2 * (1 + flagDivisions));
	}

	// Draw the pole
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GLMAIN::texture[2]);
	if (GLMAIN::samplerLoc != -1)
	{
		glUniform1i(GLMAIN::samplerLoc, 2);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, GLMAIN::poleBegin, 2 * flagDivisions + 2);
	glfwSwapBuffers(GLMAIN::window);

	// Poll for and process events
	glfwPollEvents();
}


void initVAO() // Init vao, vbo.
{
	float vertices[12 + 3 * flagDivisions * 2 * (2 + flagDivisions) + 6];
	float texArray[8 + 2 * flagDivisions * 2 * (2 + flagDivisions) + 4];
	GLuint  vbo[2];
	// The checked floor geometry: 4 points
	vertices[0] = -1.0f;  // bottom-left
	vertices[1] = -1.0f;
	vertices[2] = vertices[5] = vertices[8] = vertices[11] = -1.0f;
	vertices[3] = 1.0f;   // bottom-right
	vertices[4] = -1.0f;
	vertices[6] = -1.0f;  // top-left
	vertices[7] = 1.0f;
	vertices[9] = 1.0f;   // top-right
	vertices[10] = 1.0f;

	texArray[0] = vertices[0];  // bottom-left
	texArray[1] = vertices[1];
	texArray[2] = vertices[3];
	texArray[3] = vertices[4];
	texArray[4] = vertices[6];
	texArray[5] = vertices[7];
	texArray[6] = vertices[9];
	texArray[7] = vertices[10];

	float dw = 1.0f / flagDivisions; // flag size is 1.0 x 0.8
	float dh = 0.8f / flagDivisions;
	float dtx = 1.0f / flagDivisions;  // Texture coordinate increment per interval
	int kv = 12;  // Start index of vertex array
	int kt = 8;   // Start index of texture array
	float flagZ = 0.4f;
	for (int i = 0; i<flagDivisions; i++) {
		float x1 = dw * i;
		float x2 = x1 + dw;
		for (int j = 0; j<flagDivisions + 1; j++) {
			vertices[kv] = x1;
			vertices[kv + 1] = vertices[kv + 4] = 0.0f;  // y=0
			vertices[kv + 2] = vertices[kv + 5] = dh * j + flagZ;
			vertices[kv + 3] = x2;
			kv += 6;
			texArray[kt] = dtx * i;
			texArray[kt + 1] = 1.0f - dtx * j;
			texArray[kt + 2] = dtx + dtx * i;
			texArray[kt + 3] = 1.0f - dtx * j;
			kt += 4;
		}
	}
	// Start of the flag pole
	GLMAIN::poleBegin = kv / 3;
	float da = (float)((2 * 3.14159) / flagDivisions);
	float r = 0.02f;
	for (int i = 0; i<flagDivisions + 1; i++) {
		vertices[kv + 0] = (float)(cos(da * i) * r - r);
		vertices[kv + 1] = (float)(sin(da * i) * r);
		vertices[kv + 2] = 0.8f + flagZ;
		vertices[kv + 3] = vertices[kv];
		vertices[kv + 4] = vertices[kv + 1];
		vertices[kv + 5] = -0.99f;
		kv += 6;
		texArray[kt] = dtx * i;
		texArray[kt + 1] = 0.0f;
		texArray[kt + 2] = dtx * i + dtx;
		texArray[kt + 3] = 1.0f;
		kt += 4;
	}

	glGenVertexArrays(1, &GLMAIN::vao);
	glBindVertexArray(GLMAIN::vao);

	glGenBuffers(2, vbo);
	GLuint positionBufferHandle = vbo[0];
	GLuint texBufferHandle = vbo[1];

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texArray), texArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texBufferHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
}


int setShaders() // Init shaders.
{
	GLint vertCompiled, fragCompiled;
	GLint linked;
	char *vs = NULL, *fs = NULL;
	GLuint VertexShaderObject = 0;
	GLuint FragmentShaderObject = 0;
	GLuint ProgramObject = glCreateProgram();

	vs = textFileRead((char *)"test.vert");
	fs = textFileRead((char *)"test.frag");
	glUseProgram(ProgramObject);

	VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	// Load source code into shaders.
	glShaderSource(VertexShaderObject, 1, (const char **)&vs, NULL);
	glShaderSource(FragmentShaderObject, 1, (const char **)&fs, NULL);
	// Compile the  vertex shader.
	glCompileShader(VertexShaderObject);
	glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &vertCompiled);
	// Compile the fragment shader
	glCompileShader(FragmentShaderObject);
	glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &fragCompiled);
	if (!vertCompiled || !fragCompiled)
	{
		printf("Shader compile failed\n");
		return 0;
	}
	glAttachShader(ProgramObject, VertexShaderObject);
	glAttachShader(ProgramObject, FragmentShaderObject);
	glLinkProgram(ProgramObject);
	glGetProgramiv(ProgramObject, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		// Print logs if link shaders failed.
		GLsizei len;
		glGetProgramiv(ProgramObject, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(ProgramObject, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		delete[] log;
		return 0;
	}
	glUseProgram(ProgramObject);
	GLMAIN::phaseLoc = glGetUniformLocation(ProgramObject, "phase");
	GLMAIN::mvpLoc = glGetUniformLocation(ProgramObject, "mvp");
	GLMAIN::samplerLoc = glGetUniformLocation(ProgramObject, "sampler0");
	glDeleteShader(VertexShaderObject);
	glDeleteShader(FragmentShaderObject);
	glDeleteProgram(ProgramObject);
	return 1;
}


int main(int argc, char* argv[])
{
	// init glfw
	if (!glfwInit())
		return -1;

	// create a windowed mode window and its OpenGL context 
	GLMAIN::window = glfwCreateWindow(480, 320, "Waving Flag", NULL, NULL);
	if (!GLMAIN::window)
	{
		glfwTerminate();
		return -1;
	}

	//make the window's context current 
	glfwMakeContextCurrent(GLMAIN::window);
	// init glew
	glewInit();

	setShaders();
	initVAO();
	LoadGLTextures();

	static double limitFPS = 1.0 / 30.0; // limit to 30 frames per second
	double lastTime = glfwGetTime();
	double deltaTime = 0, nowTime = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(GLMAIN::window))
	{
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / limitFPS;
		lastTime = nowTime;

		if (deltaTime < 1.0)
			continue;
		// - Only update at 30 frames / s
		while (deltaTime >= 1.0) {
			deltaTime--;
		}
		display(); //  Render function
	}
	glfwTerminate();
	return 0;
}
