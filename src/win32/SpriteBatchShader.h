#pragma once


#include <SDL_gpu.h>
#include "Debug.h"


static const char* vsCode =
R"(  
#version 330 core
layout (location = 0) in vec2 pos;
void main(){
	gl_Position.xy = pos;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
})";


static const char* fsCode =
R"(  
#version 330 core

out vec4 color;

void main(){
	color = vec4(1,0,0,1);
}
)";



class SpriteBatchShader {

public:
	Uint32 program;
	int posLoc;
	SpriteBatchShader() {
		Uint32 vs = GPU_CompileShader(GPU_VERTEX_SHADER, vsCode);
		if (!vs) {
			const char* err = GPU_GetShaderMessage();
			EXCEPTION("Failed to compile vertex shader: %s", err);
		}
		Uint32 fs = GPU_CompileShader(GPU_FRAGMENT_SHADER, fsCode);
		if (!fs) {
			const char* err = GPU_GetShaderMessage();
			EXCEPTION("Failed to compile fragment shader: %s", err);
		}


		program = GPU_LinkShaders(vs, fs);

		if (!program) {
			const char* err = GPU_GetShaderMessage();
			EXCEPTION("Failed to compile SpriteBatch shader: %s", err);
		}

		posLoc =GPU_GetAttributeLocation(program, "pos");
	}

	void Bind() {
		GPU_ShaderBlock block;
		GPU_ActivateShaderProgram(program, &block);
	}
	void Unbind() {
		GPU_DeactivateShaderProgram();
	}
};
