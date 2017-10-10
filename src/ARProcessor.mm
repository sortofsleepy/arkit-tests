//
//  CameraFrameConverter.cpp
//  ARToolkit
//
//  Created by Joseph Chow on 8/4/17.
//

#include "ARProcessor.h"
using namespace ARCommon;
using namespace ARCore;

ARProcessor::ARProcessor(){
}

ARProcessor::ARProcessor(ARSession * session){
    this->session = session;
}

ARProcessor::~ARProcessor(){
    pauseSession();
    session = nullptr;
    
    // remove this instance of the ARCam - if there are other ARCams around, they will still be in memory
    camera.reset();
    anchorController.reset();
}

void ARProcessor::toggleDebug(){
    debugMode = !debugMode;
}

void ARProcessor::pauseSession(){
      [session pause];
}

void ARProcessor::logTrackingState(){
    camera->logTrackingState();
}

void ARProcessor::restartSession(){
    [session runWithConfiguration:session.configuration];
}

void ARProcessor::setup(bool debugMode){
    this->debugMode = debugMode;
    anchorController = ARAnchorManager::create(session);
    camera = ARCam::create(session);
    camera->setup(this->debugMode);
}

void ARProcessor::draw(){
    camera->draw();
}

void ARProcessor::update(){
    camera->update();
    if(debugMode){
        pointCloud.updatePointCloud(session.currentFrame);
    }
    
    anchorController->update();
    anchorController->updatePlanes();
}

void ARProcessor::updatePlanes(){
    anchorController->updatePlanes();
}


void ARProcessor::drawFrame(){
    draw();
}
// =========== CAMERA API ============ //
void ARProcessor::setARCameraMatrices(){
    camera->setARCameraMatrices();
}

ofVec3f ARProcessor::getCameraPosition(){
    return ARCommon::getAnchorXYZ(camera->getTransformMatrix());
}

ofTexture ARProcessor::getCameraTexture(){
   return camera->getCameraTexture();
}

ARCommon::ARCameraMatrices ARProcessor::getCameraMatrices(){
     return camera->getCameraMatrices();
}

float ARProcessor::getLightIntensity(){
    return camera->getAmbientIntensity();
}

ARTrackingState ARProcessor::getTrackingState(){
    return camera->getTrackingState();
}

void ARProcessor::rotateCameraFrame(float angle){
    camera->updateRotationMatrix(angle);
}

void ARProcessor::updateDeviceInterfaceOrientation(){
    camera->updateInterfaceOrientation();
}

ARCameraMatrices ARProcessor::getMatricesForOrientation(UIInterfaceOrientation orientation,float near, float far){
    return camera->getMatricesForOrientation(orientation,near,far);
}


void ARProcessor::deviceOrientationChanged(){
    camera->updateDeviceOrientation();
}

// ======= ANCHOR API ========= //
void ARProcessor::addAnchor(float zZoom){
    anchorController->addAnchor(zZoom);
}

void ARProcessor::addAnchor(ofVec3f position){
    anchorController->addAnchor(position);
}

void ARProcessor::drawHorizontalPlanes(){
    anchorController->drawPlanes(camera->getCameraMatrices());
}

// ======== DEBUG API =========== //

void ARProcessor::drawPointCloud(){
    if(debugMode){
        pointCloud.draw(camera->getProjectionMatrix(), camera->getViewMatrix());
    } else {
        ofLog(OF_LOG_WARNING, "Debug Mode not set");
    }
}
