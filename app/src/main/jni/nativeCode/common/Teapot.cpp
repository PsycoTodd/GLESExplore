//
// Created by XING on 4/8/2017.
//

#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include "Teapot.h"
#include "myShader.h"

#include <myJNIHelper.h>
#include <string>


/**
 * Try to initialize our graphics resoruce and gl resources here.
 */
Teapot::Teapot() {
    _loadSuccess = false;
    InitShaders();

    setVerts();
    setTexCoords();
    setNorms();
    setIndices();

    LoadTexture();

    CheckGLError("Teaport::Init");
    _loadSuccess = true;
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

/*template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}
 */

void
Teapot::Render(glm::mat4 *mvpMat)
{
    // Draw the teaport with texture.
    if(_loadSuccess == false)
    {
        return;
    }

    // Make sure we use the correct vertex and fragment shader.
    glUseProgram(_shaderProgramID);
    //Set our shader variables.
    glUniformMatrix4fv(_mvpLocation, 1, GL_FALSE, (const GLfloat*) mvpMat);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_textureSamplerLocation, 0);
    CheckGLError("Teapot: setTextureResource()");

    // Now we start to binding our resources.
    // 1. Binding our texture.
    glBindTexture(GL_TEXTURE_2D, _textureID);
    CheckGLError("Teapot: 1");
    // 2. Indices buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indBuffer);
    CheckGLError("Teapot: 2");
    // 3. Vertices buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vBuffer);
    glEnableVertexAttribArray(_vertexAttribute);
    glVertexAttribPointer(_vertexAttribute, 3, GL_FLOAT, 0, 0, 0);
    CheckGLError("Teapot: 3");
    // 3.5 ignore normal first.
    // 4. UV
    glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
    glEnableVertexAttribArray(_vertexUVAttribute);
    glVertexAttribPointer(_vertexUVAttribute, 2, GL_FLOAT, 0, 0, 0);
    CheckGLError("Teapot: 4");

    glDrawElements(GL_TRIANGLES, _verticesNumber, GL_UNSIGNED_INT, 0);

    // Release the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



}



void
Teapot::setVerts()
{
    _verticesNumber = TEAPOT_VERTS.size() / 3;

    // Now we try to generate OpenGL buffer.
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _verticesNumber * 3, &TEAPOT_VERTS[0], GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) *  3 * _verticesNumber, &TEAPOT_TEX_COORDS[0], GL_STATIC_DRAW);
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



