//
//  scrDraw3D.cpp
//  PosRotSca-test
//
//  Created by Elliot Woods on 26/06/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "scrDraw3D.h"

scrDraw3D::scrDraw3D(string caption, ofNode& node) :
_node(node),
scrBase(caption)
{
	ofAddListener(ofEvents().keyPressed, this, &scrDraw3D::keyPressed);
	camera.setPosition(0,0,1.0f);
	camera.setNearClip(0.01f);
    
	gridSize = 0.0f;
    bUseCalibration = false;
    intrinsics = NULL;
    modelMatrix = NULL;
}

void scrDraw3D::drawContent()
{
	ofRectangle viewport = getLiveBounds();
    
    if(bUseCalibration) {
        
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        intrinsics->loadProjectionMatrix(0.01, 100);
        
        ofxCv::applyMatrix(*modelMatrix);
        
    } else {
        camera.setCursorDraw(true);
        camera.begin(viewport);
    }

	glEnable(GL_DEPTH_TEST);
	if (gridSize > 0)
		ofDrawGrid(gridSize);
	_node.draw();
    
    if(!bUseCalibration) {
        
        _node.transformGL();
        ofNotifyEvent(evtDraw3D, _node);
        _node.restoreTransformGL();
        
    }
	
	glDisable(GL_DEPTH_TEST);
    
    if(bUseCalibration) {
        
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
    else {
        camera.end();
    }
}

void scrDraw3D::keyPressed(ofKeyEventArgs &args) {
	switch(args.key) {
		case OF_KEY_RIGHT:
			camera.truck(0.1f);
			break;
			
		case OF_KEY_LEFT:
			camera.truck(-0.1f);
			break;
			
		case OF_KEY_UP:
			camera.dolly(0.1f);
			break;
			
		case OF_KEY_DOWN:
			camera.dolly(-0.1f);
			break;
	}
}

ofNode& scrDraw3D::getNodeReference() {
	return _node;
}

void scrDraw3D::enableGrid(float size) {
	gridSize = size;
}