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
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ModelViewMatrix, ViewMatrix, ProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
//
#include "Object_Definitions.h"
//////////////////////////////////////////////////////////////////
//////// Setting Cameras /////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define NUMBER_OF_CAMERAS 8		// main, static cctv * 3, dynamic cctv, front view, side view, top view
#define CAM_TRANSLATION_SPEED 0.025f
#define CAM_ROTATION_SPEED 0.1f

typedef struct _CAMERA {								// 카메라 세팅
	glm::vec3 pos;										// 카메라 위치	
	glm::vec3 uaxis, vaxis, naxis;						// 카메라 방향을 정하는 u,v,n벡터
	float fov_y, aspect_ratio, near_clip, far_clip;		// field of view(시야각) / aspect_ratio : 가로 세로 비율 <- fov_y로 상하각을 정하면 비율을 통해 좌우각을 정할 수 있다.
	int move_status;
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int camera_selected;

typedef struct _VIEWPORT {
	int x, y, w, h;
} VIEWPORT;
VIEWPORT viewport[NUMBER_OF_CAMERAS];					// 뷰 포트 세팅

// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 CameraViewProjectionMatrix[NUMBER_OF_CAMERAS], CameraViewMatrix[NUMBER_OF_CAMERAS], CameraProjectionMatrix[NUMBER_OF_CAMERAS];
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 CameraModelViewProjectionMatrix; // This one is sent to vertex shader when it is ready.


/*
[0][1][2][3]
[x] ux uy uz 0
[y] vx vy vz 0
[z] nx ny nz 0
[w]  0  0  0 1
*/
void set_ViewMatrix_from_camera_frame(int cam_index) {
	CameraViewMatrix[cam_index] = glm::mat4(1.0f);
	CameraViewMatrix[cam_index][0].x = camera[cam_index].uaxis.x;
	CameraViewMatrix[cam_index][0].y = camera[cam_index].vaxis.x;
	CameraViewMatrix[cam_index][0].z = camera[cam_index].naxis.x;

	CameraViewMatrix[cam_index][1].x = camera[cam_index].uaxis.y;
	CameraViewMatrix[cam_index][1].y = camera[cam_index].vaxis.y;
	CameraViewMatrix[cam_index][1].z = camera[cam_index].naxis.y;

	CameraViewMatrix[cam_index][2].x = camera[cam_index].uaxis.z;
	CameraViewMatrix[cam_index][2].y = camera[cam_index].vaxis.z;
	CameraViewMatrix[cam_index][2].z = camera[cam_index].naxis.z;

	CameraViewMatrix[cam_index] = glm::translate(CameraViewMatrix[cam_index], -camera[cam_index].pos);	// 카메라를 WC의 원점으로 평행이동 한 후에 로테이션
}

//카메라 기준점 pos(vec3타입)을 옮김. 카메라를 axis를 축을 기준으로 del에 비례하여 이동시켜라.
void renew_cam_position_along_axis(int cam_index, float del, glm::vec3 trans_axis) {
	camera[cam_index].pos += CAM_TRANSLATION_SPEED * del*trans_axis;
}

//	 pos   ->     pos'
// (x,y,z) = (x,y,z) + alpha*(nx,ny,nz)
void renew_cam_orientation_rotation_around_axis(int cam_index, float angle, glm::vec3 rot_axis) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED*TO_RADIAN*angle, rot_axis));	// axis축 둘레로 angle만큼 회전

	camera[cam_index].uaxis = RotationMatrix * camera[cam_index].uaxis;	// 카메라 축 각각을 회전시킴(벡터의 끝 꼭지점을 회전시켜도 같음)
	camera[cam_index].vaxis = RotationMatrix * camera[cam_index].vaxis;
	camera[cam_index].naxis = RotationMatrix * camera[cam_index].naxis;
}

//////////////////////////////////////////////////////////////////
/////// End of Setting Cameras ///////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef struct _CALLBACK_CONTEXT {
	int left_button_status;
	int prevx, prevy;
} CALLBACK_CONTEXT;
CALLBACK_CONTEXT cc;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
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

	glutSwapBuffers();
}

void initialize_camera(void);

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
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

void reshape(int width, int height) {
	float aspect_ratio;
	glViewport(0, 0, width, height);
	
	aspect_ratio = (float)width / height;
	ProjectionMatrix = glm::perspective(15.0f*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);

	glutPostRedisplay();
}

void timer_scene(int timestamp_scene) {
	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void register_callbacks(void) {
	cc.left_button_status = GLUT_UP;

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
	// called only once when the OpenGL system is initialized!!!
	// only ViewMatrix[*] are set up in this function.
	// ProjectionMatrix will be initialized in the reshape callback function when the window pops up.

	// Camera 0
	camera[0].pos = glm::vec3(25.0f, 0.5f, 0.0f);
	camera[0].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[0].vaxis = glm::vec3(0.0f, 1.0f, 0.0f);
	camera[0].naxis = glm::vec3(1.0f, 0.0f, 0.0f);

	// perspective projection에 사용되는 정보
	camera[0].move_status = 0;
	camera[0].fov_y = 30.0f;		// field of view : 카메라 상하각도가 30도(중앙을 기준으로 위아래 각각 15도) -> 물체를 확대해서 보려면 각도를 좁히면 됨(망원렌즈)
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window pops up.(reshape()내에서 다시 지정해줌)
	camera[0].near_clip = 0.01f;
	camera[0].far_clip = 500.0f;

	set_ViewMatrix_from_camera_frame(0);

	//Camera 1
	camera[1].pos = glm::vec3(0.0f, 50.0f, 0.0f);
	camera[1].uaxis = glm::vec3(0.0f, 0.0f, -1.0f);
	camera[1].vaxis = glm::vec3(-1.0f, 0.0f, 0.0f);
	camera[1].naxis = glm::vec3(0.0f, 1.0f, 0.0f);

	camera[1].move_status = 0;
	camera[1].fov_y = 15.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[1].near_clip = 0.01f;
	camera[1].far_clip = 500.0f;

	set_ViewMatrix_from_camera_frame(1);

	camera_selected = 0;
}

void initialize_OpenGL(void) {
	initialize_camera();		// openGL시작하면서 카메라 세팅

	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);

	if (0) {
		ViewMatrix = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f));
	}
	if (0) {
		ViewMatrix = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if (1) {
		ViewMatrix = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}
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
