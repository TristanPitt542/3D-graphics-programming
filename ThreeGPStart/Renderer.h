#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

struct MyMesh
{
	GLuint m_VAO{ 0 };
	GLuint c_VAO{ 0 };

	GLuint m_numElements{ 0 };
	GLuint c_numElements{ 0 };

	GLuint tex{ 0 };
};

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	//vectors
	std::vector<MyMesh> m_meshes;
	std::vector<MyMesh> c_meshes;
	std::vector<MyMesh> m_sky;

	bool m_wireframe{ false };
	bool m_cullFace{ true };	

public:
	Renderer();
	~Renderer();

	float Noise(int x, int y);
	
	bool Cube();
	bool Skybox();
	bool Jeep();
	bool Terrain();
	
	// Draw GUI
	void DefineGUI();

	// Create the program. This is the compiled shaders.
	bool CreateProgram();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

