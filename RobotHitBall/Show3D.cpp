
#include"Show3D.h"

using namespace glm;

const int cps = 1;//21;

GLFWwindow* window;

double *Getpos(int Posnum)
{
	if (Drowlist[0][0] == 1)
	{
		Drowlist[0][0] = 0;
		Posnum = 1;
	}
	return Drowlist[Posnum];
}


DWORD WINAPI Drow3D(LPVOID lpParamter)    //Ïß³Ì
{

	double* P;
	double vx, vy, vz, x, y, z, tpx = 0.0, tpy = 0.0, tpz = 0.0;
	float angle;
	int Drownum;
	// Initialise GLFW

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(800, 600, "Show3D", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 800 / 2, 600 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	GLuint programID1 = LoadShaders("StandardShading1.vertexshader", "StandardShading1.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
	GLuint ViewMatrixID1 = glGetUniformLocation(programID1, "V");
	GLuint ModelMatrixID1 = glGetUniformLocation(programID1, "M");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID1 = glGetAttribLocation(programID1, "vertexPosition_modelspace");
	GLuint vertexUVID1 = glGetAttribLocation(programID1, "vertexUV");
	GLuint vertexNormal_modelspaceID1 = glGetAttribLocation(programID1, "vertexNormal_modelspace");


	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform normals
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Load the texture
	GLuint Texture1 = loadDDS("uvmap1.DDS");

	// Get a handle for our "myTextureSampler" uniform normals
	GLuint TextureID1 = glGetUniformLocation(programID1, "myTextureSampler");


	// Read our .obj file
	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec2> uvs1;
	std::vector<glm::vec3> normals1;
	bool res1 = loadOBJ("yu.obj", vertices1, uvs1, normals1);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("chang.obj", vertices, uvs, normals);


	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);



	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);

	GLuint uvbuffer1;
	glGenBuffers(1, &uvbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
	glBufferData(GL_ARRAY_BUFFER, uvs1.size() * sizeof(glm::vec2), &uvs1[0], GL_STATIC_DRAW);

	GLuint normalbuffer1;
	glGenBuffers(1, &normalbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
	glBufferData(GL_ARRAY_BUFFER, normals1.size() * sizeof(glm::vec3), &normals1[0], GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	glUseProgram(programID1);

	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	//GLuint LightID1 = glGetUniformLocation(programID1, "LightPosition_worldspace");

	//Posnum = 0;

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// Use our shader
		glUseProgram(programID);


		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		glm::mat4 myScalingMatrix = glm::scale(glm::vec3(0.0254, 0.0254, 0.0254));

		glm::mat4 ModelMatrix = glm::mat4(1.0);


		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix * myScalingMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(0, 700, 0);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// 2nd attribute buffer : uvs
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,    // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
			);

		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

		Drownum = 1;
		glm::mat4 myRotatex;
		glm::mat4 myRotatey;
		glm::mat4 myRotatez;

		while (Drownum < Drowlist[0][1])
		{

			// Use our shader
			glUseProgram(programID1);


			Drownum += cps;
			//if (Posnum > Poslist[0][1]) Posnum = 0;
			P = Getpos(Drownum);


			// Compute the MVP matrix from keyboard and mouse input
			computeMatricesFromInputs();
			ProjectionMatrix = getProjectionMatrix();
			ViewMatrix = getViewMatrix();

			myScalingMatrix = glm::scale(glm::vec3(0.02, 0.02, 0.02));

			ModelMatrix = glm::mat4(1.0);

			//cout << P[0] << " " << P[1] << " " << P[2] << endl;

			glm::mat4 myMatrix = glm::translate(glm::vec3(P[0], P[1], P[2]));
			//glm::mat4 myMatrix = glm::translate(glm::vec3( 0, 0 ,0 ));

			if (P[0] - tpx != 0)
			{
				glm::vec3 myRotationAxisx(0, -1, 0);
				myRotatex = glm::rotate(float(atan((P[2] - tpz) / (P[0] - tpx)) / pi * 180), myRotationAxisx);

				glm::vec3 myRotationAxisy(-1, 0, 0);
				myRotatey = glm::rotate(float(atan((P[1] - tpy) / (P[0] - tpx)) / pi * 180), myRotationAxisy);
			}

			glm::vec3 myRotationAxisz(0, 1, 0);
			myRotatez = glm::rotate(float(90), myRotationAxisz);

			tpx = P[0];
			tpy = P[1];
			tpz = P[2];


			MVP = ProjectionMatrix * ViewMatrix * myMatrix * myRotatez  * myRotatex * myRotatey * myScalingMatrix * ModelMatrix;

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID1, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID1, 1, GL_FALSE, &ViewMatrix[0][0]);

			lightPos = glm::vec3(0, 0, 100);
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture1);
			// Set our "myTextureSampler" sampler to user Texture Unit 0
			glUniform1i(TextureID1, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(vertexPosition_modelspaceID1);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
			glVertexAttribPointer(
				vertexPosition_modelspaceID1,  // The attribute we want to configure
				3,                            // size
				GL_FLOAT,                     // type
				GL_FALSE,                     // normalized?
				0,                            // stride
				(void*)0                      // array buffer offset
				);

			// 2nd attribute buffer : uvs
			glEnableVertexAttribArray(vertexUVID1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
			glVertexAttribPointer(
				vertexUVID1,                   // The attribute we want to configure
				2,                            // size : U+V => 2
				GL_FLOAT,                     // type
				GL_FALSE,                     // normalized?
				0,                            // stride
				(void*)0                      // array buffer offset
				);

			// 3rd attribute buffer : normals
			glEnableVertexAttribArray(vertexNormal_modelspaceID1);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
			glVertexAttribPointer(
				vertexNormal_modelspaceID1,    // The attribute we want to configure
				3,                            // size
				GL_FLOAT,                     // type
				GL_FALSE,                     // normalized?
				0,                            // stride
				(void*)0                      // array buffer offset
				);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, vertices1.size());

			glDisableVertexAttribArray(vertexPosition_modelspaceID1);
			glDisableVertexAttribArray(vertexUVID1);
			glDisableVertexAttribArray(vertexNormal_modelspaceID1);

		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			Drowlist[0][1] = 0;
		}

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);

	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteBuffers(1, &uvbuffer1);
	glDeleteBuffers(1, &normalbuffer1);
	glDeleteProgram(programID1);
	glDeleteTextures(1, &Texture1);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

