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



    bool isValid(double epsilon) const {


        if (Points.size() < 2) return false;

        for (size_t i = 0; i < Points.size() - 1; ++i) {

            if (i%2 ==1) {
                double dx = Points[i].x - Points[i + 1].x;
                double dy = Points[i].y - Points[i + 1].y;
                double distance = sqrt(pow(dx, 2) + pow(dy, 2));
                if (distance > epsilon) {
                    return false;
                }


            }
        };
return true;
    }
    void addSegmentAtIndex(int index, const Segment& segment) {
        // Insert the segment into the segments vector
        segments.insert(segments.begin() + index, segment);

        // Extract points based on the segment type
        if (std::holds_alternative<LineSegment>(segment)) {
            auto& seg = std::get<LineSegment>(segment);
            Points.insert(Points.begin() + 2*index, seg.start);
            Points.insert(Points.begin() + 2*index + 1, seg.end);
        } else if (std::holds_alternative<ArcL>(segment)) {
            auto& seg = std::get<ArcL>(segment);
            Points.insert(Points.begin() + 2*index, seg.start);
            Points.insert(Points.begin() + 2*index + 1, seg.end);
        }

        std::cout << "Segment inserted at index " << index << ". Total points: " << Points.size() << "\n";
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
    Contour contour1;

    contour1.addLineSegment({11,12},{13,14});
    contour1.addLineSegment({13,14},{17,18});
    contour1.addArc({170,18},{19,20},5,5,true);
    contour1.addSegmentAtIndex(2,ArcL{{17,18},{170,18}});



    contours.push_back(contour1);

    Contour contour2;

    contour2.addLineSegment({11,12},{13,14});
    contour2.addLineSegment({13,14},{17,18});
    contour2.addArc({170,18},{19,20},5,5,true);
    contour2.addSegmentAtIndex(2,LineSegment{{17,18},{170,18}});


    contours.push_back(contour2);

    Contour contour3;

    contour3.addLineSegment({11,12},{13,14});
    contour3.addLineSegment({13,14},{17,18});
    contour3.addArc({17,18},{19,20},5,5,true);
    contour3.addArc({19,20},{55,46},5,5,true);
    contour3.addLineSegment({55,46},{117,118});


    contours.push_back(contour3);




    // ✅ Use `std::vector` instead of malloc
    std::vector<pthread_t> thread_handles(2);
    std::vector<ThreadData> thread_data(2);

    // ✅ Assign explicit user-defined thread IDs
    thread_data[0] = {1, &contours, 0.01};  // Valid contours
    thread_data[1] = {2, &contours, 0.01};  // Invalid contours

    // ✅ Create separate threads for valid and invalid contours
    pthread_create(&thread_handles[0], nullptr, processValidContours, &thread_data[0]);
    pthread_create(&thread_handles[1], nullptr, processInvalidContours, &thread_data[1]);

    // Wait for threads to finish
    pthread_join(thread_handles[0], nullptr);
    pthread_join(thread_handles[1], nullptr);

    return 0;
}
