#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION //for stb
#include <stb_image_write.h>

#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"

const std::string vs_filename = "../vshader.glsl";
const std::string fs_filename = "../fshader.glsl";
//const std::string filename = "../res/nano/nanosuit2.obj";
const std::string filename = "../res/20.obj";

Shader shader;
Model model;

GLuint swidth = 720, sheight = 540;

bool k_blend = false;
bool k_texture = false;
int k_mode = 0;

glm::mat4 translate, rotate, scale;

int rx, ry;

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(glGetUniformLocation(shader.program, "u_rotate"), 1, GL_FALSE, &rotate[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "u_scale"), 1, GL_FALSE, &scale[0][0]);

	model.render(shader, k_texture);

	glutSwapBuffers();
}

void save()
{
	GLint viewport[4];
	GLubyte *data = new GLubyte[swidth*sheight * 3];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE, data);
	if (stbi_write_png("./res/result.png", swidth, sheight, 3, data, 0))
	{
		std::cout << "Save successfully." << std::endl;
	}
}

void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			rx = x;
			ry = y;
		}
	}
}

void onMotion(int x, int y)
{
	rx = x - rx;
	ry = y - ry;
	if (rx != 0 || ry != 0)
	{
		float angle = 0.1 / (rx*rx + ry*ry);
		rotate = glm::rotate(rotate, angle, glm::vec3((float)-ry, (float)rx, 0.0));
		//        std::cout<<rx<<"\t"<<ry<<std::endl;
		//        Debug::print_mat4(rotate);
	}
	rx = x;
	ry = y;

	myDisplay();
}

void onMouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0 && scale[0][0] < 10.0)
	{
		scale = glm::scale(scale, glm::vec3(1.2, 1.2, 1.2));
		myDisplay();
	}
	else if (direction < 0 && scale[0][0] > 0.1)
	{
		scale = glm::scale(scale, glm::vec3(0.9, 0.9, 0.9));
		myDisplay();
	}
}

void onKeyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
    case 'S':
		save();
		break;
	case 'b':
    case 'B':
		k_blend = !k_blend;
		break;
	case 't':
    case 'T':
		k_texture = !k_texture;
		break;
    case 'm':
    case 'M':
		k_mode = (++k_mode) % 3;
		break;
	}
	if (k_blend)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	if (k_mode == 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (k_mode == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	myDisplay();
}

void reshape(int width, int height)
{
	swidth = width;
	sheight = height;
	glViewport(0, 0, swidth, sheight);
}

void initGL()
{
	glClearColor(0.17, 0.17, 0.17, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//    glEnable(GL_BLEND);
	//    glEnable(GL_LIGHTING);
	//    glDisable(GL_TEXTURE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glutDisplayFunc(myDisplay);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);
	glutKeyboardFunc(onKeyBoard);
	glutReshapeFunc(reshape);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(swidth, sheight);
	glutInitWindowPosition(60, 60);
	glutCreateWindow("Graphics");

	initGL();

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
		exit(EXIT_FAILURE);
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

//	std::string filename;
//	std::cin >> filename;

	if (!shader.compile_shader(vs_filename, fs_filename) || !model.open_file(filename))
	{
		exit(EXIT_FAILURE);
	}

	shader.init_light();
	shader.load_lights();

	glutMainLoop();

	return 0;
}