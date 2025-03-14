#include "task.h"
#include <variant>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <cmath>
#include <mutex>
#include <set>




using Segment = std::variant<LineSegment, ArcL>;

// POSIX mutex for thread-safe printing
pthread_mutex_t consoleMutex = PTHREAD_MUTEX_INITIALIZER;
Contour::Contour() = default;

void Contour::addLineSegment(const Point& start, const Point& end) {
    segments.emplace_back(LineSegment{start, end});
    Points.emplace_back(start);
    Points.emplace_back(end);
}
void Contour::addArc(const Point& start, const Point& end, double i, double j, bool clockwise) {
    ArcL arc{start, end, i, j, clockwise};
    segments.emplace_back(arc);
    Points.emplace_back(start);
    Points.emplace_back(end);
}
bool Contour::isValid(double epsilon) const {
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
void Contour::addSegmentAtIndex(int index, const Segment& segment) {
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
void Contour::removeSegmentAtIndex(int index) {
    if (index < 0 || index >= segments.size()) {
        std::cerr << "Error: Index out of bounds! Segment not removed.\n";
        return;
    }
    segments.erase(segments.begin() + index);
    Points.erase(Points.begin() + 2*index);
    Points.erase(Points.begin() + 2*index);
}
Contour Contour::fromPolyline(const std::vector<Point>& points) {
    Contour contour;

    if (points.size() < 2) {
        std::cerr << "Warning: Not enough points to form a contour. Returning an empty contour.\n";
        return contour;
    }

    for (size_t i = 0; i < points.size() - 1; ++i) {
        contour.addLineSegment(points[i], points[i + 1]);
    }

    return contour;
}

// Struct for passing thread data
struct ThreadData {
    int thread_id;
    std::vector<Contour>* contours;
    double epsilon;

};

// ✅ Thread function to process only valid contours
void* processValidContours(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    auto* processedContours = new std::vector<const Contour*>;

    pthread_mutex_lock(&consoleMutex);
    std::cout << "Thread " << data->thread_id << " processing valid contours.\n";
    pthread_mutex_unlock(&consoleMutex);

    for (const auto& contour : *(data->contours)) {
        if (contour.isValid(data->epsilon)) {
            pthread_mutex_lock(&consoleMutex);
            std::cout << "Thread " << data->thread_id << " - Valid Contour: " << &contour << std::endl;
            pthread_mutex_unlock(&consoleMutex);
            processedContours->push_back(&contour);
        }
    }
    return processedContours;
}

// ✅ Thread function to process only invalid contours
void* processInvalidContours(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    auto* processedContours = new std::vector<const Contour*>; // Store processed contour addresses


    pthread_mutex_lock(&consoleMutex);
    std::cout << "Thread " << data->thread_id << " processing invalid contours.\n";
    pthread_mutex_unlock(&consoleMutex);

    for (const auto& contour : *(data->contours)) {
        if (!contour.isValid(data->epsilon)) {
            pthread_mutex_lock(&consoleMutex);
            std::cout << "Thread " << data->thread_id << " - Invalid Contour: " << &contour << std::endl;
            pthread_mutex_unlock(&consoleMutex);
            processedContours->push_back(&contour); // Store contour address

        }
    }
    return processedContours;
}

#ifndef RUNNING_TESTS

int main() {

    std::vector<Point> polyline = {
        {0, 0}, {10, 10}, {20, 5}, {30, 15}, {40, 10}
    };

    // ✅ Correct: Call static method on class
    Contour myContour = Contour::fromPolyline(polyline);
    bool xx = myContour.isValid(0.001);
    std::cout << xx << std::endl;

    std::vector<Contour> contours;

    // ✅ Create contours
    Contour contour1;

    contour1.addLineSegment({11,12},{13,14});
    contour1.addLineSegment({13,14},{17,18});
    contour1.addArc({15,18},{19,20},5,5,true);
    //contour1.addSegmentAtIndex(2,ArcL{{17,18},{15,19}});
    contour1.removeSegmentAtIndex(2);



    contours.push_back(contour1);

    Contour contour2;

    contour2.addLineSegment({11,12},{13,14});
    contour2.addLineSegment({13,14},{17,18});
    contour2.addArc({17,18},{19,20},5,5,true);
    contour2.addArc({170,18},{19,20},5,5,true);
    contour2.addSegmentAtIndex(2,LineSegment{{17,18},{170,18}});
    contour2.removeSegmentAtIndex(3);

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

    void* validContoursResult;
    void* invalidContoursResult;

    // ✅ Create separate threads for valid and invalid contours
    pthread_create(&thread_handles[0], nullptr, processValidContours, &thread_data[0]);
    pthread_create(&thread_handles[1], nullptr, processInvalidContours, &thread_data[1]);

    // Wait for threads to finish
    pthread_join(thread_handles[0],  &validContoursResult);
    pthread_join(thread_handles[1], &invalidContoursResult);

    auto* validContours = static_cast<std::vector<const Contour*>*>(validContoursResult);
    auto* invalidContours = static_cast<std::vector<const Contour*>*>(invalidContoursResult);

    std::cout << "Processed Valid Contours:\n";
    for (const auto* c : *validContours) {
        std::cout << c << "\n";
    }

    std::cout << "Processed Invalid Contours:\n";
    for (const auto* c : *invalidContours) {
        std::cout << c << "\n";
    }

    // ✅ Ensure all contours were processed
    std::set<const Contour*> allProcessedContours(validContours->begin(), validContours->end());
    allProcessedContours.insert(invalidContours->begin(), invalidContours->end());

    if (allProcessedContours.size() == contours.size()) {
        std::cout << "All contours were processed.\n";
    } else {
        std::cout << "Warning: Some contours may not have been processed!\n";
    }

    // Free dynamically allocated memory
    delete validContours;
    delete invalidContours;

    return 0;
}
#endif

