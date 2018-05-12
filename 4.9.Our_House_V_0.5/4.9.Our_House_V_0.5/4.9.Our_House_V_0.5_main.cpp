#include <math.h>
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
	float left, right, bottom, top;
	glm::vec3 uaxis, vaxis, naxis;
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
glm::vec3 init_camera_vup;
glm::mat4 mat_rotate = glm::mat4(1.0f);
float angle_rotate_u = 0.0f;
float angle_rotate_v = 0.0f;
float angle_rotate_n = 0.0f;
glm::vec3 rotate_axis_u = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 rotate_axis_v = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 rotate_axis_n = glm::vec3(0.0f, 0.0f, 1.0f);
int camera_selected; // 0 for main_camera, 7 for dynamic_cctv 

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

#define VIEW_CAMERA 0
#define VIEW_CCTV	1
int view_mode;
void display_camera(int cam_index){ // display()함수로 인해 매초마다 불러짐.

	glViewport(viewport[cam_index].x, viewport[cam_index].y, viewport[cam_index].w, viewport[cam_index].h);

	switch(view_mode) {
	case VIEW_CAMERA:
		glLineWidth(2.0f);
		draw_axes(cam_index);
		draw_main_camera_axes(cam_index);
		//draw_frustum(cam_index);
		draw_frustum_line(cam_index);
		glLineWidth(1.0f);

		draw_static_object(&(static_objects[OBJ_BUILDING]), 0, cam_index);

		draw_static_object(&(static_objects[OBJ_TABLE]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_TABLE]), 1, cam_index);	// takes given teapot

		draw_static_object(&(static_objects[OBJ_LIGHT]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 1, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 2, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 3, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 4, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 5, cam_index);			// NEW OBJ_LIGHT

		draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, cam_index);	// on the OBJ_TABLE 1
		draw_static_object(&(static_objects[OBJ_TEAPOT]), 1, cam_index);			// NEW OBJ_TEAPOT
		draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 1, cam_index);		// NEW OBJ_NEW_CHAIR
		draw_static_object(&(static_objects[OBJ_FRAME]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_FRAME]), 1, cam_index);			// NEW OBJ_FRAME
		draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 1, cam_index);		// NEW OBJ_NEW_PICTURE
		draw_static_object(&(static_objects[OBJ_COW]), 0, cam_index);

		draw_animated_tiger(cam_index);
		break;
	case VIEW_CCTV:
		glLineWidth(2.0f);
		draw_axes(cam_index);
		glLineWidth(1.0f);

		draw_static_object(&(static_objects[OBJ_BUILDING]), 0, cam_index);

		draw_static_object(&(static_objects[OBJ_TABLE]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_TABLE]), 1, cam_index);	// takes given teapot

		draw_static_object(&(static_objects[OBJ_LIGHT]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 1, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 2, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 3, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 4, cam_index);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 5, cam_index);			// NEW OBJ_LIGHT

		draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, cam_index);	// on the OBJ_TABLE 1
		draw_static_object(&(static_objects[OBJ_TEAPOT]), 1, cam_index);			// NEW OBJ_TEAPOT
		draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 1, cam_index);		// NEW OBJ_NEW_CHAIR
		draw_static_object(&(static_objects[OBJ_FRAME]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_FRAME]), 1, cam_index);			// NEW OBJ_FRAME
		draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0, cam_index);
		draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 1, cam_index);		// NEW OBJ_NEW_PICTURE
		draw_static_object(&(static_objects[OBJ_COW]), 0, cam_index);

		draw_animated_tiger(cam_index);
		break;
	} // END OF SWITCH(VIEW_MODE)
}
void display(void) {   // 매초마다 불러짐
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (view_mode) {
	case VIEW_CAMERA:
		display_camera(0);	// main_camera
		display_camera(1);	// front_view
		display_camera(2);	// side_view
		display_camera(3);	// top_view
		break;
	case VIEW_CCTV:
		display_camera(4);	// static cctv 1
		display_camera(5);	// static cctv 2
		display_camera(6);	// static cctv 3
		display_camera(7);	// dynamic cctv
		break;
	}
	glutSwapBuffers();
}

void initialize_camera(void);
void motion_translate_uaxis(int x, int y);
void motion_translate_vaxis(int x, int y);
void motion_translate_naxis(int x, int y);
void motion_rotate_uaxis(int x, int y);
void motion_rotate_vaxis(int x, int y);
void motion_rotate_naxis(int x, int y);
void motion_1(int x, int y);

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'x':					// Translation for u axis
		glutMotionFunc(motion_translate_uaxis);
		glutPostRedisplay();
		break;
	case 'y':					// Translation for v axis
		glutMotionFunc(motion_translate_vaxis);
		glutPostRedisplay();
		break;
	case 'z':					// Translation for n axis
		glutMotionFunc(motion_translate_naxis);
		glutPostRedisplay();
		break;
	case 'u':					// Rotation for u axis
		glutMotionFunc(motion_rotate_uaxis);
		glutPostRedisplay();
		break;
	case 'v':					// Rotation for v axis
		glutMotionFunc(motion_rotate_vaxis);
		glutPostRedisplay();
		break;
	case 'n':					// Rotation for n axis
		glutMotionFunc(motion_rotate_naxis);
		glutPostRedisplay();
		break;
	case 'i':					// Zoom in
		camera[camera_selected].fov_y = camera[camera_selected].fov_y - camera[camera_selected].zoom_factor;
		if (camera[camera_selected].fov_y <= 0.0f) // fov_y가 0이 되면 화면에 아무것도 안보임, 이후 화면 뒤집힘
			camera[camera_selected].fov_y = 1.0f;

		ProjectionMatrix[camera_selected] = glm::perspective(camera[camera_selected].fov_y*TO_RADIAN, camera[camera_selected].aspect_ratio, camera[camera_selected].near_clip, camera[camera_selected].far_clip);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		printf("camera_selected = %d\n", camera_selected);
		printf("fov_y = %f\n", camera[camera_selected].fov_y);
		printf("asp_ratio = %f\n", camera[camera_selected].aspect_ratio);
		glutPostRedisplay();
		break;
	case 'o':					// Zoom out
		camera[camera_selected].fov_y = camera[camera_selected].fov_y + camera[camera_selected].zoom_factor;
		if (camera[camera_selected].fov_y >= 180.0f) // fov_y가 180이 되면 화면에 아무것도 안보임, 이후 화면 뒤집힘
			camera[camera_selected].fov_y = 179.0f;

		ProjectionMatrix[camera_selected] = glm::perspective(camera[camera_selected].fov_y*TO_RADIAN, camera[camera_selected].aspect_ratio,	camera[camera_selected].near_clip, camera[camera_selected].far_clip);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		printf("camera_selected = %d\n", camera_selected);
		printf("fov_y = %f\n", camera[camera_selected].fov_y);
		printf("asp_ratio = %f\n", camera[camera_selected].aspect_ratio);
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

	case '/':		// Initialize main_camera or dynamic_cctv
		switch(view_mode){
			case VIEW_CAMERA:
				camera[0].prp = glm::vec3(202.0f, 38.0f, 14.0f);	// 카메라 위치
				camera[0].vrp = glm::vec3(197.0f, 86.0f, 13.0f);		// 바라보는 곳
				camera[0].vup = init_camera_vup;
				camera[camera_selected].fov_y = 50.0f;
				angle_rotate_u = 0.0f;
				angle_rotate_v = 0.0f;
				rotate_axis_u = glm::vec3(0.0f, 0.0f, 1.0f);
				rotate_axis_v = glm::vec3(0.0f, 0.0f, 1.0f);
				break;

			case VIEW_CCTV:
				camera[camera_selected].prp = glm::vec3(50.0f, 50.0f, 50.0f);	// 카메라 위치
				camera[camera_selected].vrp = glm::vec3(10.0f, 50.0f, 20.0f);		// 바라보는 곳
				camera[camera_selected].vup = glm::vec3(0.0f, 0.0f, 1.0f);
				camera[camera_selected].fov_y = 100.0f;
				break;
		}
		//u,v,n벡터를 lookAt으로 세팅
		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);
		camera[camera_selected].zoom_factor = 1.0f;
		ProjectionMatrix[camera_selected] = glm::perspective(camera[camera_selected].fov_y*TO_RADIAN, camera[camera_selected].aspect_ratio, camera[camera_selected].near_clip, camera[camera_selected].far_clip);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
		break;

	case 's':					// change mode between CAMERA MODE and CCTV MODE
		camera_selected = 7-camera_selected;
		view_mode = 1 - view_mode;
		glutPostRedisplay();
		break;
	case 'm':
		glutMotionFunc(motion_1);
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

void reshape(int width, int height) {
// VIEW_CAMERA mode
	camera[0].aspect_ratio = (float)width / height;	// viewport비율과 projection비율을 동기화시킴
	//printf("aspect_ratio = %f\n\n", camera[0].aspect_ratio);
	viewport[0].x = viewport[0].y = 0;
	viewport[0].w = (int)(0.70f*width); viewport[0].h = (int)(0.70f*height);
	ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
	ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];

// front view
	camera[1].aspect_ratio = camera[0].aspect_ratio;
	viewport[1].x = (int)(0.70f*width); viewport[1].y = (int)(0.70f*height);
	viewport[1].w = (int)(0.30f*width); viewport[1].h = (int)(0.30*height);
	ProjectionMatrix[1] = glm::ortho(camera[1].left, camera[1].right, camera[1].bottom, camera[1].top, camera[1].near_clip, camera[1].far_clip);
	ViewProjectionMatrix[1] = ProjectionMatrix[1] * ViewMatrix[1];
	
// side view
	camera[2].aspect_ratio = camera[0].aspect_ratio;
	viewport[2].x = (int)(0.70f*width); viewport[2].y = (int)(0.40f*height);
	viewport[2].w = (int)(0.30f*width); viewport[2].h = (int)(0.30*height);
	ProjectionMatrix[2] = glm::ortho(camera[2].left, camera[2].right, camera[2].bottom, camera[2].top, camera[2].near_clip, camera[2].far_clip);
	ViewProjectionMatrix[2] = ProjectionMatrix[2] * ViewMatrix[2];

// top view
	camera[3].aspect_ratio = camera[0].aspect_ratio;
	viewport[3].x = (int)(0.70f*width); viewport[3].y = (int)(0.05f*height);
	viewport[3].w = (int)(0.30f*width); viewport[3].h = (int)(0.30*height);
	ProjectionMatrix[3] = glm::ortho(camera[3].left, camera[3].right, camera[3].bottom, camera[3].top, camera[3].near_clip, camera[3].far_clip);
	ViewProjectionMatrix[3] = ProjectionMatrix[3] * ViewMatrix[3];

// VIEW_CCTV mode
	// static_cctv_1
	camera[4].aspect_ratio = (float)width / height;	// viewport비율과 projection비율을 동기화시킴
	viewport[4].x = 0; viewport[4].y = (int)(0.70f*height);
	viewport[4].w = (int)(0.30f*width); viewport[4].h = (int)(0.30f*height);
	ProjectionMatrix[4] = glm::perspective(camera[4].fov_y*TO_RADIAN, camera[4].aspect_ratio, camera[4].near_clip, camera[4].far_clip);
	ViewProjectionMatrix[4] = ProjectionMatrix[4] * ViewMatrix[4];

	// static_cctv_2
	camera[5].aspect_ratio = camera[4].aspect_ratio;
	viewport[5].x = 0; viewport[5].y = (int)(0.35f*height);
	viewport[5].w = (int)(0.30f*width); viewport[5].h = (int)(0.30*height);
	ProjectionMatrix[5] = glm::perspective(camera[5].fov_y*TO_RADIAN, camera[5].aspect_ratio, camera[5].near_clip, camera[5].far_clip);
	ViewProjectionMatrix[5] = ProjectionMatrix[5] * ViewMatrix[5];

	// static_cctv_3
	camera[6].aspect_ratio = camera[4].aspect_ratio;
	viewport[6].x = 0; viewport[6].y = 0;
	viewport[6].w = (int)(0.30f*width); viewport[6].h = (int)(0.30*height);
	ProjectionMatrix[6] = glm::perspective(camera[6].fov_y*TO_RADIAN, camera[6].aspect_ratio, camera[6].near_clip, camera[6].far_clip);
	ViewProjectionMatrix[6] = ProjectionMatrix[6] * ViewMatrix[6];
	
	// dynamic_cctv
	camera[7].aspect_ratio = camera[4].aspect_ratio;
	viewport[7].x = (int)(0.35f*width); viewport[7].y = 0;
	viewport[7].w = (int)(0.65f*width); viewport[7].h = (int)(0.65*height);
	ProjectionMatrix[7] = glm::perspective(camera[7].fov_y*TO_RADIAN, camera[7].aspect_ratio, camera[7].near_clip, camera[7].far_clip);
	ViewProjectionMatrix[7] = ProjectionMatrix[7] * ViewMatrix[7];

	glutPostRedisplay();
}

unsigned int leftbutton_pressed = 0;
int prevx, prevy;

// 호랑이 움직임관련 함수
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
#define CAM_TRA_SENSITIVITY 1.0f
void motion_1(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed && view_mode==VIEW_CAMERA) {
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
		

		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
	}
}

void motion_translate_uaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	glm::vec3 vec3_unit_uaxis;
	//float sqrt_vector;
	float delx;

	if (leftbutton_pressed && view_mode == VIEW_CAMERA) {
		delx = (float)(x - prevx);
		prevx = x;

		vec3_tmp = glm::cross(camera[camera_selected].vup, camera[camera_selected].prp - camera[camera_selected].vrp);
		//sqrt_vector = sqrt((vec3_tmp.x*vec3_tmp.x) + (vec3_tmp.y*vec3_tmp.y) + (vec3_tmp.z*vec3_tmp.z));
		//vec3_unit_uaxis = glm::vec3( vec3_tmp.x / sqrt_vector, vec3_tmp.y / sqrt_vector, vec3_tmp.z / sqrt_vector);
		vec3_unit_uaxis = glm::normalize(vec3_tmp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), delx*vec3_unit_uaxis);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vrp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		
		printf("prp.x = %f\t prp.y = %f\t prp.z = %f\nvrp.x = %f\t vrp.y = %f\t vrp.z = %f\nvup.x = %f\t vup.y = %f\t vup.z = %f\n\n",
				camera[camera_selected].prp.x, camera[camera_selected].prp.y, camera[camera_selected].prp.z,
				camera[camera_selected].vrp.x, camera[camera_selected].vrp.y, camera[camera_selected].vrp.z,
				camera[camera_selected].vup.x, camera[camera_selected].vup.y, camera[camera_selected].vup.z);
		glutPostRedisplay();
	}
}
void motion_translate_vaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float dely;

	if (leftbutton_pressed && view_mode == VIEW_CAMERA) {
		dely = -(float)(y - prevy);
		prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), dely*camera[camera_selected].vup);
		
		// prp(카메라 위치), vrp(카메라가 바라보는 곳)은 변경  // vup(카메라 위)도 변경 - prp가 움직이니까 vup도 따라가야함.
		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vrp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

		printf("prp.x = %f\t prp.y = %f\t prp.z = %f\nvrp.x = %f\t vrp.y = %f\t vrp.z = %f\nvup.x = %f\t vup.y = %f\t vup.z = %f\n\n",
			camera[camera_selected].prp.x, camera[camera_selected].prp.y, camera[camera_selected].prp.z,
			camera[camera_selected].vrp.x, camera[camera_selected].vrp.y, camera[camera_selected].vrp.z,
			camera[camera_selected].vup.x, camera[camera_selected].vup.y, camera[camera_selected].vup.z);
		glutPostRedisplay();
	}
}
void motion_translate_naxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	glm::vec3 vec3_unit_naxis;
	//float sqrt_vector;
	float dely;

	if (leftbutton_pressed && view_mode == VIEW_CAMERA) {
		dely = -(float)(y - prevy);
		prevy = y;

		vec3_tmp = camera[camera_selected].prp - camera[camera_selected].vrp;			// prp-vrp = n벡터  <- 이 모든건 카메라 축 기준
		//sqrt_vector = sqrt((vec3_tmp.x*vec3_tmp.x) + (vec3_tmp.y*vec3_tmp.y) + (vec3_tmp.z*vec3_tmp.z));
		//vec3_unit_naxis = glm::vec3(vec3_tmp.x / sqrt_vector, vec3_tmp.y / sqrt_vector, vec3_tmp.z / sqrt_vector);
		vec3_unit_naxis = glm::normalize(vec3_tmp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), -dely * vec3_unit_naxis);

		camera[camera_selected].prp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].prp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vrp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

		printf("prp.x = %f\t prp.y = %f\t prp.z = %f\nvrp.x = %f\t vrp.y = %f\t vrp.z = %f\nvup.x = %f\t vup.y = %f\t vup.z = %f\n\n",
			camera[camera_selected].prp.x, camera[camera_selected].prp.y, camera[camera_selected].prp.z,
			camera[camera_selected].vrp.x, camera[camera_selected].vrp.y, camera[camera_selected].vrp.z,
			camera[camera_selected].vup.x, camera[camera_selected].vup.y, camera[camera_selected].vup.z);
		glutPostRedisplay();
	}
}

void motion_rotate_uaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float dely;

	if (leftbutton_pressed) {
		dely = -(float)(y - prevy);
		prevy = y;

		// prp-vrp = n벡터 // vup X n = u벡터   <- 이 모든건 카메라 축 기준
		vec3_tmp = glm::cross(camera[camera_selected].vup, camera[camera_selected].prp - camera[camera_selected].vrp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*dely*TO_RADIAN, vec3_tmp);		// u벡터(카메라의 오른쪽)를 둘레로 회전 : 양의 각도로 회전 시, 카메라가 고개를 쳐드니까 화면은 아래로 내려감
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].prp);
		angle_rotate_u += CAM_ROT_SENSITIVITY * dely*TO_RADIAN;
		rotate_axis_u = vec3_tmp;

		camera[camera_selected].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vrp, 1.0f));	// affine transformation of point (x,y,z,1)

		if(view_mode==VIEW_CAMERA) // CCTV는 vup을 (0,0,1)로 고정
			camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

		printf("prp.x = %f\t prp.y = %f\t prp.z = %f\nvrp.x = %f\t vrp.y = %f\t vrp.z = %f\nvup.x = %f\t vup.y = %f\t vup.z = %f\n\n",
			camera[camera_selected].prp.x, camera[camera_selected].prp.y, camera[camera_selected].prp.z,
			camera[camera_selected].vrp.x, camera[camera_selected].vrp.y, camera[camera_selected].vrp.z,
			camera[camera_selected].vup.x, camera[camera_selected].vup.y, camera[camera_selected].vup.z);
		glutPostRedisplay();
	}
}
void motion_rotate_vaxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	glm::vec4 uaxis, vaxis, naxis;
	float delx;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx);
		prevx = x;
		// prp를 원점으로 만든 뒤, 원하는만큼 회전 시키고, 다시 돌아옴. 
		// vup벡터(=v벡터)는 고정, u, n벡터가 회전
		// prp-vrp = n벡터 // vup X n = u벡터   <- 이 모든건 카메라 축 기준
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*-delx*TO_RADIAN, camera[camera_selected].vup);	//좌우움직이면 vup벡터 기준으로 360도 회전 가능
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].prp);
		angle_rotate_v += CAM_ROT_SENSITIVITY * -delx * TO_RADIAN;
		rotate_axis_v = camera[camera_selected].vup;

		camera[camera_selected].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vrp, 1.0f));	// affine transformation of point (x,y,z,1)

		// v축 둘레로 회전하니까 vup 고정
		//if (view_mode == VIEW_CAMERA) camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)
		
		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);
		
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

		printf("prp.x = %f\t prp.y = %f\t prp.z = %f\nvrp.x = %f\t vrp.y = %f\t vrp.z = %f\nvup.x = %f\t vup.y = %f\t vup.z = %f\n\n",
			camera[camera_selected].prp.x, camera[camera_selected].prp.y, camera[camera_selected].prp.z,
			camera[camera_selected].vrp.x, camera[camera_selected].vrp.y, camera[camera_selected].vrp.z,
			camera[camera_selected].vup.x, camera[camera_selected].vup.y, camera[camera_selected].vup.z);
		glutPostRedisplay();
	}
}
void motion_rotate_naxis(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx;

	if (leftbutton_pressed && view_mode == VIEW_CAMERA) {
		delx = (float)(x - prevx);
		prevx = x;

		vec3_tmp = camera[camera_selected].prp - camera[camera_selected].vrp;			// prp-vrp = n벡터  <- 이 모든건 카메라 축 기준
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera[camera_selected].prp);
		// n벡터(카메라의 뒤쪽)를 둘레로 회전 : 양의 각도로 회전 시, 카메라가 고개를 왼쪽으로 까딱하니까 화면은 오른쪽으로 돌아감
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY*delx*TO_RADIAN, vec3_tmp);	
		mat4_tmp = glm::translate(mat4_tmp, -camera[camera_selected].prp);
		angle_rotate_n += CAM_ROT_SENSITIVITY *delx*TO_RADIAN;
		rotate_axis_n = vec3_tmp;

		camera[camera_selected].vrp = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vrp, 1.0f));	// affine transformation of point (x,y,z,1)
		camera[camera_selected].vup = glm::vec3(mat4_tmp*glm::vec4(camera[camera_selected].vup, 0.0f));	// affine transformation of vector(x,y,z,0)

		ViewMatrix[camera_selected] = glm::lookAt(camera[camera_selected].prp, camera[camera_selected].vrp, camera[camera_selected].vup);

		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

		printf("prp.x = %f\t prp.y = %f\t prp.z = %f\nvrp.x = %f\t vrp.y = %f\t vrp.z = %f\nvup.x = %f\t vup.y = %f\t vup.z = %f\n\n",
			camera[camera_selected].prp.x, camera[camera_selected].prp.y, camera[camera_selected].prp.z,
			camera[camera_selected].vrp.x, camera[camera_selected].vrp.y, camera[camera_selected].vrp.z,
			camera[camera_selected].vup.x, camera[camera_selected].vup.y, camera[camera_selected].vup.z);
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
	// initialize the 0th camera.
	camera[0].prp = glm::vec3(202.0f, 38.0f, 14.0f);		// 카메라 위치
	camera[0].vrp = glm::vec3(197.0f, 86.0f, 13.0f);		// 바라보는 곳
	camera[0].vup = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 uaxis, vaxis, naxis;
	naxis = camera[0].prp - camera[0].vrp;
	naxis = glm::normalize(naxis);

	uaxis = glm::cross(camera[0].vup, naxis);
	uaxis = glm::normalize(uaxis);

	vaxis = glm::cross(naxis, uaxis);
	vaxis = glm::normalize(vaxis);

	init_camera_vup = camera[0].vup = vaxis;

	//uaxis = glm::cross(camera[camera_selected].vup, camera[camera_selected].prp - camera[camera_selected].vrp);
	//uaxis = glm::normalize(uaxis);
	camera[0].uaxis = uaxis;
	camera[0].vaxis = vaxis;
	camera[0].naxis = naxis;
	printf("vup.x = %f\tvup.y = %f\tvup.z = %f\n\n",vaxis.x,vaxis.y,vaxis.z);

	ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup); //u,v,n벡터를 lookAt으로 세팅

//	ProjectionMatrix = glm::perspective(15.0f*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);
	camera[0].fov_y = 30.0f;
	camera[0].aspect_ratio = 1.5f; // will be set when the viewing window popped up.
	camera[0].near_clip = 1.0f;
	camera[0].far_clip = 150.0f;
	camera[0].zoom_factor = 1.0f; // will be used for zoomming in and out.

//initialize the 1st camera. used for front_view
	camera[1].prp = glm::vec3(300.0f, 0.0f, 0.0f);	// 카메라 위치
	camera[1].vrp = glm::vec3(0.0f, 0.0f, 0.0f);		// 바라보는 곳
	camera[1].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[1] = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);

	camera[1].left = -40.0f;   // prp를 (0,0,0)으로 놓고 계산한 값
	camera[1].right = 200.f;   // 160 + 40
	camera[1].bottom = -20.0f; // 0 - 20
	camera[1].top = 70.0f;     // 50 + 20

	//camera[1].fov_y = 15.0f;
	//camera[1].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[1].near_clip = 10.0f;
	camera[1].far_clip = 600.0f;

//initialize the 2nd camera used for side_view
	
	camera[2].prp = glm::vec3(230.0f, 230.0f, 0.0f);	// 카메라 위치
	camera[2].vrp = glm::vec3(230.0f, 0.0f, 0.0f);		// 바라보는 곳
	camera[2].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[2] = glm::lookAt(camera[2].prp, camera[2].vrp, camera[2].vup);

	camera[2].left = -40.0f;  // prp를 (0,0,0)으로 놓고 계산한 값
	camera[2].right = 270.0f; // 230 + 40
	camera[2].bottom =-20.0f; // 0 - 20
	camera[2].top = 70.0f;	  // 50 + 20
	
	/*
	camera[2].prp = glm::vec3(0.0f, -100.0f, 0.0f);	// 카메라 위치
	camera[2].vrp = glm::vec3(0.0f, 230.0f, 0.0f);		// 바라보는 곳
	camera[2].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[2] = glm::lookAt(camera[2].prp, camera[2].vrp, camera[2].vup);

	camera[2].left = -40.0f;  // prp를 (0,0,0)으로 놓고 계산한 값
	camera[2].right = 270.0f; // 230 + 40
	camera[2].bottom = -20.0f; // 0 - 20
	camera[2].top = 70.0f;	  // 50 + 20
	*/
	//camera[2].fov_y = 15.0f;
	//camera[2].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[2].near_clip = 10.0f;
	camera[2].far_clip = 600.0f;
	/*
	camera[2].near_clip = 60.0f;
	camera[2].far_clip = 400.0f;
	*/
//initialize the 3rd camera used for top_view
	camera[3].prp = glm::vec3(230.0f, 0.0f, 1200.0f);	// 카메라 위치
	camera[3].vrp = glm::vec3(230.0f, 0.0f, 0.0f);		// 바라보는 곳
	camera[3].vup = glm::vec3(-10.0f, 0.0f, 0.0f);
	ViewMatrix[3] = glm::lookAt(camera[3].prp, camera[3].vrp, camera[3].vup);

	camera[3].left = -40.0f;  // prp를 (0,0,0)으로 놓고 계산한 값
	camera[3].right = 200.0f; // 160 + 40
	camera[3].bottom = -20.0f; // 0 - 20
	camera[3].top = 250.0f;	  // 230 + 20

	//camera[3].fov_y = 15.0f;
	//camera[3].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[3].near_clip = 930.0f;    // 1200 - (50 + 220)  - 천정에서 220 더
	camera[3].far_clip = 1220.0f;     // 1200 + (0 + 20)   - 바닥에서 20 더

// VIEW_CCTV
// static cctv 1
	camera[4].prp = glm::vec3(57.0f, 145.0f, 45.0f);	// 카메라 위치
	camera[4].vrp = glm::vec3(41.0f, 137.0f, 26.0f);		// 바라보는 곳
	camera[4].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[4] = glm::lookAt(camera[4].prp, camera[4].vrp, camera[4].vup);

	camera[4].fov_y = 100.0f;
	camera[4].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[4].near_clip = 1.0f;
	camera[4].far_clip = 10000.0f;
	
// static cctv 2
	camera[5].prp = glm::vec3(162.0f, 62.0f, 45.0f);	// 카메라 위치
	camera[5].vrp = glm::vec3(127.0f, 82.0f, 21.0f);		// 바라보는 곳
	camera[5].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[5] = glm::lookAt(camera[5].prp, camera[5].vrp, camera[5].vup);

	camera[5].fov_y = 60.0f;
	camera[5].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[5].near_clip = 1.0f;
	camera[5].far_clip = 10000.0f;

// static cctv 3
	camera[6].prp = glm::vec3(210.0f, 43.0f, 45.0f);	// 카메라 위치
	camera[6].vrp = glm::vec3(200.0f, 80.0f, 16.0f);		// 바라보는 곳
	camera[6].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[6] = glm::lookAt(camera[6].prp, camera[6].vrp, camera[6].vup);

	camera[6].fov_y = 80.0f;
	camera[6].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[6].near_clip = 1.0f;
	camera[6].far_clip = 10000.0f;

// dynamic cctv
	glm::vec3 cam7_tmp;

	camera[7].prp = glm::vec3(50.0f, 50.0f, 50.0f);	// 카메라 위치
	camera[7].vrp = glm::vec3(10.0f, 50.0f, 20.0f);		// 바라보는 곳
	camera[7].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[7] = glm::lookAt(camera[7].prp, camera[7].vrp, camera[7].vup);

	camera[7].fov_y = 100.0f;
	camera[7].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[7].near_clip = 1.0f;
	camera[7].far_clip = 10000.0f;
	camera[7].zoom_factor = 1.0f; // will be used for zoomming in and out.


	camera_selected = 0;
	view_mode = VIEW_CAMERA;
}

void initialize_OpenGL(void) {
	initialize_camera();		// openGL시작하면서 카메라 세팅

	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
	/*
	//기존에 주어진 카메라 세팅부분
	// top view
	if (0) {
		ViewMatrix[0] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f));
	}
	// front view
	if (0) {
		ViewMatrix[0] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}
	// Used in initialize_camera()
	if (0) {
		ViewMatrix[0] = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}
	*/
}

void prepare_scene(void) {
	define_axes();
	define_frustum();
	define_frustum_line();
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
