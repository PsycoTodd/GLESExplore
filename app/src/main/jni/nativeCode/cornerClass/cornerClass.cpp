/*
 *    Copyright 2016 Anand Muralidhar
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "myShader.h"
#include "cornerClass.h"


#include "assimp/Importer.hpp"
#include <opencv2/opencv.hpp>
#include <myJNIHelper.h>

/**
 * Class constructor
 */
CornerClass::CornerClass() {

    MyLOGD("CornerClass::CornerClass");
    initsDone = false;
    back = NULL;
    cornerDetector = cv::ORB::create(); // choosing ORB detector with default parameters

}

CornerClass::~CornerClass() {

    MyLOGD("CornerClass::CornerClass");
    if(back) {
        delete back;
        back = NULL;
    }
}

void CornerClass::SetCameraParams(int cameraPreviewWidth, int cameraPreviewHeight, float cameraFOV)
{
    _cameraPreviewWidth = cameraPreviewWidth;
    _cameraPreviewHeight = cameraPreviewHeight;
    _cameraFOV = cameraFOV;
}


/**
 * Perform inits, create objects for detecting corners and rendering image
 */
void CornerClass::PerformGLInits() {

    MyLOGD("CornerClass::PerformGLInits");

    MyGLInits();

    back = new BackTexture(cameraPreviewWidth, cameraPreviewHeight);

    CheckGLError("CornerClass::PerformGLInits");

    // Todd: Need to be done here since we need OpenGL context.
    _teapot = new Teapot();

    CheckGLError("Initialize the teapot");

    _myGLCamera = new MyGLCamera(_cameraFOV, 0);

    newCameraImage = false;
    initsDone = true;

}



/**
 * Render to the display
 */
void CornerClass::Render() {

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cameraMutex.try_lock();
    if(newCameraImage) {
        back->LoadBackImg(cameraImageForBack);
    }
    newCameraImage = false;
    cameraMutex.unlock();
    //back->Render();
    glm::mat4 mvpMat = _myGLCamera->GetMVP();
    _teapot->Render(&mvpMat);
    CheckGLError("CornerClass::Render");

}

/**
 * set the viewport, function is also called when user changes device orientation
 */
void CornerClass::SetViewport(int width, int height) {

    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
    CheckGLError("CornerClass::SetViewport");
    _myGLCamera->SetAspectRatio((float)1280/720);

}

/**
 * Save camera image, detect feature points in it, highlight them
 */
void CornerClass::ProcessCameraImage(cv::Mat cameraRGBImage, int mPreview_width, int mPreview_height) {

    cameraMutex.lock();

    cameraImageForBack = cameraRGBImage.clone();
    // OpenCV image needs to be flipped for OpenGL
    cv::flip(cameraImageForBack, cameraImageForBack, 0);
    DetectAndHighlightCorners();
    newCameraImage = true; // indicate to Render() that a new image is available

    cameraMutex.unlock();
}

/**
 * Use the corner detector to find feature points and draw small circles around them
 */
void CornerClass::DetectAndHighlightCorners(){

    cornerDetector->detect(cameraImageForBack, keyPoints);
    for(int i=0;i<keyPoints.size();i++){

        cv::circle(cameraImageForBack, keyPoints[i].pt, 5, cv::Scalar(255,0,0));

    }
}

/**
 * Camera preview dimensions are saved -- used later to initialize BackTexture object
 */
void CornerClass::SetCameraPreviewDims(int cameraPreviewWidth, int cameraPreviewHeight) {

    this->cameraPreviewWidth = cameraPreviewWidth;
    this->cameraPreviewHeight = cameraPreviewHeight;
}
