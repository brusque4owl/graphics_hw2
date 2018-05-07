#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

//////////////////////////////////////////////////////////////////
//////// Setting Cameras /////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define NUMBER_OF_CAMERAS 8		// main, static cctv * 3, dynamic cctv, front view, side view, top view
#define CAM_TRANSLATION_SPEED 0.025f
#define CAM_ROTATION_SPEED 0.1f
glm::mat4 ViewProjectionMatrix[NUMBER_OF_CAMERAS], ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];
glm::mat4 ModelViewMatrix[NUMBER_OF_CAMERAS];
glm::mat4 ModelViewProjectionMatrix;
typedef struct {
	// eye, viewpoint, up vector.
	glm::vec3 prp, vrp, vup; // in this example code, make vup always equal to the v direction.
	float fov_y, aspect_ratio, near_clip, far_clip, zoom_factor;
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int camera_selected;

#include "Object_Definitions.h"

typedef struct _VIEWPORT {
	int x, y, w, h;
} VIEWPORT;
VIEWPORT viewport[NUMBER_OF_CAMERAS];					// 뷰 포트 세팅

//카메라 기준점 pos(vec3타입)을 옮김. 카메라를 axis를 축을 기준으로 del에 비례하여 이동시켜라.

//////////////////////////////////////////////////////////////////
/////// End of Setting Cameras ///////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef struct _CALLBACK_CONTEXT {
	int left_button_status;
	int prevx, prevy;
} CALLBACK_CONTEXT;
CALLBACK_CONTEXT cc;

void display_camera(int cam_index){

	glViewport(viewport[cam_index].x, viewport[cam_index].y, viewport[cam_index].w, viewport[cam_index].h);

	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);


	draw_static_object(&(static_objects[OBJ_BUILDING]), 0);

	draw_static_object(&(static_objects[OBJ_TABLE]), 0);
	draw_static_object(&(static_objects[OBJ_TABLE]), 1);	// takes given teapot

	draw_static_object(&(static_objects[OBJ_LIGHT]), 0);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 1);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 2);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 3);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 4);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 5);			// NEW OBJ_LIGHT

	draw_static_object(&(static_objects[OBJ_TEAPOT]), 0);	// on the OBJ_TABLE 1
	draw_static_object(&(static_objects[OBJ_TEAPOT]), 1);			// NEW OBJ_TEAPOT
	draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0);
	draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 1);		// NEW OBJ_NEW_CHAIR
	draw_static_object(&(static_objects[OBJ_FRAME]), 0);
	draw_static_object(&(static_objects[OBJ_FRAME]), 1);			// NEW OBJ_FRAME
	draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0);
	draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 1);		// NEW OBJ_NEW_PICTURE
	draw_static_object(&(static_objects[OBJ_COW]), 0);

	draw_animated_tiger();

}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	display_camera(0);
	glutSwapBuffers();
}

void initialize_camera(void);
void motion_translate_xaxis(int x, int y);
void motion_translate_yaxis(int x, int y);
void motion_translate_zaxis(int x, int y);
void motion_rotate_xaxis(int x, int y);
void motion_rotate_yaxis(int x, int y);
void motion_rotate_zaxis(int x, int y);

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'r':					// Translation for x axis
		glutMotionFunc(motion_translate_xaxis);
		glutPostRedisplay();
		break;
	case 'g':					// Translation for y axis
		glutMotionFunc(motion_translate_xaxis);
		glutPostRedisplay();
		break;
	case 'b':					// Translation for z axis
		glutMotionFunc(motion_translate_xaxis);
		glutPostRedisplay();
		break;
	case 'x':					// Rotation for x axis
		glutMotionFunc(motion_rotate_xaxis);
		glutPostRedisplay();
		break;
	case 'y':					// Rotation for y axis
		glutMotionFunc(motion_rotate_yaxis);
		glutPostRedisplay();
		break;
	case 'z':					// Rotation for z axis
		glutMotionFunc(motion_rotate_zaxis);
		glutPostRedisplay();
		break;
	case 'i':					// Zoom in
		camera[camera_selected].fov_y = camera[camera_selected].fov_y - camera[camera_selected].zoom_factor;
		if (camera[camera_selected].fov_y <= 0.0f) // fov_y가 0이 되면 화면에 아무것도 안보임, 이후 화면 뒤집힘
			camera[camera_selected].fov_y = 1.0f;
		ProjectionMatrix[camera_selected] = glm::perspective(camera[camera_selected].fov_y*TO_RADIAN, camera[camera_selected].aspect_ratio, 
																camera[camera_selected].near_clip, camera[camera_selected].far_clip);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		//printf("%f\n", camera[camera_selected].fov_y);
		glutPostRedisplay();
		break;
	case 'o':					// Zoom out
		camera[camera_selected].fov_y = camera[camera_selected].fov_y + camera[camera_selected].zoom_factor;
		if (camera[camera_selected].fov_y >= 180.0f) // fov_y가 180이 되면 화면에 아무것도 안보임, 이후 화면 뒤집힘
			camera[camera_selected].fov_y = 179.0f;
		ProjectionMatrix[camera_selected] = glm::perspective(camera[camera_selected].fov_y*TO_RADIAN, camera[camera_selected].aspect_ratio,
			camera[camera_selected].near_clip, camera[camera_selected].far_clip);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		//printf("%f\n", camera[camera_selected].fov_y);
		glutPostRedisplay();
		break;
	case ',':		// Increase zoom factor
		camera[camera_selected].zoom_factor += 1.0f;
		glutPostRedisplay();
		break;
	case '.':		// Decrease zoom factor
		camera[camera_selected].zoom_factor -= 1.0f;
		if(camera[camera_selected].zoom_factor <= 0.0f)
			camera[camera_selected].zoom_factor = 1.0f;
		glutPostRedisplay();
		break;
	case '1':					// static CCTV 1
		glutPostRedisplay();
		break;
	case '2':					// static CCTV 2
		glutPostRedisplay();
		break;
	case '3':					// static CCTV 3
		glutPostRedisplay();
		break;
	case '4':					// dynamic CCTV
		glutPostRedisplay();
		break;

	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	}
}

/* 원본 reshape
void reshape(int width, int height) {
	float aspect_ratio;
	aspect_ratio = (float)width / height;
	glViewport(0, 0, width, height);   -> display_camera()에서 처리
	ProjectionMatrix = glm::perspective(15.0f*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);

	glutPostRedisplay();
}
*/
void reshape(int width, int height) {
	camera[0].aspect_ratio = (float)width / height;	// viewport비율과 projection비율을 동기화시킴
	viewport[0].x = viewport[0].y = 0;
	viewport[0].w = width; viewport[0].h = height;
	ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
	ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];

	camera[1].aspect_ratio = camera[0].aspect_ratio; // for the time being ...
	viewport[1].x = (int)(0.70f*width); viewport[1].y = (int)(0.70f*height);
	viewport[1].w = (int)(0.30f*width); viewport[1].h = (int)(0.30*height);
	ProjectionMatrix[1] = glm::perspective(camera[1].fov_y*TO_RADIAN, camera[1].aspect_ratio, camera[1].near_clip, camera[1].far_clip);
	ViewProjectionMatrix[1] = ProjectionMatrix[1] * ViewMatrix[1];

	glutPostRedisplay();
}

unsigned int leftbutton_pressed = 0;
int prevx, prevy;

void timer_scene(int timestamp_scene) {
	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		prevx = x, prevy = y;
		leftbutton_pressed = 1;
		glutPostRedisplay();
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
		leftbutton_pressed = 0;
		glutPostRedisplay();
	}
}

#define CAM_ROT_SENSITIVITY 0.15f
void motion_1(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*delx*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	//좌우움직이면 v벡터 기준으로 360도 회전 가능
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].vrp);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		
		vec3_tmp = glm::cross(camera[camera_selected].vup, camera[camera_selected].vrp - camera[camera_selected].prp);		// vrp-prp = -n vector   // result of cross is -u vector
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*dely*TO_RADIAN, vec3_tmp);	// rotate for -u vector
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].vrp);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));
		

		ViewMatrix[0] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		glutPostRedisplay();
	}
}
void motion_translate_xaxis(int x, int y) {
	glutPostRedisplay();
}
void motion_translate_yaxis(int x, int y) {
	glutPostRedisplay();
}
void motion_translate_zaxis(int x, int y) {
	glutPostRedisplay();
}
void motion_rotate_xaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*dely*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].vrp);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[0] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		glutPostRedisplay();
	}
}
void motion_rotate_yaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*dely*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].vrp);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[0] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		glutPostRedisplay();
	}
}
void motion_rotate_zaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*delx*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));	//좌우움직이면 v벡터 기준으로 360도 회전 가능
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].vrp);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[0] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		glutPostRedisplay();
	}
}

void register_callbacks(void) {
	cc.left_button_status = GLUT_UP;

	glutMouseFunc(mousepress);
	glutMotionFunc(motion_1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_camera(void) {
// example 4.5.3을 이용하여 camera세팅
/*	주어진 lootAt함수 파라미터
	lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
	glm::vec3(0.0f, 0.0f, 1.0f));
*/
	// initialize the 0th camera.
	camera[0].prp = glm::vec3(600.0f, 600.0f, 200.0f);	// 카메라 위치
	//camera[0].prp = glm::vec3(120.0f, 70.0f, 35.0f);
	camera[0].vrp = glm::vec3(125.0f, 80.0f, 25.0f);		// 바라보는 곳
	camera[0].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	//u,v,n벡터를 lookAt으로 세팅
	ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
	//camera[0].vup = glm::vec3(ViewMatrix[0][0].y, ViewMatrix[0][1].y, ViewMatrix[0][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 0", ViewMatrix[0]);
#endif
//	ProjectionMatrix = glm::perspective(15.0f*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);

	camera[0].fov_y = 15.0f;
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[0].near_clip = 1.0f;
	camera[0].far_clip = 10000.0f;
	camera[0].zoom_factor = 1.0f; // will be used for zoomming in and out.

	/*
//initialize the 1st camera.
	camera[1].prp = glm::vec3(0.0f, 50.0f, 0.0f);
	camera[1].vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera[1].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[1] = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);
	camera[1].vup = glm::vec3(ViewMatrix[1][0].y, ViewMatrix[1][1].y, ViewMatrix[1][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 1", ViewMatrix[1]);
#endif

	camera[1].fov_y = 16.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[1].near_clip = 0.1f;
	camera[1].far_clip = 100.0f;
	camera[1].zoom_factor = 1.0f; // will be used for zoomming in and out.
	*/
	camera_selected = 0;
}

void initialize_OpenGL(void) {
	initialize_camera();		// openGL시작하면서 카메라 세팅

	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
	//기존에 주어진 카메라 세팅부분
	if (0) {
		ViewMatrix[0] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f));
	}
	if (0) {
		ViewMatrix[0] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}
	/* Used in initialize_camera()
	if (1) {
		ViewMatrix[0] = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}
	*/
}

void prepare_scene(void) {
	define_axes();
	define_static_objects();
	define_animated_tiger();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170 Our_House_GLSL_V_0.5";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'c', 'f', 'd', 'ESC'" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
