#include "include/r_local.h"


qboolean R_GenFBO(int32_t width, int32_t height, int32_t bilinear, GLenum format, fbo_t *FBO)
{
	GLuint fbo, tex, dep;
	int32_t err;
	glGetError();

	glGenFramebuffersEXT(1, &fbo);
	glGenTextures(1, &tex);
	glGenRenderbuffersEXT(1, &dep);

	GL_MBind(0,tex);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	if (bilinear)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	GL_MBind(0,0);
	err = glGetError();
	if (err != GL_NO_ERROR)
		VID_Printf(PRINT_ALL, "R_GenFBO: Texture creation: glGetError() = 0x%x\n", err);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, dep);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, width, height);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dep);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dep);
	err = glGetError();
	if (err != GL_NO_ERROR)
		VID_Printf(PRINT_ALL, "R_GenFBO: Depth buffer creation: glGetError() = 0x%x\n", err);
	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		Com_Printf("ERROR: Creation of %i x %i FBO failed!\n", width, height);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,glState.currentFBO->framebuffer);
		glDeleteTextures(1, &tex);
		glDeleteRenderbuffersEXT(1, &dep);
		glDeleteFramebuffersEXT(1, &fbo);
		return false;
	}
	else {
		int32_t err;
		err = glGetError();
		if (err != GL_NO_ERROR)
			VID_Printf(PRINT_ALL, "R_GenFBO: glGetError() = 0x%x\n", err);

		FBO->framebuffer = fbo;
		FBO->texture = tex;
		FBO->depthbuffer = dep;
		FBO->width = width;
		FBO->height = height;
		FBO->format = format;
		FBO->valid = 1;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,glState.currentFBO->framebuffer);
		return true;
	}
}

qboolean R_ResizeFBO(int32_t width, int32_t height,  int32_t bilinear, GLenum format, fbo_t *FBO)
{
	int32_t err;
	
	if (FBO == &screenFBO)
		return false;

	if (!FBO->valid)
	{
		return R_GenFBO(width, height, bilinear, format, FBO);
	}

	glGetError();

	GL_MBind(0,FBO->texture);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
	if (bilinear)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GL_MBind(0,0);
	err = glGetError();
	if (err != GL_NO_ERROR)
		VID_Printf(PRINT_ALL, "R_ResizeFBO: Texture resize: glGetError() = 0x%x\n", err);

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, FBO->depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, width, height);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	err = glGetError();
	if (err != GL_NO_ERROR)
		VID_Printf(PRINT_ALL, "R_ResizeFBO: Depth buffer resize: glGetError() = 0x%x\n", err);
	FBO->width = width;
	FBO->height = height;
	FBO->format = format;
	return true;
}

void R_SetFBOFilter(int32_t bilinear, fbo_t *FBO)
{
	if (!FBO->framebuffer)
		return;

	GL_MBind(0,FBO->texture);

	if (bilinear)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	GL_MBind(0,0);
}
void R_DelFBO(fbo_t *FBO)
{
	if (FBO->valid)
	{
		glDeleteFramebuffersEXT(1, &FBO->framebuffer);
		glDeleteTextures(1, &FBO->texture);
		glDeleteRenderbuffersEXT(1, &FBO->depthbuffer);
	}
	R_InitFBO(FBO);
}

void R_InitFBO(fbo_t *FBO)
{
	memset(FBO,0,sizeof(fbo_t));
}

void R_BindFBO(fbo_t *FBO)
{
	GL_BindFBO(FBO);
	glViewport(0, 0, FBO->width, FBO->height);
	vid.width = FBO->width;
	vid.height = FBO->height;
}

void R_Clear();
void R_ClearFBO(fbo_t *FBO)
{
	fbo_t *current = glState.currentFBO;

	// use GL_BindFBO to avoid trampling on current viewport
	GL_BindFBO(FBO);
	R_Clear();
	GL_BindFBO(current);
}