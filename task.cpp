#include <iostream>
#include <vector>
#include <variant>
#include <cmath>
#include "convertarc.h"

// Define a 2D Point struct


// Use std::variant to store either a LineSegment or an Arc
using Segment = std::variant<LineSegment, ArcL>;


// Define the Contour class
class Contour {
public:
    // Add a line segment to the contour
    void addLineSegment(const Point& start, const Point& end) {
        segments.emplace_back(LineSegment{start, end});

    }

    // Add an arc to the contour
    void addArc(const Point& start, const Point& end, double i,double j ,bool clockwise) {
        segments.emplace_back(ArcL{start,end, i,j,clockwise});
        convertArcToSegments(ArcL{start,end, i,j,clockwise});

    }




private:
    std::vector<Segment> segments;
    std::vector<Point> Points;
    void convertArcToSegments(const ArcL& arc) {
        linearizer(arc);



    }
};

int main() {
    Contour contour;
    contour.addLineSegment({4.0005, 2.0005}, {6.0005, 2.0005});
    contour.addArc({6.0005, 2.0005}, {10.0005, 6.0005}, 4.0005, 0.0005, true);
    contour.addArc({10.0005, 6.0005}, {14.0005, 2.0005}, 0.0005, -3.9995, true);

    contour.addLineSegment({14.0005, 2.0005}, {16.0005, 2.0005});
    contour.addLineSegment({16.0005, 2.0005}, {18.0005, 4.0005});
    contour.addArc({18.0005, 4.0005}, {19.0005, -3.9995}, 0.0005, -3.9995, true);
    contour.addArc({19.0005, -3.9995}, {21, -1}, 3, 0, true);


    return 0;
}
