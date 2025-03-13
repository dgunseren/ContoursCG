#include <list>
#include <vector>

struct Point {
    double x, y;
};

// Define a Line Segment struct
struct LineSegment {
    Point start, end;
};

// Define an Arc struct (circular arc defined by center, radius, start and end angles)

struct ArcL {
    Point start;
    Point end;
    double i;
    double j; // In radians
    bool clockwise;

};
std::list<Point>  linearizer(const ArcL& arc);
double findAngles(const Point& point,std::vector<double>& given_center) ;