#ifndef TASK_H
#define TASK_H
#include <vector>
#include <variant>
#include <list>

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
using Segment = std::variant<LineSegment, ArcL>;

class Contour {
public:
    Contour();
    void addLineSegment(const Point& start, const Point& end);
    void addArc(const Point& start, const Point& end, double i, double j, bool clockwise);
    bool isValid(double epsilon) const;
    void removeSegmentAtIndex(int index);
    void addSegmentAtIndex(int index, const Segment& segment);

    static Contour fromPolyline(const std::vector<Point>& points);
private:
    std::vector<Segment> segments;
    std::vector<Point> Points;
};

#endif