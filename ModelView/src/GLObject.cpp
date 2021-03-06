//
//  GLObject.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLObject.h"
#include "GLApplication.h"
bool is_exit = false;
void GLObject::OpenGLShouldHaveNoError(const char* who){
    // TODO: we should check whether GLApplication exists
    GLenum error = glGetError();
	static bool firstCall = true;
    if(error){
		if (error == 1280 && firstCall){
			firstCall = false;
			return;
		}
		if (error == 1282 && is_exit){
			return;
		}
        Error(who, "Fatal OpenGL Error %d:%s", error, glGetString(error));
    }
}

void GLObject::Log(const char* what, const char* who, const char* how, ...){
    va_list ap;
    
    printf("[%s][%s]: ", what, who);
    va_start(ap, how);
    vprintf(how, ap);
    printf("\n");
    va_end(ap);
}

void GLObject::Info(const char* who, const char* how, ...){
    va_list ap;
    
    printf("[INFO][%s]: ", who);
    va_start(ap, how);
    vprintf(how, ap);
    printf("\n");
    va_end(ap);
}

void GLObject::Warning(const char* who, const char* how, ...){
    va_list ap;
    
    printf("[WARNING][%s]: ", who);
    va_start(ap, how);
    vprintf(how, ap);
    printf("\n");
    va_end(ap);
}

void GLObject::Error(const char* who, const char* how, ...){
    va_list ap;
    
    printf("[ERROR][%s]: ", who);
    va_start(ap, how);
    vprintf(how, ap);
    printf("\n");
    va_end(ap);
    // FIXME: should we do more than just exit?
    exit(-1);
}