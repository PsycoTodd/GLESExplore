//
// Created by XING on 4/8/2017.
//

#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include "Teapot.h"
#include "myShader.h"

#include <myJNIHelper.h>


/**
 * Try to initialize our graphics resoruce and gl resources here.
 */
Teapot::Teapot() {

    InitShaders();

    setVerts();
    setTexCoords();
    setNorms();
    setIndices();

    LoadTexture();

    CheckGLError("Teaport::Init");
}

/**
 * Clean up the graphics resources.
 */
Teapot::~Teapot()
{

}

/**
 * Compile the opengl shader code and binding the CPU/GPU variables.
 */
void
Teapot::InitShaders(){
    // shader related setup -- loading, attribute and uniform locations
    std::string vertexShader    = "shaders/modelTextured.vsh";
    std::string fragmentShader  = "shaders/modelTextured.fsh";
    _shaderProgramID         = LoadShaders(vertexShader, fragmentShader);
    _vertexAttribute         = GetAttributeLocation(_shaderProgramID, "vertexPosition");
    _vertexUVAttribute       = GetAttributeLocation(_shaderProgramID, "vertexUV");
    _mvpLocation             = GetUniformLocation(_shaderProgramID, "mvpMat");
    _textureSamplerLocation  = GetUniformLocation(_shaderProgramID, "textureSampler");
}


/**
 * Load the teapot texture resource from the package.
 */
void
Teapot::LoadTexture() {
    std::string assetPath = "shaders/TextureTeapotBrass.png";
    std::string texFilePath;
    gHelperObject->ExtractAssetReturnFilename(assetPath.c_str(), texFilePath);
    // Since we have openCV we use it to load image.
    MyLOGI("Loading texture %s", texFilePath.c_str());
    cv::Mat texMat = cv::imread(texFilePath);
    if(!texMat.empty())
    {
        // Convert BGR to RGB.
        cv::cvtColor(texMat, texMat, CV_BGR2RGB);
        cv::flip(texMat, texMat, 0);
        // Bind the texture.
        GLuint * textureGLNames = new GLuint[1];
        glGenTextures(1, textureGLNames);
        glBindTexture(GL_TEXTURE_2D, textureGLNames[0]);
        _textureID = textureGLNames[0];
        // Set the linear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // load the OpenCV Mat into GLES
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texMat.cols,
                     texMat.rows, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     texMat.data);
        CheckGLError("Teapot::loadGLTexGen");
        delete[] textureGLNames;
        return;
    }
    else{
        MyLOGE("Cannot load the texture %s", texFilePath.c_str());
    }




}
void
Teapot::Render(glm::mat4 *MVP)
{
    // Draw the teaport with texture.

}



void
Teapot::setVerts()
{
    _verticesNumber = sizeof(TEAPOT_VERTS) / (sizeof(float) * 3);

    // Now we try to generate OpenGL buffer.
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TEAPOT_VERTS), &TEAPOT_VERTS[0], GL_STATIC_DRAW);
    _vBuffer = buffer;

}


void
Teapot::setTexCoords()
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) *  2 * _verticesNumber, &TEAPOT_TEX_COORDS[0], GL_STATIC_DRAW);
    _uvBuffer = buffer;
}


void
Teapot::setNorms()
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) *  2 * _verticesNumber, &TEAPOT_TEX_COORDS[0], GL_STATIC_DRAW);
    _uvBuffer = buffer;
}


void
Teapot::setIndices()
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * _verticesNumber * 3, &TEAPOT_INDICES[0],
                 GL_STATIC_DRAW);
   _indBuffer = buffer;
}



