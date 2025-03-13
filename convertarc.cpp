#include "convertarc.h"
#include <bits/stdc++.h>
#include <cmath>
#include <vector>
#include <iostream>
#define M_PI 3.14159265358979323846




std::list<Point> linearizer(const ArcL& arc) {
    std::list<Point> linearizedPoints;
    double radius = sqrt(pow(arc.i, 2) + pow(arc.j, 2));
    std::vector<double> given_center = {arc.start.x+arc.i,arc.start.y+arc.j};
    double angle1 = findAngles(arc.start,given_center);
    double angle2 = findAngles(arc.end,given_center);
    std::cout << angle1 << " " << angle2 << std::endl;
    double epsilon = 0.0001;

    

    if (arc.clockwise) {
      double angleholder = 0;
      if (std::fabs(angle1 - angle2) > epsilon && angle1 > angle2) {
        for(int i = 0; i < 50; i++ ){

          angleholder = angle2+i*(angle1-angle2)/49;
          Point point;
          point.x = given_center[0]+radius*cos(angleholder);
          point.y = given_center[1]+radius*sin(angleholder);
          linearizedPoints.push_back(point);
        }
      }
      else if (std::fabs(angle1 - angle2) > epsilon && angle2 > angle1) {
        double angleholder = 0;
        angle2 = angle2-2*M_PI;
        for(int i = 0; i < 50; i++ ){
          angleholder = angle2+i*(angle1-angle2)/49;
          Point point;
          point.x = given_center[0]+radius*cos(angleholder);
          point.y = given_center[1]+radius*sin(angleholder);
          linearizedPoints.push_back(point);
        }
        }
        
      }


    else if (!arc.clockwise) {
        if (angle1>angle2) {

      }
      else if (angle2<angle1) {
        
      }

    }


    
return linearizedPoints;
}

double findAngles(const Point& point,std::vector<double>& given_center) {

  double radius = sqrt(pow(point.x-given_center[0], 2)+pow(point.y-given_center[1], 2));
  double asinVal;
  double diff = 0.00000001;





  if (point.x-given_center[0] > diff && point.y-given_center[1] > diff){
      asinVal = asin(abs(point.y - given_center[1]) / (radius + 1e-10));

    return asinVal;

      }
  if (point.x-given_center[0] < diff && point.y-given_center[1] > diff){
      asinVal = asin(abs(point.x - given_center[0]) / (radius + 1e-10));

    return M_PI*0.5+asinVal;
      }

  if (point.x-given_center[0] < diff && point.y-given_center[1] < diff){
    asinVal = asin(abs(point.y - given_center[1]) / (radius + 1e-10));

  return M_PI +asinVal;
  }
  if (point.x-given_center[0] > diff && point.y-given_center[1] < diff){
    asinVal = asin(abs(point.x - given_center[0]) / (radius + 1e-10));

  return 3*M_PI*0.5+asinVal;
  }

}