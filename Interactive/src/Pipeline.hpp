// 3D Graphics and Animation - Pipeline Template
// Create a program and attach shaders.

#pragma once


#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
//#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
//#include <vector>


 struct Pipe{
	public:
		GLuint program;										// Pipeline reference
		string vsName = ""s;								// name of file for vertex shader
		string fsName = ""s;								// name of file for fragment shader
		bool error = false;									// Flag any errors.
	private:

 };

class Pipeline{
	public:
		Pipeline();
		Pipe* currentPipe;

		// Modification made: the pipeline can have several pipes!
		vector<Pipe*> pipes;											// Save all information.

		void UsePipe(Pipe* pipe);
		// Returns a new pipe.
		// Optional argument creates a new pipe struct
		Pipe LoadShaders(string vs, string fs, Pipe pipe = Pipe());				// Load shaders using files		
		void ReloadShaders();								// Reload Shaders for debugging...			

		virtual ~Pipeline();
	protected:

	private:
		string ReadShader(string name, Pipe* pipe);
		void CheckErrorShader(GLuint shader, string name, Pipe* pipe);
		void CheckErrorLinking(Pipe* pipe);
};