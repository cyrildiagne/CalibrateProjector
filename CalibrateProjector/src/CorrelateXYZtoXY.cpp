//
//  CorrelateXYZtoXY.cpp
//  CalibrateProjector
//
//  Created by Elliot Woods on 14/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#include "CorrelateXYZtoXY.h"

CorrelateXYZtoXY::CorrelateXYZtoXY() {
	
	cvParameters.insert(pair<string, bool>("CV_CALIB_USE_INTRINSIC_GUESS", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_PRINCIPAL_POINT", false));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_ASPECT_RATIO", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_ZERO_TANGENT_DIST", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_K1", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_K2", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_K3", true));
	
	
	projectorResolution.x = 1280;
	projectorResolution.y = 800;

	clear();
	
	xyzPreview.setMode(OF_PRIMITIVE_POINTS);
}

void CorrelateXYZtoXY::push(ofVec3f &xyz, ofVec2f &xy) {
    
	if (xyz.length() < 0.1)
		return;
	
	++count;
	this->xyz.push_back(toCv(xyz));
	this->xy.push_back(toCv(xy));
	
	xyzPreview.addVertex(xyz);
}

void CorrelateXYZtoXY::clear() {
	xyz.clear();
	xy.clear();
	xyzPreview.clear();
	count=0;
	error=0;
	ready=false;
}

int CorrelateXYZtoXY::size() {
	return xyz.size();
}

void CorrelateXYZtoXY::draw() {
	ofPushStyle();
	xyzPreview.draw();
	ofPopStyle();
}

void CorrelateXYZtoXY::correlate() {
    
    cout << "\n CORRELATE :\n" << endl;
    
	if (size() < 11)
		return;
    
    Size2i imageSize(projectorResolution.x, projectorResolution.y);
    float f = imageSize.width * ofDegToRad(80); // i think this is wrong, but it's optimized out anyway
    Point2f c = Point2f(imageSize) * 0.5;
	cameraMatrix = (Mat1d(3, 3) <<
					f, 0, c.x,
					0, f, c.y,
					0, 0, 1);
    
    
    // generate flags
	int flags = 0;
	flags = CV_CALIB_USE_INTRINSIC_GUESS;
    flags |= CV_CALIB_FIX_ASPECT_RATIO;
    flags |= CV_CALIB_FIX_K1 | CV_CALIB_FIX_K2 | CV_CALIB_FIX_K3;
    flags |= CV_CALIB_ZERO_TANGENT_DIST;
    //flags |= CV_CALIB_FIX_PRINCIPAL_POINT;
    
    vector<Mat> rvecs, tvecs;
	
	vector<vector<Point3f> > referenceObjectPoints(1); //object points
	vector<vector<Point2f> > referenceImagePoints(1); //image points
    
    referenceObjectPoints[0] = xyz;
    referenceImagePoints[0] = xy;
	
	
	try {
		error = calibrateCamera(referenceObjectPoints,
                                referenceImagePoints,
                                imageSize,
                                cameraMatrix,
                                distCoeffs,
                                rvecs,
                                tvecs,
                                flags);
		ready=true;
	} catch (...) {
		ofLogError() << "Calibration threw exception. Check parameters.";
	}
	
	intrinsics.setup(cameraMatrix, imageSize);

	rvec = rvecs[0];
    tvec = tvecs[0];
    modelMatrix = makeMatrix(rvec, tvec);
    
	cout << "CAMERA MATRIX :" << endl;
    cout << cameraMatrix << endl;
    cout << "DIST COEFFS :" << endl;
    cout << distCoeffs << endl;
    cout << "MODEL MATRIX :" << endl;
    cout << modelMatrix << endl;
}

ofVec2f CorrelateXYZtoXY::project(const Point3f o) const {
	vector<cv::Point3f> vo(1, o);
	
	vector<Point2f> projected;
	
	projectPoints(Mat(vo), rvec, tvec, cameraMatrix, distCoeffs, projected);
	
	return toOf(projected[0]);
}