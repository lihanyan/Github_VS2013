#pragma once

#include "Parameter.h"

// Include GLEW
#include <GL/glew.h>



// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

using namespace glm;




double *Getpos(int Posnum);

DWORD WINAPI Drow3D(LPVOID lpParamter);