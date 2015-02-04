#include "sim.h"
//#include "simrobot.h"
//#include "world.h"



Sim::Sim(){
  // place robot onto world
  robot.orientation = M_PI/8;
  robot.x = 50;
  robot.y = 50;
  time = 0;
  plotIdx = 0;
  imgBfieldRobot = Mat(140, 500, CV_8UC3, Scalar(0,0,0));
  float steering_noise    = 0.01;
  float distance_noise    = 0.01;
  float measurement_noise = 0.1;
  robot.set_noise(steering_noise, distance_noise, measurement_noise);
  filter.init(robot.x, robot.y, robot.orientation,
              steering_noise*2, distance_noise*2, measurement_noise*2);
}


void Sim::step(){
  float dt=0.04;
  //printf("stateTime=%1.4f\n", stateTime);

  // simulate robot movement
  robot.move(world, robot.steer, robot.speed);
  filter.move(world, robot.steer, robot.speed);
  //var [x,y,theta] = this.filter.get_position();

  robot.sense(world);
  filter.sense(world, robot.bfieldStrength);

  // run robot controller
  robot.control(world, dt);


  // plot robot bfield sensor
  float bfieldStrength = max(-2.0f, min(30.0f, robot.bfieldStrength));
  plotXY(imgBfieldRobot, plotIdx % imgBfieldRobot.cols, 15+bfieldStrength*5, 255,255,255, true);
  imshow("bfieldrobot", imgBfieldRobot);
  plotIdx++;



  // simulation time
  time += dt;
}


void Sim::draw(){
  world.draw();
  filter.draw(world.imgWorld);
  robot.draw(world.imgWorld);
}

void Sim::plotXY(Mat &image, int x, int y, int r, int g, int b, bool clearplot){
  if (clearplot) for (int y=0; y < image.rows; y++) image.at<Point3_<uchar> >(y, x) = Point3_<uchar>(0,0,0);
  if ( (x< 0) || (y < 0) || (x >= image.cols) || (y >= image.rows)) return;
  image.at<Point3_<uchar> >(image.rows-y-1, x) = Point3_<uchar>(r,g,b);
  //line( image, Point( x, image.rows ), Point( x, image.rows-y-1), Scalar( r, g, b),  1, 8 );
}