#include "Renderer.h"
#include "Camera.h"
#include "Helper.h"
#include "ImageLoader.h"

// Globals
GLuint gProgram;
GLuint gVAO;

Renderer::Renderer() 
{
}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	MyMesh newMesh;
	glDeleteProgram(m_program);
	
	glDeleteProgram(newMesh.m_VAO);
}

// Noise
float Renderer::Noise(int x, int y)
{
	int n = x + y * 46;
	n = (n >> 13) ^ n;
	int nn = (n* (n * n * 60493 + 199990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741824.0f);
}

//skybox
bool Renderer::Skybox()
{
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\Sky\\Clouds\\skybox.x"))
		return false;

	// Now we can loop through all the mesh in the loaded model:
	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		MyMesh newMesh;

		//positions
		GLuint positionsVBO;
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//normals
		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//elements
		GLuint elementsEBO;
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//uvcoords
		GLuint uvcoordsVBO;
		glGenBuffers(1, &uvcoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvcoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		newMesh.m_numElements = mesh.elements.size();


		//VAO
		glGenVertexArrays(1, &newMesh.m_VAO);
		glBindVertexArray(newMesh.m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, uvcoordsVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBindVertexArray(0);


		std::string fname{ loader.GetMaterialVector()[mesh.materialIndex].diffuseTextureFilename };

		//texture
		Helpers::ImageLoader imageLoader;
		if (!imageLoader.Load("Data/Models/Sky/Clouds/" + fname))
			return false;

		glGenTextures(1, &newMesh.tex);
		glBindTexture(GL_TEXTURE_2D, newMesh.tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageLoader.Width(), imageLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		m_sky.push_back(newMesh);
	}
	return true;
}

//Cube
bool Renderer::Cube()
{

	MyMesh newMesh;

	glm::vec3 minValues{ -10, -10, 10 };
	glm::vec3 maxValues{ 10, 10, -10 };

	//Add verts and index
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> colours;
	std::vector<GLuint> elements;

	glm::vec3 corners[8] =
	{
		{minValues.x, minValues.y, minValues.z}, //0
		{maxValues.x, minValues.y, minValues.z}, //1
		{maxValues.x, maxValues.y, minValues.z}, //2
		{minValues.x, maxValues.y, minValues.z}, //3

		{maxValues.x, minValues.y, maxValues.z}, //4
		{minValues.x, minValues.y, maxValues.z}, //5
		{minValues.x, maxValues.y, maxValues.z}, //6
		{maxValues.x, maxValues.y, maxValues.z}, //7
	};

	GLuint offset{ 0 };

	//Front
	verts.push_back(corners[0]); verts.push_back(corners[1]);  verts.push_back(corners[2]);  verts.push_back(corners[3]);
	colours.push_back({ 0,1,0 }); colours.push_back({ 0,1,0 }); colours.push_back({ 0,1,0 }); colours.push_back({ 0,1,0 });

	elements.push_back(offset + 0);	elements.push_back(offset + 1);	elements.push_back(offset + 3);
	elements.push_back(offset + 1);	elements.push_back(offset + 2);	elements.push_back(offset + 3);

	//Right
	verts.push_back(corners[1]); verts.push_back(corners[4]);  verts.push_back(corners[7]);  verts.push_back(corners[2]);
	colours.push_back({ 1,0,0 }); colours.push_back({ 1,0,0 });	colours.push_back({ 1,0,0 }); colours.push_back({ 1,0,0 });

	offset += 4;

	elements.push_back(offset + 0);	elements.push_back(offset + 1);	elements.push_back(offset + 3);
	elements.push_back(offset + 1);	elements.push_back(offset + 2);	elements.push_back(offset + 3);

	// Back
	verts.push_back(corners[4]); verts.push_back(corners[5]);  verts.push_back(corners[6]);  verts.push_back(corners[7]);
	colours.push_back({ 0,0,1 }); colours.push_back({ 0,0,1 }); colours.push_back({ 0,0,1 }); colours.push_back({ 0,0,1 });

	offset += 4;

	elements.push_back(offset + 0); elements.push_back(offset + 1); elements.push_back(offset + 3);
	elements.push_back(offset + 1); elements.push_back(offset + 2); elements.push_back(offset + 3);

	// Left
	verts.push_back(corners[5]); verts.push_back(corners[0]);  verts.push_back(corners[3]);  verts.push_back(corners[6]);
	colours.push_back({ 1,0,1 }); colours.push_back({ 1,0,1 }); colours.push_back({ 1,0,1 }); colours.push_back({ 1,0,1 });

	offset += 4;

	elements.push_back(offset + 0); elements.push_back(offset + 1); elements.push_back(offset + 3);
	elements.push_back(offset + 1); elements.push_back(offset + 2); elements.push_back(offset + 3);

	// Top
	verts.push_back(corners[7]); verts.push_back(corners[6]);  verts.push_back(corners[3]);  verts.push_back(corners[2]);
	colours.push_back({ 1,1,0 }); colours.push_back({ 1,1,0 }); colours.push_back({ 1,1,0 }); colours.push_back({ 1,1,0 });

	offset += 4;

	elements.push_back(offset + 0); elements.push_back(offset + 1); elements.push_back(offset + 3);
	elements.push_back(offset + 1); elements.push_back(offset + 2); elements.push_back(offset + 3);

	// Bottom
	verts.push_back(corners[0]); verts.push_back(corners[5]);  verts.push_back(corners[4]);  verts.push_back(corners[1]);
	colours.push_back({ 1,1,1 }); colours.push_back({ 1,1,1 }); colours.push_back({ 1,1,1 }); colours.push_back({ 1,1,1 });

	offset += 4;

	elements.push_back(offset + 0); elements.push_back(offset + 1); elements.push_back(offset + 3);
	elements.push_back(offset + 1); elements.push_back(offset + 2); elements.push_back(offset + 3);

	//positions
	GLuint positionVB0;
	glGenBuffers(1, &positionVB0);
	glBindBuffer(GL_ARRAY_BUFFER, positionVB0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//colors
	GLuint colorVB0;
	glGenBuffers(1, &colorVB0);
	glBindBuffer(GL_ARRAY_BUFFER, colorVB0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colours.size(), colours.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint elementsEBO;
	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	newMesh.c_numElements = elements.size();

	//VAO
	glGenVertexArrays(1, &newMesh.c_VAO);
	glBindVertexArray(newMesh.c_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, positionVB0);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);



	glBindBuffer(GL_ARRAY_BUFFER, colorVB0);

	glEnableVertexAttribArray(3);

	glVertexAttribPointer(
		3,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBindVertexArray(0);

	c_meshes.push_back(newMesh);

	return true;
}



//Jeep
bool Renderer::Jeep()
{
	Helpers::ModelLoader Loader;
	if (!Loader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
		return false;

	// Now we can loop through all the jeep_mesh in the loaded model:
	for (Helpers::Mesh mesh : Loader.GetMeshVector())
	{
		for (size_t i = 0; i < mesh.vertices.size(); i++)
		{
			mesh.vertices[i] = mesh.vertices[i] + glm::vec3(-500,-200,0);
		}

		MyMesh newMesh;

		//positions
		GLuint positionsVBO;
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//normals
		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//elements
		GLuint elementsEBO;
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//uvcoords
		GLuint uvcoordsVBO;
		glGenBuffers(1, &uvcoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvcoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		//clear binding
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//VAO
		glGenVertexArrays(1, &newMesh.m_VAO);
		glBindVertexArray(newMesh.m_VAO);

		//elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, uvcoordsVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);

		glBindVertexArray(0);

		newMesh.m_numElements = (GLuint)mesh.elements.size();


		//texture
		Helpers::ImageLoader imageLoader;
		if (!imageLoader.Load("Data\\Models\\Jeep\\jeep_army.jpg"))
			return false;

		glGenTextures(1, &newMesh.tex);
		glBindTexture(GL_TEXTURE_2D, newMesh.tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageLoader.Width(), imageLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		m_meshes.push_back(newMesh);
	}

	return true;
}

//Terrain
bool Renderer::Terrain()
{
	MyMesh newMesh;

	//vectors
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> colours;
	std::vector<glm::vec2> texcoords;
	std::vector<GLuint> elements;

	std::vector<glm::vec3> vertsMovedUp;
	std::vector<glm::vec3> normals;

	//number of verts
	int numberOfVertsInX = 50;
	int numberOfVertsInZ = 50;

	//total cells
	int numCellX = numberOfVertsInX - 1;
	int numCellZ = numberOfVertsInZ - 1;
	int totalCells = numCellX * numCellZ;

	//toggle for diamond shape
	bool toggleForDiamondPattern = true;

	// terrain verts + col
	for (int cellZ = 0; cellZ < numberOfVertsInZ; cellZ++)
	{
		for (int cellX = 0; cellX < numberOfVertsInX; cellX++)
		{
			//add verts to vector & increase frequency of texture
			verts.push_back(glm::vec3(cellX * 300 - 6000, -500, cellZ * 300 - 6000));

			float xTex = cellZ / (numberOfVertsInX - 1.f) * 100;
			float yTex = cellX / (numberOfVertsInZ - 1.f) * 100;

			texcoords.push_back(glm::vec2(xTex, yTex));
		}
	}

	//terrain elements
	for (int cellZ = 0; cellZ < numCellZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellX; cellX++)
		{
			int startVertIndex = cellZ * numberOfVertsInX + cellX;


			if (toggleForDiamondPattern)
			{
				//triangles1
				elements.push_back(startVertIndex + numberOfVertsInX);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex);

				//triangles2
				elements.push_back(startVertIndex + numberOfVertsInX);
				elements.push_back(startVertIndex + numberOfVertsInX + 1);
				elements.push_back(startVertIndex + 1);
			}
			else
			{
				//triangles1
				elements.push_back(startVertIndex + numberOfVertsInX + 1);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex);
		
				//triangles2
				elements.push_back(startVertIndex + numberOfVertsInX);
				elements.push_back(startVertIndex + numberOfVertsInX + 1);
				elements.push_back(startVertIndex);
			}
			toggleForDiamondPattern = !toggleForDiamondPattern;
		}
	}

	//move up verts
	vertsMovedUp = verts;

	//verts to move up
	for (int i = 0; i < vertsMovedUp.size(); i++)
		vertsMovedUp[i].y = vertsMovedUp[i].x + 10;

	int index = 0;

	//bools for noise
	bool noiseOn = true;
	bool makeExt = true;

	//noise
	if (noiseOn)
	{
		for (int i = 0; i < numberOfVertsInZ; i++)
		{
			for (int j = 0; j < numberOfVertsInX; j++)
			{
				float noiseValue = Noise(i, j);
				noiseValue = noiseValue + 1.00001 / 2;
				glm::vec3 myvec = verts[index];

				if (makeExt)
				{
					noiseValue = noiseValue * 300;
				}

				myvec.y = myvec.y + noiseValue;

				verts[index] = myvec;
				index++;

			}
		}

	}

	// Calculate normals
	for (int cellZ = 0; cellZ < numberOfVertsInZ; cellZ++)
	{
		for (int cellX = 0; cellX < numberOfVertsInX; cellX++)
		{
			int currentVertIndex = cellZ * numberOfVertsInX + cellX;
			glm::vec3 normal(0.0f);

			if (cellX < numCellX && cellZ < numCellZ)
			{
				// Calculate normal for the current vertex
				// Triangle1
				if (cellX > 0 && cellZ > 0)
				{
					glm::vec3 v1 = verts[currentVertIndex - numberOfVertsInX] - verts[currentVertIndex];
					glm::vec3 v2 = verts[currentVertIndex - 1] - verts[currentVertIndex];
					normal += glm::cross(v1, v2);
				}

				// Triangle2
				if (cellX < numCellX - 1 && cellZ < numCellZ - 1)
				{
					glm::vec3 v1 = verts[currentVertIndex + 1] - verts[currentVertIndex];
					glm::vec3 v2 = verts[currentVertIndex + numberOfVertsInX] - verts[currentVertIndex];
					normal += glm::cross(v1, v2);
				}
			}

			// Normalize the normal vector
			normal = glm::normalize(normal);

			// Assign the calculated normal to the normals vector
			normals.push_back(normal);
		}
	}

	newMesh.m_numElements = (GLuint)elements.size();

	//positions
	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
	//clear binding
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//colours
	GLuint coloursVBO;
	glGenBuffers(1, &coloursVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colours.size(), colours.data(), GL_STATIC_DRAW);
	//clear binding
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normals
	GLuint normalsVBO;
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	//clear binding
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texcoords
	GLuint texcoordsVBO;
	glGenBuffers(1, &texcoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);
	//clear binding
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//elements
	GLuint elementsEBO;
	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
	//clear binding
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//VAO
	glGenVertexArrays(1, &newMesh.m_VAO);
	glBindVertexArray(newMesh.m_VAO);


	//positions
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);

	glBindVertexArray(0);

	//texture
	Helpers::ImageLoader imageLoader;
	if (!imageLoader.Load("Data\\Textures\\grass_green-01_df_.dds"))
		return false;

	glGenTextures(1, &newMesh.tex);
	glBindTexture(GL_TEXTURE_2D, newMesh.tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageLoader.Width(), imageLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	m_meshes.push_back(newMesh);

	return true;
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Checkbox("Cull Face", &m_cullFace);


	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	if (!m_program)
		glDeleteProgram(m_program);

	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.vert") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.frag") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return true;
}


// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	Skybox();
	Cube();
	Terrain();
	Jeep();


	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);

	if (m_cullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, .1f, 400000.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader

	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));


	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);

	glm::mat4 model_xform(1);
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	//rotate
	{
		static float angle = 0;
		static bool rotateY = true;

		if (rotateY) // Rotate around y axis		
			model_xform = glm::rotate(model_xform, angle, glm::vec3{ 0 ,1,0 });
		else // Rotate around x axis		
			model_xform = glm::rotate(model_xform, angle, glm::vec3{ 1 ,0,0 });

		angle += 0.001f;
		if (angle > glm::two_pi<float>())
		{
			angle = 0;
			rotateY = !rotateY;
		}
	}

	glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);











	// Disable lighting for skybox
	glUniform1i(glGetUniformLocation(m_program, "useLighting"), 0);

	// Enable for skybox
	glUniform1i(glGetUniformLocation(m_program, "useTexture"), 1);
	//skybox
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	for (MyMesh& mesh : m_sky)
	{

		glm::mat4 combined_xform = glm::mat4(projection_xform * view_xform2);

		GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.tex);

		// Bind our VAO and render
		glBindVertexArray(mesh.m_VAO);
		glDrawElements(GL_TRIANGLES, mesh.m_numElements, GL_UNSIGNED_INT, (void*)0);
	}

	// Disable texture for cube
	glUniform1i(glGetUniformLocation(m_program, "useTexture"), 0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	for (MyMesh& mesh : c_meshes)
	{

		glm::mat4 combined_xform = projection_xform * view_xform;

		GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));


		// Bind our VAO and render
		glBindVertexArray(mesh.c_VAO);
		glDrawElements(GL_TRIANGLES, mesh.c_numElements, GL_UNSIGNED_INT, (void*)0);
	}

	// Enable texture for meshes
	glUniform1i(glGetUniformLocation(m_program, "useTexture"), 1);

	// Enable lighting for meshes
	glUniform1i(glGetUniformLocation(m_program, "useLighting"), 1);

	//jeep & terrain
	for (MyMesh& mesh : m_meshes)
	{

		glm::mat4 combined_xform = projection_xform * view_xform;

		GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.tex);

		// Bind our VAO and render
		glBindVertexArray(mesh.m_VAO);
		glDrawElements(GL_TRIANGLES, mesh.m_numElements, GL_UNSIGNED_INT, (void*)0);
	}

	// Reset to default (texture enabled)
	glUniform1i(glGetUniformLocation(m_program, "useTexture"), 1);

	// Reset to default (lighting enabled)
	glUniform1i(glGetUniformLocation(m_program, "useLighting"), 1);
}