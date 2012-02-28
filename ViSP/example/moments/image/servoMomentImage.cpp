/****************************************************************************
 *
 * $Id$
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 *
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 * Example of visual servoing with moments using an image as object
 * container
 *
 * Authors:
 * Filip Novotny
 *
 *****************************************************************************/

/*!
  \example servoMomentImage.cpp
  Example of moment-based visual servoing with Images
*/

#include <visp/vpDebug.h>
#include <visp/vpConfig.h>
#include <iostream>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpMomentObject.h>
#include <visp/vpMomentDatabase.h>
#include <visp/vpMomentCommon.h>
#include <visp/vpFeatureMomentCommon.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpIoTools.h>
#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpServo.h>
#include <visp/vpDebug.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpPlane.h>


//setup robot parameters
void paramRobot();

//update moment objects and interface
void refreshScene(vpMomentObject &obj);
//initialize scene in the interface
void initScene();
//initialize the moment features
void initFeatures();

void init(vpHomogeneousMatrix& cMo, vpHomogeneousMatrix& cdMo);
void execute(int nbIter); //launch the simulation
void setInteractionMatrixType(vpServo::vpServoIteractionMatrixType type);
double error();
void _planeToABC(vpPlane& pl, double& A,double& B, double& C);
void paramRobot();

#if !defined(WIN32) && !defined(VISP_HAVE_PTHREAD)
// Robot simulator used in this example is not available
int main()
{
  std::cout << "Can't run this example since vpSimulatorAfma6 capability is not available." << std::endl;
  std::cout << "You should install pthread third-party library." << std::endl;
}
// No display available
#elif !defined(VISP_HAVE_X11) && !defined(VISP_HAVE_OPENCV) && !defined(VISP_HAVE_GDI) && !defined(VISP_HAVE_D3D9) && !defined(VISP_HAVE_GTK)
int main()
{
  std::cout << "Can't run this example since no display capability is available." << std::endl;
  std::cout << "You should install one of the following third-party library: X11, OpenCV, GDI, GTK." << std::endl;
}
#else
int main()
{
    //intial pose
    vpHomogeneousMatrix cMo(-0.1,-0.1,1.5,-vpMath::rad(20),-vpMath::rad(20),-vpMath::rad(30));
    //Desired pose
    vpHomogeneousMatrix cdMo(vpHomogeneousMatrix(0.0,-0.0,1.0,vpMath::rad(0),vpMath::rad(0),-vpMath::rad(0)));

    //init the simulation
    init(cMo,cdMo);

    execute(1500);
    return 0;
}


//init the right display
#if defined VISP_HAVE_X11
vpDisplayX displayInt;
#elif defined VISP_HAVE_OPENCV
vpDisplayOpenCV displayInt;
#elif defined VISP_HAVE_GDI
vpDisplayGDI displayInt;
#elif defined VISP_HAVE_D3D9
vpDisplayD3D displayInt;
#elif defined VISP_HAVE_GTK
vpDisplayGTK displayInt;
#endif

//start and destination positioning matrices
vpHomogeneousMatrix cMo;
vpHomogeneousMatrix cdMo;

vpRobotCamera robot;//robot used in this simulation
vpImage<vpRGBa> Iint(480,640, 0);//internal image used for interface display
vpServo task;    //servoing task
vpCameraParameters cam;//robot camera parameters
double _error; //current error
vpImageSimulator imsim;//image simulator used to simulate the perspective-projection camera

//several images used in the simulation
vpImage<unsigned char> cur_img(480,640, 0);
vpImage<unsigned char> src_img(480,640, 0);
vpImage<unsigned char> dst_img(480,640, 0);
vpImage<vpRGBa> start_img(480,640, 0);
vpServo::vpServoIteractionMatrixType interaction_type; //current or desired
//source and destination objects for moment manipulation
vpMomentObject src(6);
vpMomentObject dst(6);

//moment sets and their corresponding features
vpMomentCommon *moments;
vpMomentCommon *momentsDes;
vpFeatureMomentCommon *featureMoments;
vpFeatureMomentCommon *featureMomentsDes;

using namespace std;


void initScene(){
    vpColVector X[4];
    for (int i = 0; i < 4; i++) X[i].resize(3);
    X[0][0] = -0.2;
    X[0][1] = -0.1;
    X[0][2] = 0;

    X[1][0] = 0.2;
    X[1][1] = -0.1;
    X[1][2] = 0;

    X[2][0] = 0.2;
    X[2][1] = 0.1;
    X[2][2] = 0;

    X[3][0] = -0.2;
    X[3][1] = 0.1;
    X[3][2] = 0;
    //init source and destination images
    vpImage<unsigned char> tmp_img(480,640,255);
    vpImage<vpRGBa> tmp_start_img(480,640,vpRGBa(255,0,0));

    vpImageSimulator imsim_start;
    imsim_start.setInterpolationType(vpImageSimulator::BILINEAR_INTERPOLATION) ;
    imsim_start.init(tmp_start_img, X);
    imsim_start.setCameraPosition(cdMo);
    imsim_start.getImage(start_img,cam);

    imsim.setInterpolationType(vpImageSimulator::BILINEAR_INTERPOLATION) ;
    imsim.init(tmp_img, X);

    imsim.setCameraPosition(cMo);
    imsim.getImage(src_img,cam);

    src.setType(vpMomentObject::DENSE_FULL_OBJECT);
    src.fromImage(src_img,128,cam);

    dst.setType(vpMomentObject::DENSE_FULL_OBJECT);
    imsim.setCameraPosition(cdMo);
    imsim.getImage(dst_img,cam);
    dst.fromImage(dst_img,128,cam);

}

void refreshScene(vpMomentObject &obj){
    cur_img = 0;
    imsim.setCameraPosition(cMo);
    imsim.getImage(cur_img,cam);
    obj.fromImage(cur_img,128,cam);
}

void init(vpHomogeneousMatrix& _cMo, vpHomogeneousMatrix& _cdMo)
{
    cMo = _cMo; //init source matrix
    cdMo = _cdMo; //init destination matrix

    interaction_type = vpServo::CURRENT;//use interaction matrix for current position

    displayInt.init(Iint,700,0, "Visual servoing with moments") ;

    paramRobot(); //set up robot parameters

    task.setServo(vpServo::EYEINHAND_CAMERA);
    initScene(); //initialize graphical scene (for interface)
    initFeatures();//initialize moment features
}

void initFeatures(){
    //A,B,C parameters of source and destination plane
    double A; double B; double C;
    double Ad; double Bd; double Cd;
    //init main object: using moments up to order 5

    //Initializing values from regular plane (with ax+by+cz=d convention)
    vpPlane pl;
    pl.setABCD(0,0,1.0,0);
    pl.changeFrame(cMo);
    _planeToABC(pl,A,B,C);

    pl.setABCD(0,0,1.0,0);
    pl.changeFrame(cdMo);
    _planeToABC(pl,Ad,Bd,Cd);

    //extracting initial position (actually we only care about Zdst)
    vpTranslationVector vec;
    cdMo.extract(vec);

    ///////////////////////////// initializing moments and features /////////////////////////////////
    //don't need to be specific, vpMomentCommon automatically loads Xg,Yg,An,Ci,Cj,Alpha moments
    moments = new vpMomentCommon(vpMomentCommon ::getSurface(dst),vpMomentCommon::getMu3(dst),vpMomentCommon::getAlpha(dst), vec[2]);
    momentsDes = new vpMomentCommon(vpMomentCommon::getSurface(dst),vpMomentCommon::getMu3(dst),vpMomentCommon::getAlpha(dst),vec[2]);
    //same thing with common features
    featureMoments = new vpFeatureMomentCommon(*moments);
    featureMomentsDes = new vpFeatureMomentCommon(*momentsDes);

    moments->updateAll(src);
    momentsDes->updateAll(dst);

    featureMoments->updateAll(A,B,C);
    featureMomentsDes->updateAll(Ad,Bd,Cd);

    //setup the interaction type
    task.setInteractionMatrixType(interaction_type) ;
    //////////////////////////////////add useful features to task//////////////////////////////
    task.addFeature(featureMoments->getFeatureGravityNormalized(),featureMomentsDes->getFeatureGravityNormalized());
    task.addFeature(featureMoments->getFeatureAn(),featureMomentsDes->getFeatureAn());
    //the moments are different in case of a symmetric object
    task.addFeature(featureMoments->getFeatureCInvariant(),featureMomentsDes->getFeatureCInvariant(),(1 << 10) | (1 << 11));
    task.addFeature(featureMoments->getFeatureAlpha(),featureMomentsDes->getFeatureAlpha());

    task.setLambda(1.) ;
}

void execute(int nbIter){
    //init main object: using moments up to order 6
    vpMomentObject obj(6);
    //setting object type (disrete, continuous[form polygon])
    obj.setType(vpMomentObject::DENSE_FULL_OBJECT);

    vpTRACE("Display task information " ) ;
    task.print() ;

    vpDisplay::display(Iint);    
    vpDisplay::flush(Iint);
    int iter=0;
    double t=0;
	robot.setPosition(cMo);
	float sampling_time = 0.010f; // Sampling period in seconds
	robot.setSamplingTime(sampling_time);
    ///////////////////SIMULATION LOOP/////////////////////////////
    while(iter++<nbIter ){
        vpColVector v ;
        t = vpTime::measureTimeMs();
        //get the cMo
        robot.getPosition(cMo);
        //setup the plane in A,B,C style
        vpPlane pl;
        double A,B,C;
        pl.setABCD(0,0,1.0,0);
        pl.changeFrame(cMo);
        _planeToABC(pl,A,B,C);

        //track points, draw points and add refresh our object
        refreshScene(obj);
        //this is the most important thing to do: update our moments
        moments->updateAll(obj);
        //and update our features. Do it in that order. Features need to use the information computed by moments
        featureMoments->updateAll(A,B,C);
        //some graphics again
        imsim.setCameraPosition(cMo);

        Iint = start_img;

        imsim.getImage(Iint,cam);
        vpDisplay::display(Iint) ;
        
        vpDisplay::flush(Iint);

        if (iter == 1)
            vpDisplay::getClick(Iint) ;
        v = task.computeControlLaw() ;
        //pilot robot using position control. The displacement is t*v with t=10ms step
        //robot.setPosition(vpRobot::CAMERA_FRAME,0.01*v);
		robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;
    vpTime::wait(t, sampling_time * 1000); // Wait 10 ms

        //vpTime::wait(t,10);
        _error = task.error.sumSquare();
    }

    task.kill();

    vpTRACE("\n\nClick in the internal view window to end...");
    vpDisplay::getClick(Iint) ;

    delete moments;
    delete momentsDes;
    delete featureMoments;
    delete featureMomentsDes;
}

void setInteractionMatrixType(vpServo::vpServoIteractionMatrixType type){interaction_type=type;}
double error(){return _error;}



void _planeToABC(vpPlane& pl, double& A,double& B, double& C){
	if(fabs(pl.getD())<std::numeric_limits<double>::epsilon()){
		std::cout << "Invalid position:" << std::endl;
		std::cout << cMo << std::endl;
		std::cout << "Cannot put plane in the form 1/Z=Ax+By+C." << std::endl;
		throw vpException(vpException::divideByZeroError,"invalid position!");
	}
    A=-pl.getA()/pl.getD();
    B=-pl.getB()/pl.getD();
    C=-pl.getC()/pl.getD();
}

void paramRobot(){
	cam = vpCameraParameters(640,480,320,240);
}

#endif