#include "GLPlatform.h"

#include <Debug.h>
#include <GL\glew.h>
#include <SDL_opengl.h>
#include <GL\wglew.h>
#include <gl\glu.h>

#include "SpriteBatchShader.h"



static GLuint vertexBuffer, vao;
static SpriteBatchShader* shader = nullptr;

extern GPU_Target* screen;
extern SDL_Window* window;;

static void CheckForErrors() {

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		// Process/log the error.
		EXCEPTION("OpenGL error %#10x\n", err);
	}
}




void GLPlatform::Init()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		EXCEPTION("Failed to initialize GLEW!");
	}

	glGenVertexArrays(1, &vao);
	CheckForErrors();

	glGenBuffers(1, &vertexBuffer);
	CheckForErrors();

	shader = new SpriteBatchShader();
}

void GLPlatform::PrepareDraw()
{
	GPU_FlushBlitBuffer();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glBindVertexArray(vao);
	CheckForErrors();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	CheckForErrors();

	// Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// Uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
	// Color
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
	CheckForErrors();

	glEnableVertexAttribArray(0);
	CheckForErrors();

	glUseProgram(shader->program);
	CheckForErrors();
	// Uv
	//glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	// Color
	//glVertexAttribPointer(2, 1, GL_INT, false, 0, 0);


}

void GLPlatform::UpdateBuffer(const Span<Vertex> vertexes)
{
	glBufferData(GL_ARRAY_BUFFER, vertexes.Size() * sizeof(Vertex), vertexes.Data(), GL_STREAM_DRAW);
	CheckForErrors();
}

void GLPlatform::DrawTriangles(unsigned start, unsigned count)
{
	glDrawArrays(GL_TRIANGLES, start, count * 3);
	CheckForErrors();

}

void GLPlatform::DrawLines(unsigned start, unsigned count)
{
	glDrawArrays(GL_LINES, start, count * 2);
	CheckForErrors();
}
