
// The object modelling tasks performed by this file are usually done 
// by reading a scene configuration file or through a help of graphics user interface!!!

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

// FOR DRAW CAR
#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2
// END OF FOR DRAW CAR

typedef struct _material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
} Material;

#define N_MAX_GEOM_COPIES 6
typedef struct _Object {
	char filename[512];

	GLenum front_face_mode; // clockwise or counter-clockwise
	int n_triangles;

	int n_fields; // 3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	GLfloat *vertices; // pointer to vertex array data
	GLfloat xmin, xmax, ymin, ymax, zmin, zmax; // bounding box <- compute this yourself

	GLuint VBO, VAO; // Handles to vertex buffer object and vertex array object

	int n_geom_instances;
	glm::mat4 ModelMatrix[N_MAX_GEOM_COPIES];
	Material material[N_MAX_GEOM_COPIES];
} Object;

#define N_MAX_STATIC_OBJECTS		8

Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
int n_static_objects = 0;

#define OBJ_BUILDING		0
#define OBJ_TABLE			1
#define OBJ_LIGHT			2
#define OBJ_TEAPOT			3
#define OBJ_NEW_CHAIR		4
#define OBJ_FRAME			5
#define OBJ_NEW_PICTURE		6
#define OBJ_COW				7

int read_geometry(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(*object, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
														  // fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void compute_AABB(Object *obj_ptr) {
	// Do it yourself.
}

void prepare_geom_of_static_object(Object *obj_ptr) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = obj_ptr->n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	obj_ptr->n_triangles = read_geometry(&(obj_ptr->vertices), n_bytes_per_triangle, obj_ptr->filename);

	// Initialize vertex buffer object.
	glGenBuffers(1, &(obj_ptr->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glBufferData(GL_ARRAY_BUFFER, obj_ptr->n_triangles*n_bytes_per_triangle, obj_ptr->vertices, GL_STATIC_DRAW);

	compute_AABB(obj_ptr);
	free(obj_ptr->vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &(obj_ptr->VAO));
	glBindVertexArray(obj_ptr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void define_static_objects(void) {
	// building
	strcpy(static_objects[OBJ_BUILDING].filename, "Data/Building1_vnt.geom");
	static_objects[OBJ_BUILDING].n_fields = 8;

	static_objects[OBJ_BUILDING].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUILDING]));

	static_objects[OBJ_BUILDING].n_geom_instances = 1;

	static_objects[OBJ_BUILDING].ModelMatrix[0] = glm::mat4(1.0f);

	static_objects[OBJ_BUILDING].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].exponent = 128.0f*0.1f;

	// table
	strcpy(static_objects[OBJ_TABLE].filename, "Data/Table_vn.geom");
	static_objects[OBJ_TABLE].n_fields = 6;

	static_objects[OBJ_TABLE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TABLE]));

	static_objects[OBJ_TABLE].n_geom_instances = 2;

	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[0],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[0].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[0].diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	static_objects[OBJ_TABLE].material[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[0].exponent = 128.0f*0.078125f;

	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[1],
		glm::vec3(0.5f, 0.5f, 0.5f));

	static_objects[OBJ_TABLE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[1].ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[1].diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	static_objects[OBJ_TABLE].material[1].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[1].exponent = 15.0f;

	// Light
	strcpy(static_objects[OBJ_LIGHT].filename, "Data/Light_vn.geom");
	static_objects[OBJ_LIGHT].n_fields = 6;

	static_objects[OBJ_LIGHT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(static_objects + OBJ_LIGHT);

	static_objects[OBJ_LIGHT].n_geom_instances = 6;		// Number of OBJ_LIGHT

	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[3],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[4],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[4].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].exponent = 128.0f*0.4f;

	// Added OBJ_LIGHT NO.5 - HK
	static_objects[OBJ_LIGHT].ModelMatrix[5] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[5] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[5],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[5].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].diffuse = glm::vec4(0.12f, 0.34f, 0.56f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].exponent = 128.0f*0.4f;

	// teapot
	strcpy(static_objects[OBJ_TEAPOT].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT].n_fields = 6;

	static_objects[OBJ_TEAPOT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT]));

	static_objects[OBJ_TEAPOT].n_geom_instances = 2;
	//vec3(198.0f, 120.0f, 0.0f) - OBJ_TABLE NO.0
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[0],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].exponent = 128.0f*0.6;

	// Added OBJ_TEAPOT NO.1 - HK
	//vec3(157.0f, 76.5f, 0.0f) - OBJ_TABLE NO.1
	static_objects[OBJ_TEAPOT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 50.0f, 12.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[1] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[1],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].diffuse = glm::vec4(0.34f, 0.12f, 0.56f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].exponent = 128.0f*0.6;

	// new_chair
	strcpy(static_objects[OBJ_NEW_CHAIR].filename, "Data/new_chair_vnt.geom");
	static_objects[OBJ_NEW_CHAIR].n_fields = 8;

	static_objects[OBJ_NEW_CHAIR].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_CHAIR]));

	static_objects[OBJ_NEW_CHAIR].n_geom_instances = 2;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].exponent = 128.0f*0.078125f;

	// Added OBJ_NEW_CHAIR NO.1 - HK
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 52.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[1],
		glm::vec3(1.2f, 1.2f, 1.2f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].diffuse = glm::vec4(0.12f, 0.34f, 0.56f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].exponent = 128.0f*0.078125f;

	// frame
	strcpy(static_objects[OBJ_FRAME].filename, "Data/Frame_vn.geom");
	static_objects[OBJ_FRAME].n_fields = 6;

	static_objects[OBJ_FRAME].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_FRAME]));

	static_objects[OBJ_FRAME].n_geom_instances = 2;

	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[0],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[0].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[0].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[0].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[0].exponent = 128.0f*0.1f;

	// Added OBJ_FRAME NO.1 - HK
	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, 20.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[1],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_FRAME].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[1].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[1].diffuse = glm::vec4(0.56f, 0.12f, 0.34f, 1.0f);
	static_objects[OBJ_FRAME].material[1].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[1].exponent = 128.0f*0.1f;

	// new_picture
	strcpy(static_objects[OBJ_NEW_PICTURE].filename, "Data/new_picture_vnt.geom");
	static_objects[OBJ_NEW_PICTURE].n_fields = 8;

	static_objects[OBJ_NEW_PICTURE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_PICTURE]));

	static_objects[OBJ_NEW_PICTURE].n_geom_instances = 2;
	// OBJ_FRAME 0 - (188.0f, 116.0f, 30.0f)
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].exponent = 128.0f*0.6f;

	// Added OBJ_NEW_PICTURE NO.1 - HK
	// OBJ_FRAME 1 - (25.0f, 15.0f, 30.0f)
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(26.5f, 20.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[1] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[1],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[1] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[1] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[1].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[1].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[1].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[1].exponent = 128.0f*0.6f;

	// COW
	strcpy(static_objects[OBJ_COW].filename, "Data/cow_vn.geom");
	static_objects[OBJ_COW].n_fields = 6;

	static_objects[OBJ_COW].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_COW]));

	static_objects[OBJ_COW].n_geom_instances = 1;

	static_objects[OBJ_COW].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::scale(static_objects[OBJ_COW].ModelMatrix[0],
		glm::vec3(30.0f, 30.0f, 30.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_COW].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_COW].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_COW].material[0].exponent = 0.21794872f*0.6f;

	n_static_objects = 8;
}

void draw_static_object(Object *obj_ptr, int instance_ID, int cam_index) {
	glFrontFace(obj_ptr->front_face_mode);

	ModelViewMatrix[cam_index] = ViewMatrix[cam_index] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[cam_index] * ModelViewMatrix[cam_index];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);		// rgb coloring
	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);
}

GLuint VBO_axes, VAO_axes;
GLfloat vertices_axes[6][3] = {
	{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

void define_axes(void) {
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define WC_AXIS_LENGTH		60.0f
void draw_axes(int cam_index) {
	ModelViewMatrix[cam_index] = glm::scale(ViewMatrix[cam_index], glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	ModelViewProjectionMatrix = ProjectionMatrix[cam_index] * ModelViewMatrix[cam_index];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}


// /*   // START OF FRUSTUM LINE
GLuint VBO_frustum_line, VAO_frustum_line;
GLfloat frustum_line_color[4][3] = { { 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f, 1.0f } }; // yellow, magenta

void define_frustum_line(void) {
	printf("camera[0].far_clip = %f\t camera[0].fov_y = %f\t camera[0].aspect_ratio = %f\n\n", camera[0].far_clip, camera[0].fov_y, camera[0].aspect_ratio);

	GLfloat Hnear = 2.0f * (tan(camera[0].fov_y *TO_RADIAN / 2.0f) * camera[0].near_clip);
	GLfloat Wnear = Hnear * camera[0].aspect_ratio;
	GLfloat Hfar = 2.0f * (tan(camera[0].fov_y *TO_RADIAN / 2.0f) * camera[0].far_clip);
	GLfloat Wfar = Hfar * camera[0].aspect_ratio;

	glm::vec3 ntl_1 = camera[0].near_clip * -camera[0].naxis + (camera[0].vaxis * (Hnear / 2.0f)) + (-camera[0].uaxis * (Wnear / 2.0f));
	glm::vec3 ntl_2 = camera[0].near_clip * -camera[0].naxis + (camera[0].vaxis * (Hnear / 2.0f)) + (camera[0].uaxis * (Wnear / 2.0f));
	glm::vec3 ntl_3 = camera[0].near_clip * -camera[0].naxis + (-camera[0].vaxis * (Hnear / 2.0f)) + (-camera[0].uaxis * (Wnear / 2.0f));
	glm::vec3 ntl_4 = camera[0].near_clip * -camera[0].naxis + (-camera[0].vaxis * (Hnear / 2.0f)) + (camera[0].uaxis * (Wnear / 2.0f));

	glm::vec3 ftl_1 = camera[0].far_clip * -camera[0].naxis + (camera[0].vaxis * (Hfar / 2.0f)) + (-camera[0].uaxis * (Wfar / 2.0f));     // up-left    - yellow
	glm::vec3 ftl_2 = camera[0].far_clip * -camera[0].naxis + (camera[0].vaxis * (Hfar / 2.0f)) + (camera[0].uaxis * (Wfar / 2.0f));      // up-right   - magenta
	glm::vec3 ftl_3 = camera[0].far_clip * -camera[0].naxis + (-camera[0].vaxis * (Hfar / 2.0f)) + (-camera[0].uaxis * (Wfar / 2.0f));	  // down-left  - white
	glm::vec3 ftl_4 = camera[0].far_clip * -camera[0].naxis + (-camera[0].vaxis * (Hfar / 2.0f)) + (camera[0].uaxis * (Wfar / 2.0f));     // down-right - cyan
	GLfloat vertices_frustum_line[16][3] = {
		{ 0.0f, 0.0f, 0.0f },{ ftl_1.x,ftl_1.y,ftl_1.z },
	{ 0.0f, 0.0f, 0.0f },{ ftl_2.x,ftl_2.y,ftl_2.z },
	{ 0.0f, 0.0f, 0.0f },{ ftl_3.x,ftl_3.y,ftl_3.z },
	{ 0.0f, 0.0f, 0.0f },{ ftl_4.x,ftl_4.y,ftl_4.z },
	//far plane
	{ ftl_1.x,ftl_1.y,ftl_1.z },{ ftl_3.x,ftl_3.y,ftl_3.z },{ ftl_4.x,ftl_4.y,ftl_4.z },{ ftl_2.x,ftl_2.y,ftl_2.z },
	//near plane
	{ ntl_1.x,ntl_1.y,ntl_1.z },{ ntl_3.x,ntl_3.y,ntl_3.z },{ ntl_4.x,ntl_4.y,ntl_4.z },{ ntl_2.x,ntl_2.y,ntl_2.z }
	};
	/*
	printf("tfl.x = %f\ttfl.y = %f\ttfl.z = %f\tcolor = (%f,%f,%f)\n\n", ftl_1.x, ftl_1.y, ftl_1.z, frustum_line_color[0][0], frustum_line_color[0][1], frustum_line_color[0][2]);
	printf("tfl.x = %f\ttfl.y = %f\ttfl.z = %f\tcolor = (%f,%f,%f)\n\n", ftl_2.x, ftl_2.y, ftl_2.z, frustum_line_color[1][0], frustum_line_color[1][1], frustum_line_color[1][2]);
	printf("tfl.x = %f\ttfl.y = %f\ttfl.z = %f\tcolor = (%f,%f,%f)\n\n", ftl_3.x, ftl_3.y, ftl_3.z, frustum_line_color[2][0], frustum_line_color[2][1], frustum_line_color[2][2]);
	printf("tfl.x = %f\ttfl.y = %f\ttfl.z = %f\tcolor = (%f,%f,%f)\n\n", ftl_4.x, ftl_4.y, ftl_4.z, frustum_line_color[3][0], frustum_line_color[3][1], frustum_line_color[3][2]);
	*/
	glGenBuffers(1, &VBO_frustum_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_frustum_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_frustum_line), &vertices_frustum_line[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_frustum_line);
	glBindVertexArray(VAO_frustum_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_frustum_line);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define WC_FRUSTUM_LINE_LENGTH		1.0f
void draw_frustum_line(int cam_index) {

	if (cam_index == 0) return;
	ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], camera[camera_selected].prp);
	ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], angle_rotate_u, rotate_axis_u);
	ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], angle_rotate_v, rotate_axis_v);
	ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], angle_rotate_n, rotate_axis_n);
	ModelViewMatrix[cam_index] = glm::scale(ModelViewMatrix[cam_index], glm::vec3(WC_FRUSTUM_LINE_LENGTH, WC_FRUSTUM_LINE_LENGTH, WC_FRUSTUM_LINE_LENGTH));

	ModelViewProjectionMatrix = ProjectionMatrix[cam_index] * ModelViewMatrix[cam_index];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_frustum_line);
	glUniform3fv(loc_primitive_color, 1, frustum_line_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, frustum_line_color[0]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, frustum_line_color[0]);
	glDrawArrays(GL_LINES, 4, 2);
	glUniform3fv(loc_primitive_color, 1, frustum_line_color[0]);
	glDrawArrays(GL_LINES, 6, 2);

	// far plane
	glUniform3fv(loc_primitive_color, 1, frustum_line_color[1]);
	glDrawArrays(GL_LINE_LOOP, 8, 4);
	// near plane
	glUniform3fv(loc_primitive_color, 1, frustum_line_color[1]);
	glDrawArrays(GL_LINE_LOOP, 12, 4);

	glBindVertexArray(0);
}
// */    END OF FRUSTUM LINE

void draw_main_camera_axes(int cam_index) {
	if(cam_index==0) return;
	ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], camera[camera_selected].prp);
	ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], angle_rotate_u, rotate_axis_u);
	ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], angle_rotate_v, rotate_axis_v);
	ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], angle_rotate_n, rotate_axis_n);
	ModelViewMatrix[cam_index] = glm::scale(ModelViewMatrix[cam_index], glm::vec3(-15.0f, 15.0f, 15.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[cam_index] * ModelViewMatrix[cam_index];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}


#define N_TIGER_FRAMES 12
Object tiger[N_TIGER_FRAMES];
struct {
	int cur_frame = 0;
	float rotation_angle = 0.0f;
} tiger_data;

void define_animated_tiger(void) {
	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(tiger[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);

		tiger[i].n_fields = 8;
		tiger[i].front_face_mode = GL_CW;
		prepare_geom_of_static_object(&(tiger[i]));

		tiger[i].n_geom_instances = 1;

		tiger[i].ModelMatrix[0] = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

		tiger[i].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		tiger[i].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		tiger[i].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		tiger[i].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		tiger[i].material[0].exponent = 128.0f*0.21794872f;
	}
}

typedef struct{
	float x,y,z;
}TIGER;
TIGER tiger_pos = {30.5f, 22.3f, 0.0f};

#define TIMER_LENGTH 0.4f
#define TIGER_DEFAULT_ROT 90.0f
#define SMOOTH_ROT	30
typedef struct{
	float x,y;	// 호랑이가 도달할 점과 그 점까지 갈때 사용할 회전각
	float mov_x, mov_y;		// 도달할 점까지 움직일 때 쓰이는 값
	float timer;
	int smooth;
	float rot_angle;
}TIGER_POINT;
#define NUM_POINTS 16
TIGER_POINT tiger_point[NUM_POINTS] = { { 75.5f, 22.2f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },	// 1st
								{ 89.1f, 96.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 2nd
								{ 162.0f, 115.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },		// 3rd
								{ 162.0f, 130.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },		// 4th
								{ 162.0f, 110.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },		// 5th
								{ 125.0f, 100.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },		// 6th
								{ 125.0f, 75.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 7th
								{ 115.0f, 63.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 8th

								{ 115.0f, 30.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 9th

								{ 115.0f, 50.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 10th - back to 8th
								{ 120.0f, 65.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 11th - back to 7th
								{ 120.0f, 85.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 12th
								{ 85.0f, 85.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 13th
								{ 85.0f, 18.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 14th
								{ 35.5f, 18.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 },			// 15th
								{ 27.5f, 18.3f, TIMER_LENGTH, TIMER_LENGTH, 0.0f, 0 }			// 16th - dest.
};

void draw_animated_tiger(int cam_index) {
	/*
	ModelViewMatrix[cam_index] = glm::rotate(ViewMatrix[cam_index], -tiger_data.rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix[cam_index] = glm::translate(ModelViewMatrix[cam_index], glm::vec3(100.0f, 0.0f, 0.0f));
	*/
	int i;
	float rot = 15.0f;
	tiger_point[0].rot_angle = TIGER_DEFAULT_ROT;
	tiger_point[1].rot_angle = atan((tiger_point[1].y - tiger_point[0].y) / (tiger_point[1].x - tiger_point[0].x))*TO_DEGREE;
	tiger_point[2].rot_angle = atan((tiger_point[2].y - tiger_point[1].y) / (tiger_point[2].x - tiger_point[1].x))*TO_DEGREE;
	tiger_point[3].rot_angle = TIGER_DEFAULT_ROT;
	tiger_point[4].rot_angle = -TIGER_DEFAULT_ROT;
	tiger_point[5].rot_angle = atan((tiger_point[5].x - tiger_point[4].x)/(tiger_point[5].y - tiger_point[4].y))*TO_DEGREE;
	tiger_point[6].rot_angle = -TIGER_DEFAULT_ROT;
	tiger_point[7].rot_angle = atan((tiger_point[7].x - tiger_point[6].x) / (tiger_point[7].y - tiger_point[6].y))*TO_DEGREE;
	tiger_point[8].rot_angle = -TIGER_DEFAULT_ROT;
	tiger_point[9].rot_angle = TIGER_DEFAULT_ROT * 2;
	tiger_point[10].rot_angle = atan((tiger_point[7].y - tiger_point[6].y) / (tiger_point[7].x - tiger_point[6].x))*TO_DEGREE;
	tiger_point[11].rot_angle = atan((tiger_point[11].x - tiger_point[10].x) / (tiger_point[11].y - tiger_point[10].y))*TO_DEGREE;
	tiger_point[12].rot_angle = TIGER_DEFAULT_ROT;
	tiger_point[13].rot_angle = TIGER_DEFAULT_ROT;
	tiger_point[14].rot_angle = -TIGER_DEFAULT_ROT;
	tiger_point[15].rot_angle = -TIGER_DEFAULT_ROT * 2;
	ModelViewMatrix[cam_index] = glm::rotate(ViewMatrix[cam_index], -tiger_data.rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	
	// go to 1st point
	if(tiger_point[0].timer < tiger_point[0].x-30.5f){
		tiger_pos.x = tiger_pos.x + tiger_point[0].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], tiger_point[0].rot_angle*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		tiger_point[0].timer += TIMER_LENGTH;
	}
	// go to 2nd point
	else if(tiger_point[1].timer < tiger_point[1].y-tiger_point[0].y){
		if(tiger_pos.x < tiger_point[1].x) tiger_pos.x = tiger_pos.x + tiger_point[1].mov_x*(tiger_point[1].x - tiger_point[0].x)/(tiger_point[1].y - tiger_point[0].y);
		tiger_pos.y = tiger_pos.y + tiger_point[1].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if(tiger_point[1].smooth<SMOOTH_ROT){
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[1].rot_angle*tiger_point[1].smooth/SMOOTH_ROT) *TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[1].smooth++;
		}
		else{
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index], 
				(tiger_point[0].rot_angle + tiger_point[1].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[1].timer += TIMER_LENGTH;
	}
	// go to 3rd point
	else if(tiger_point[2].timer < tiger_point[2].x-tiger_point[1].x){
		if (tiger_pos.y < tiger_point[2].y) tiger_pos.y = tiger_pos.y + tiger_point[2].mov_y*(tiger_point[2].y - tiger_point[1].y)/(tiger_point[2].x - tiger_point[1].x);
		tiger_pos.x = tiger_pos.x + tiger_point[2].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[2].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[1].rot_angle
					-(tiger_point[1].rot_angle - tiger_point[2].rot_angle)*tiger_point[2].smooth/SMOOTH_ROT)*TO_RADIAN, // 1에서 돌릴 각도 - 2에서 돌릴 각도를 시계방향으로 회전하려고 -붙임
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[2].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[2].rot_angle)*TO_RADIAN,  // if식에서 smooth/SMOOTH_ROT가 1이 되면  [1].rot_angle 항은 사라짐
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[2].timer += TIMER_LENGTH;
	}
	// go to 4th point
	else if (tiger_point[3].timer < tiger_point[3].y - tiger_point[2].y) {
		tiger_pos.y = tiger_pos.y + tiger_point[3].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[3].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[2].rot_angle 
					+ (tiger_point[3].rot_angle - tiger_point[2].rot_angle)*tiger_point[3].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[3].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[3].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[3].timer += TIMER_LENGTH;
	}
	// go to 5th point
	else if (tiger_point[4].timer < tiger_point[3].y - tiger_point[4].y) {
		tiger_pos.y = tiger_pos.y - tiger_point[4].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[4].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[3].rot_angle
					+ (tiger_point[4].rot_angle - tiger_point[3].rot_angle)*tiger_point[4].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[4].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[4].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[4].timer += TIMER_LENGTH;
	}
	// go to 6th point
	else if (tiger_point[5].timer < tiger_point[4].x - tiger_point[5].x) {
		if (tiger_pos.y > tiger_point[5].y) tiger_pos.y = tiger_pos.y - tiger_point[5].mov_y*(tiger_point[5].y - tiger_point[4].y) / (tiger_point[5].x - tiger_point[4].x);
		tiger_pos.x = tiger_pos.x - tiger_point[5].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[5].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-tiger_point[5].rot_angle*tiger_point[5].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
				
			tiger_point[5].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-tiger_point[5].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[5].timer += TIMER_LENGTH;
	}
	// go to 7th point
	else if (tiger_point[6].timer < tiger_point[5].y - tiger_point[6].y) {
		tiger_pos.y = tiger_pos.y - tiger_point[6].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[6].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-tiger_point[5].rot_angle + (tiger_point[5].rot_angle)*tiger_point[6].smooth / SMOOTH_ROT)*TO_RADIAN,  // 6th point에서 회전한만큼 다시 돌아와야함.
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[6].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[6].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[6].timer += TIMER_LENGTH;
	}
	// go to 8th point
	else if (tiger_point[7].timer < tiger_point[6].x - tiger_point[7].x) {
		if (tiger_pos.y > tiger_point[7].y) tiger_pos.y = tiger_pos.y - tiger_point[7].mov_y*((tiger_point[7].x - tiger_point[6].x)/ (tiger_point[7].y - tiger_point[6].y));
		tiger_pos.x = tiger_pos.x - tiger_point[7].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[7].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-tiger_point[7].rot_angle*tiger_point[7].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[7].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-tiger_point[7].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[7].timer += TIMER_LENGTH;
	}
	// go to 9th point
	else if (tiger_point[8].timer < tiger_point[7].y - tiger_point[8].y) {
		tiger_pos.y = tiger_pos.y - tiger_point[7].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[8].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-tiger_point[7].rot_angle + (tiger_point[7].rot_angle)*tiger_point[8].smooth / SMOOTH_ROT)*TO_RADIAN,  // 7th point에서 회전한만큼 다시 돌아와야함.
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[8].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[0].rot_angle + tiger_point[8].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[8].timer += TIMER_LENGTH;
	}
	// go to 10th point - BACK to 8th point
	else if (tiger_point[9].timer < tiger_point[9].y - tiger_point[8].y) {
		tiger_pos.y = tiger_pos.y + tiger_point[9].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[9].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-(tiger_point[9].rot_angle)*tiger_point[9].smooth / SMOOTH_ROT)*TO_RADIAN,  // 7th point에서 회전한만큼 다시 돌아와야함.
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[9].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[9].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[9].timer += TIMER_LENGTH;
	}
	// go to 11th point - BACK to 7th point
	else if (tiger_point[10].timer < tiger_point[10].y - tiger_point[9].y) {
		if (tiger_pos.x < tiger_point[10].x) tiger_pos.x = tiger_pos.x + tiger_point[10].mov_x*((tiger_point[10].x - tiger_point[9].x) / (tiger_point[10].y - tiger_point[9].y));
		tiger_pos.y = tiger_pos.y + tiger_point[10].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[10].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(180.0f - tiger_point[10].rot_angle*tiger_point[10].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[10].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(180.0f - tiger_point[10].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[10].timer += TIMER_LENGTH;
	}
	// go to 12th point
	else if (tiger_point[11].timer < tiger_point[11].y - tiger_point[10].y) {
		tiger_pos.y = tiger_pos.y + tiger_point[11].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[11].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				((180.0f - tiger_point[10].rot_angle) + tiger_point[10].rot_angle*tiger_point[11].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[11].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(180.0f)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[11].timer += TIMER_LENGTH;
	}
	// go to 13th point
	else if (tiger_point[12].timer < tiger_point[11].x - tiger_point[12].x) {
		tiger_pos.x = tiger_pos.x - tiger_point[12].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[12].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(180.0f + tiger_point[12].rot_angle*tiger_point[12].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[12].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(180.0f + tiger_point[12].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[12].timer += TIMER_LENGTH;
	}
	// go to 14th point
	else if (tiger_point[13].timer < tiger_point[12].y - tiger_point[13].y) {
		tiger_pos.y = tiger_pos.y - tiger_point[13].mov_y;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[13].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(270.0f + tiger_point[13].rot_angle*tiger_point[13].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[13].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(0.0f)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[13].timer += TIMER_LENGTH;
	}
	// go to 15th point
	else if (tiger_point[14].timer < tiger_point[13].x - tiger_point[14].x) {
		tiger_pos.x = tiger_pos.x - tiger_point[14].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[14].smooth<SMOOTH_ROT) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(tiger_point[14].rot_angle*tiger_point[14].smooth / SMOOTH_ROT)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[14].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				tiger_point[14].rot_angle*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[14].timer += TIMER_LENGTH;
	}
	// go to 16th point
	else if (tiger_point[15].timer < tiger_point[14].x - tiger_point[15].x) {
		tiger_pos.x = tiger_pos.x - tiger_point[15].mov_x;
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		if (tiger_point[15].smooth<20) {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-90.0f + tiger_point[15].rot_angle*tiger_point[15].smooth / 20)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
			tiger_point[15].smooth++;
		}
		else {
			ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],
				(-90.0f + tiger_point[15].rot_angle)*TO_RADIAN,
				glm::vec3(0.0f, 0.0f, 1.0f));
		}
		tiger_point[15].timer += TIMER_LENGTH;
	}
	// initialize positions and timer of tiger
	else{
		tiger_pos.x = 30.5f; tiger_pos.y = 22.2f; tiger_pos.z = 0.0f;
		for(i=0;i<NUM_POINTS;i++){
			tiger_point[i].timer = 0.0f;
			tiger_point[i].smooth = 0;
		}
		ModelViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], glm::vec3(tiger_pos.x, tiger_pos.y, tiger_pos.z));
		ModelViewMatrix[cam_index] = glm::rotate(ModelViewMatrix[cam_index],(-90.0f + tiger_point[15].rot_angle)*TO_RADIAN,	glm::vec3(0.0f, 0.0f, 1.0f));
	}

	ModelViewMatrix[cam_index] *= tiger[tiger_data.cur_frame].ModelMatrix[0];	// ModelMatrix[0]만 scale(0.2,0.2,0.2)가 됐으므로 여기는 ModelMatrix[cam_index]가 아니라 ModelMatrix[0]을 써줘야함.

	ModelViewProjectionMatrix = ProjectionMatrix[cam_index] * ModelViewMatrix[cam_index];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);
	//if(cam_index==0) // Only for main_camera
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
}




// DRAW CAR BODY, WHEEL, NUT
#define N_GEOMETRY_OBJECTS			3

#define GEOM_OBJ_ID_CAR_BODY 0
#define GEOM_OBJ_ID_CAR_WHEEL 1
#define GEOM_OBJ_ID_CAR_NUT 2

GLuint geom_obj_VBO[N_GEOMETRY_OBJECTS];
GLuint geom_obj_VAO[N_GEOMETRY_OBJECTS];

int geom_obj_n_triangles[N_GEOMETRY_OBJECTS];
GLfloat *geom_obj_vertices[N_GEOMETRY_OBJECTS];

// codes for the 'general' triangular-mesh object
typedef enum _GEOM_OBJ_TYPE { GEOM_OBJ_TYPE_V = 0, GEOM_OBJ_TYPE_VN, GEOM_OBJ_TYPE_VNT } GEOM_OBJ_TYPE;
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
int GEOM_OBJ_ELEMENTS_PER_VERTEX[3] = { 3, 6, 8 };

int read_geometry_file(GLfloat **object, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int i, n_triangles;
	float *flt_ptr;
	FILE *fp;

	//fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the geometry file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d", &n_triangles);
	*object = (float *)malloc(3 * n_triangles*GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float));
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	flt_ptr = *object;
	for (i = 0; i < 3 * n_triangles * GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type]; i++)
		fscanf(fp, "%f", flt_ptr++);
	fclose(fp);

	//fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);

	return n_triangles;
}

void prepare_geom_obj(int geom_obj_ID, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int n_bytes_per_vertex;

	n_bytes_per_vertex = GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float);
	geom_obj_n_triangles[geom_obj_ID] = read_geometry_file(&geom_obj_vertices[geom_obj_ID], filename, geom_obj_type);

	// Initialize vertex array object.
	glGenVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glGenBuffers(1, &geom_obj_VBO[geom_obj_ID]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_obj_VBO[geom_obj_ID]);
	glBufferData(GL_ARRAY_BUFFER, 3 * geom_obj_n_triangles[geom_obj_ID] * n_bytes_per_vertex,
		geom_obj_vertices[geom_obj_ID], GL_STATIC_DRAW);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	if (geom_obj_type >= GEOM_OBJ_TYPE_VN) {
		glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (geom_obj_type >= GEOM_OBJ_TYPE_VNT) {
		glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	free(geom_obj_vertices[geom_obj_ID]);
}

void draw_geom_obj(int geom_obj_ID) {
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * geom_obj_n_triangles[geom_obj_ID]);
	glBindVertexArray(0);
}

void free_geom_obj(int geom_obj_ID) {
	glDeleteVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glDeleteBuffers(1, &geom_obj_VBO[geom_obj_ID]);
}
// END OF DRAW CAR BODY, WHEEL, NUT


// DRAW CAR OBJECTS
#define rad 1.7f
#define ww 1.0f
void draw_wheel_and_nut(int cam_index) {
	// angle is used in Hierarchical_Car_Correct later
	int i;

	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel

	for (i = 0; i < 5; i++) {
		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));
		ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_NUT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
	}
}

void draw_car_dummy(int cam_index) {
	//ModelMatrix_CAR_BODY = glm::rotate(glm::mat4(1.0f), -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_BODY = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 4.5f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	//if (camera_type == CAMERA_DRIVER) set_ViewMatrix_for_driver();

	ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_BODY;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine
	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY); // draw body

	glLineWidth(2.0f);
	draw_axes(cam_index); // draw MC axes of body
	glLineWidth(1.0f);

	ModelMatrix_CAR_DRIVER = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_DRIVER = glm::rotate(ModelMatrix_CAR_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_DRIVER;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(5.0f);
	draw_axes(cam_index); // draw camera frame at driver seat
	glLineWidth(1.0f);

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, 4.5f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 0

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 1

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 2

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[cam_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(cam_index);  // draw wheel 3
}


// END OF DRAW CAR OBJECTS

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < n_static_objects; i++) {
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);
}