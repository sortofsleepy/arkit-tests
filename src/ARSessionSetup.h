//
//  SessionSetup.h
//
//  Created by Joseph Chow on 11/16/17.
//  Copyright © 2017 Joseph Chow. All rights reserved.
//

#ifndef SessionSetup_h
#define SessionSetup_h

namespace ARCore {
    
    typedef struct {
        bool useFaceTracking = false;
        bool usePlaneTracking = false;
        bool useImageDetection = false;
        bool useLightEstimation = false;
        bool useAudio = false;
        bool useHighRes = true;
        bool useAutoFocus = true;
        
        NSObject<ARSessionDelegate> * delegateClass = NULL;
        
        // the options for alignment are -
        // 1. ARWorldAlignmentCamera - https://developer.apple.com/documentation/arkit/arworldalignment/arworldalignmentcamera?language=objc
        // 2. ARWorldAlignmentGravityAndHeading - https://developer.apple.com/documentation/arkit/arworldalignment/arworldalignmentgravityandheading?language=objc
        // 3. ARWorldAlignmentGravity - https://developer.apple.com/documentation/arkit/arworldalignment/arworldalignmentgravity?language=objc
        
        ARWorldAlignment worldAlignment = ARWorldAlignmentGravity;
        ARPlaneDetection planeDetectionType;
        
        string imageBundleName = "AR Resources";
        
    }FormatState;
    
    class SFormat {
        
      
        FormatState state;
     
        
    public:
        SFormat(){};
        
       
        // Returns the current state
        FormatState getState(){
            return state;
        }
        
        //! Enables light estimation for the session
        SFormat& enableLighting(){
            state.useLightEstimation = true;
            return *this;
        }
        
        //! Enables face tracking for the session.
        //! TODO does face tracking affect other things like lighting and plane detection?
        SFormat& enableFaceTracking(){
            state.useFaceTracking = true;
            return *this;
        }
        
        //! Enables feature detection
        SFormat & enableImageDetection(string imageBundleName = "AR Resources"){
            state.useImageDetection = true;
            state.imageBundleName = imageBundleName;
            return *this;
        }
        
        //! Enables plane detection - vertical and horizontal
        SFormat& enablePlaneTracking(){
            // not all devices can support plane tracking, check first to make sure it's supported.
            if([ARWorldTrackingConfiguration isSupported]){
                state.usePlaneTracking = true;
                if (@available(iOS 11.3, *)) {
                    state.planeDetectionType = ARPlaneDetectionHorizontal | ARPlaneDetectionVertical;
                } else {
                    // Fallback on earlier versions
                    state.planeDetectionType = ARPlaneDetectionHorizontal;
                }
            }else {
                NSLog(@"This device is unfortunately unable to use plane tracking");
            }
            return *this;
        }
        
        //! Enables horizontal plane detection.
        SFormat& enableHorizontalPlaneTracking(){
            // not all devices can support plane tracking, check first to make sure it's supported.
            if([ARWorldTrackingConfiguration isSupported]){
                state.usePlaneTracking = true;
                state.planeDetectionType = ARPlaneDetectionHorizontal;
            }else {
                NSLog(@"This device is unfortunately unable to use plane tracking");
            }
            
            return *this;
        }
        
        //! Enables vertical plane detection.
        SFormat& enableVerticalPlaneTracking(){
            // not all devices can support plane tracking, check first to make sure it's supported.
            if([ARWorldTrackingConfiguration isSupported]){
                state.usePlaneTracking = true;
                if (@available(iOS 11.3, *)) {
                    state.planeDetectionType = ARPlaneDetectionVertical;
                } else {
                    state.planeDetectionType = ARPlaneDetectionHorizontal;
                    NSLog(@"This version of iOS is too old to support vertical plane tracking.");
                }
            }else {
                NSLog(@"This device is unfortunately unable to use plane tracking");
            }
            
            return *this;
        }
        
        //! Sets the delegate class for the session.
        SFormat& setDelegate(NSObject<ARSessionDelegate> * delegateClass){
            state.delegateClass = delegateClass;
            return *this;
        }
        
        SFormat& setHighRes( bool bHighRes ){
            state.useHighRes = bHighRes;
            return *this;
        }
    };
    
    //! Generates a new ARSession object. Pass in a SFormat object describing the
    //! settings you want to enable on the session.
    static ARSession * generateNewSession(SFormat format){
        ARSession * session = [ARSession new];
        
        auto state = format.getState();
        
        
        // first check if we want face tracking and if it's supported.
        // Currently unknown if this affects other possible tracking implementations since
        // it has it's own configuration type.
        if(state.useFaceTracking){
            
            if([ARFaceTrackingConfiguration isSupported]){
                ARFaceTrackingConfiguration * config = [ARFaceTrackingConfiguration new];
                
                if(state.useLightEstimation){
                    config.lightEstimationEnabled = YES;
                }
                
                config.worldAlignment = state.worldAlignment;
                
                if(state.delegateClass != NULL){
                    session.delegate = state.delegateClass;
                    
                    // note that audio is only available when you use a delegate class.
                    if(state.useAudio){
                        config.providesAudioData = YES;
                    }
                }
                
                [session runWithConfiguration:config];
                
                return session;
            }else{
                NSLog(@"Unable to use face tracking configuration, defaulting to a more standard config.");
            }
        }
        
        // if face tracking is not available, should pass through to here where we
        // figure out regular configuration, starting with determining if we can do plane detection.
        if([ARWorldTrackingConfiguration isSupported]){
            
            ARWorldTrackingConfiguration * config = [ARWorldTrackingConfiguration new];
            
            if(state.usePlaneTracking){
                config.planeDetection = state.planeDetectionType;
            }
            
            if(state.useLightEstimation){
                config.lightEstimationEnabled = YES;
            }
            
            // image detection
            if (@available(iOS 11.3, *)) {
                if ( state.useImageDetection ){
                    // this looks crazier than it is!
                    // basically, it just passes in the name of your resource group
                    config.detectionImages = [ARReferenceImage referenceImagesInGroupNamed:[NSString stringWithUTF8String:state.imageBundleName.c_str()] bundle:[NSBundle mainBundle]];
                }
            }
            
            config.worldAlignment = state.worldAlignment;
            if (@available(iOS 11.3, *)) {
                if ( state.useAutoFocus ){
                    config.autoFocusEnabled = true;
                }
            }
            
            if (@available(iOS 11.3, *)) {
                // WIP API: high res = default 1080p (first item in configs array)
                if (state.useHighRes){
                    config.videoFormat = ARWorldTrackingConfiguration.supportedVideoFormats[0];
                
                // Backup is lowest res avail; right now seems to be 720p
                } else {
                    config.videoFormat = ARWorldTrackingConfiguration.supportedVideoFormats.lastObject;
                }
            }
            
            if(state.delegateClass != NULL){
                session.delegate = state.delegateClass;
                
                // note that audio is only available when you use a delegate class.
                if(state.useAudio){
                    config.providesAudioData = YES;
                }
            }
            
            [session runWithConfiguration:config];
            
            
        }else {
            // if we can't do plane detection, switch to regular tracking.
            AROrientationTrackingConfiguration * config = [AROrientationTrackingConfiguration new];
            if(state.useLightEstimation){
                config.lightEstimationEnabled = YES;
            }
            
            config.worldAlignment = state.worldAlignment;
            
            if(state.delegateClass != NULL){
                session.delegate = state.delegateClass;
                
                // note that audio is only available when you use a delegate class.
                if(state.useAudio){
                    config.providesAudioData = YES;
                }
            }
            
            [session runWithConfiguration:config];
            
        }
        
        
        
      
        return session;
        
    }
}

#endif /* SessionSetup_h */

