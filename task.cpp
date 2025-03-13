#include <iostream>
#include <vector>
#include <pthread.h>
#include <cmath>
#include <mutex>
#include "convertarc.h"
#include <variant>


using Segment = std::variant<LineSegment, ArcL>;

// POSIX mutex for thread-safe printing
pthread_mutex_t consoleMutex = PTHREAD_MUTEX_INITIALIZER;

class Contour {
public:
    Contour() = default;

    // ✅ Copy Constructor
    Contour(const Contour& other)
        : segments(other.segments), Points(other.Points) {}

    // ✅ Copy Assignment Operator
    Contour& operator=(const Contour& other) {
        if (this != &other) { // Avoid self-assignment
            segments = other.segments;
            Points = other.Points;
        }
        return *this;
    }

    // ✅ Move Constructor
    Contour(Contour&& other) noexcept
        : segments(std::move(other.segments)), Points(std::move(other.Points)) {}

    // ✅ Move Assignment Operator
    Contour& operator=(Contour&& other) noexcept {
        if (this != &other) { // Avoid self-move
            segments = std::move(other.segments);
            Points = std::move(other.Points);
        }
        return *this;
    }

    // ✅ Destructor (default)
    ~Contour() = default;

    void addLineSegment(const Point& start, const Point& end) {
        segments.emplace_back(LineSegment{start, end});
        Points.emplace_back(start);
        Points.emplace_back(end);
    }

    void addArc(const Point& start, const Point& end, double i, double j, bool clockwise) {
        ArcL arc{start, end, i, j, clockwise};
        segments.emplace_back(arc);
        Points.emplace_back(start);
        Points.emplace_back(end);
    }
    void addSegmentAtIndex(int index, const Segment& segment) {
        if (index < 0 || index > segments.size()) {
            std::cerr << "Error: Index out of bounds! Segment not inserted.\n";
            return;
        }

        // Insert the segment at the specified index
        segments.insert(segments.begin() + index, segment);

        // Extract points from the segment and add them to Points vector
        std::visit([this](auto&& seg) {
            Points.emplace_back(seg.start);
            Points.emplace_back(seg.end);
        }, segment);
    }


    bool isValid(double epsilon) const {


        if (Points.size() < 2) return false;

        for (size_t i = 0; i < Points.size() - 1; ++i) {
            if (i%2 ==1) {
                double distance = sqrt(pow(Points[i].x - Points[i + 1].x, 2) + pow(Points[i].y - Points[i + 1].y, 2));
                if (distance > epsilon) {
                    return false;
                }
                return true;
            }
        }

    }

private:
    std::vector<Segment> segments;
    std::vector<Point> Points;
};

// Struct for passing thread data
struct ThreadData {
    int thread_id;
    std::vector<Contour>* contours;
    double epsilon;
};

// ✅ Thread function to process only valid contours
void* processValidContours(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    pthread_mutex_lock(&consoleMutex);
    std::cout << "Thread " << data->thread_id << " processing valid contours.\n";
    pthread_mutex_unlock(&consoleMutex);

    for (const auto& contour : *(data->contours)) {
        if (contour.isValid(data->epsilon)) {
            pthread_mutex_lock(&consoleMutex);
            std::cout << "Thread " << data->thread_id << " - Valid Contour: " << &contour << std::endl;
            pthread_mutex_unlock(&consoleMutex);
        }
    }
    return nullptr;
}

// ✅ Thread function to process only invalid contours
void* processInvalidContours(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    pthread_mutex_lock(&consoleMutex);
    std::cout << "Thread " << data->thread_id << " processing invalid contours.\n";
    pthread_mutex_unlock(&consoleMutex);

    for (const auto& contour : *(data->contours)) {
        if (!contour.isValid(data->epsilon)) {
            pthread_mutex_lock(&consoleMutex);
            std::cout << "Thread " << data->thread_id << " - Invalid Contour: " << &contour << std::endl;
            pthread_mutex_unlock(&consoleMutex);
        }
    }
    return nullptr;
}

int main() {
    std::vector<Contour> contours;

    // ✅ Create contours
    Contour contour1, contour2, contour3, contour4,contour5 ;
    contour1.addLineSegment({4.0005, 2.0005}, {6.0005, 2.0005});
    contour1.addArc({6.0005, 2.0005}, {10.0005, 6.0005}, 4.0005, 0.0005, true);
    contour1.addArc({10.0005, 6.0005}, {14.0005, 2.0005}, 0.0005, -3.9995, true);
    contour1.addLineSegment({14.0005, 2.0005},{3,3});
    contour1.addLineSegment({4, 3},{5,5});
    //contour1.addSegmentAtIndex(2,LineSegment{{3, 3},{5,5}}) ;
    contours.push_back(contour1);

    contour2.addLineSegment({0, 0}, {0, 1});
    contour2.addLineSegment({0, 1}, {2, 1});
    contour2.addLineSegment({2, 1}, {5, 1});

    contours.push_back(contour2);

    contour3.addLineSegment({0, 0}, {200, 100});
    contour3.addLineSegment({200, 100}, {-5, -9});
    contours.push_back(contour3);

    contour4.addLineSegment({10, 10}, {15, 15});
    contour4.addArc({15445, 15}, {20, 20}, 3, 3, false);
    contours.push_back(contour4);

    contour5.addLineSegment({4.0005, 2.0005}, {6.0005, 2.0005});
    contour5.addArc({6.0005, 2.0005}, {10.0005, 6.0005}, 4.0005, 0.0005, true);
    contour5.addArc({10.0005, 6.0005}, {14.0005, 2.0005}, 0.0005, -3.9995, true);
    contour5.addLineSegment({14.0005, 2.0005},{3,3});
    contour5.addLineSegment({1454544, 3},{5,5});
    //contour1.addSegmentAtIndex(2,LineSegment{{3, 3},{5,5}}) ;
    contours.push_back(contour5);

    // ✅ Use `std::vector` instead of malloc
    std::vector<pthread_t> thread_handles(2);
    std::vector<ThreadData> thread_data(2);

    // ✅ Assign explicit user-defined thread IDs
    thread_data[0] = {1, &contours, 0.0000001};  // Valid contours
    thread_data[1] = {2, &contours, 0.0000001};  // Invalid contours

    // ✅ Create separate threads for valid and invalid contours
    pthread_create(&thread_handles[0], nullptr, processValidContours, &thread_data[0]);
    pthread_create(&thread_handles[1], nullptr, processInvalidContours, &thread_data[1]);

    // Wait for threads to finish
    pthread_join(thread_handles[0], nullptr);
    pthread_join(thread_handles[1], nullptr);

    return 0;
}
