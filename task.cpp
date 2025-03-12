#include <iostream>
#include <vector>
#include <variant>
#include <cmath>
#include "convertarc.h"
#include <pthread.h>



// Use std::variant to store either a LineSegment or an Arc
using Segment = std::variant<LineSegment, ArcL>;
pthread_mutex_t consoleMutex = PTHREAD_MUTEX_INITIALIZER;


// Define the Contour class
class Contour {
public:
    // Default constructor
    Contour() = default;

    // Copy constructor
    Contour(const Contour& other)
        : segments(other.segments), Points(other.Points) {}

    // Copy assignment operator
    Contour& operator=(const Contour& other) {
        if (this != &other) {
            segments = other.segments;
            Points = other.Points;
        }
        return *this;
    }

    // Move constructor
    Contour(Contour&& other) noexcept
        : segments(std::move(other.segments)), Points(std::move(other.Points)) {}

    // Move assignment operator
    Contour& operator=(Contour&& other) noexcept {
        if (this != &other) {
            segments = std::move(other.segments);
            Points = std::move(other.Points);
        }
        return *this;
    }
    // Add a line segment to the contour
    void addLineSegment(const Point& start, const Point& end) {
        segments.emplace_back(LineSegment{start, end});
        Points.emplace_back(start);
        Points.emplace_back(end);

    }

    // Add an arc to the contour
    void addArc(const Point& start, const Point& end, double i,double j ,bool clockwise) {
        segments.emplace_back(ArcL{start,end, i,j,clockwise});
        Points.emplace_back(start);
        Points.emplace_back(end);

    }

    void printPoints() const {
        std::cout << "Stored Points:\n";
        for (const auto& point : Points) {
            std::cout << "(" << point.x << ", " << point.y << ")\n";
        }
    }

    Contour* isValid(double epsilon,int rank) {
        for (int i = 0; i < Points.size()-1; ++i) {
            if (i%2 ==1) {
                double distance = pow(Points[i].x-Points[i+1].x,2)+ pow(Points[i].y-Points[i+1].y,2);
                distance = sqrt(distance);
                if (distance >epsilon && rank==0) {
                    std::cout <<2334<< std::endl;
                    std::cout <<this<< std::endl;
                    return this;
                }
            }
        }
        if (rank==1) {
            std::cout <<2335<< std::endl;
            std::cout <<this<< std::endl;
            return this;


        }
    }

private:
    std::vector<Segment> segments;
    std::vector<Point> Points;
    void convertArcToSegments(const ArcL& arc) {
        linearizer(arc);



    }
};
struct ThreadData {
    int thread_id;
    std::vector<Contour>* contours;
    double epsilon;
};
void* processContours(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    pthread_mutex_lock(&consoleMutex);
    std::cout << "Thread ID " << data->thread_id << " started.\n";
    pthread_mutex_unlock(&consoleMutex);

    for (auto& contour : *(data->contours)) {
        if (contour.isValid(data->epsilon, data->thread_id)) {
            pthread_mutex_lock(&consoleMutex);
            std::cout << "Thread ID " << data->thread_id << " - Valid Contour: " << &contour << std::endl;
            pthread_mutex_unlock(&consoleMutex);
        } else {
            pthread_mutex_lock(&consoleMutex);
            std::cout << "Thread ID " << data->thread_id << " - Invalid Contour: " << &contour << std::endl;
            pthread_mutex_unlock(&consoleMutex);
        }
    }
    return nullptr;
}

int main() {
    std::vector<Contour> contours;

    Contour contour1;
    contour1.addLineSegment({4.0005, 2.0005}, {6.0005, 2.0005});
    contour1.addArc({6.0005, 2.0005}, {10.0005, 6.0005}, 4.0005, 0.0005, true);
    contour1.addArc({10.0005, 6.0005}, {14.0005, 2.0005}, 0.0005, -3.9995, true);
    contour1.addLineSegment({14.0005, 2.0005}, {16.0005, 2.0005});
    contour1.addLineSegment({16.0005, 2.0005}, {18.0005, 4.0005});
    contour1.addArc({18.0005, 4.0005}, {19.0005, -3.9995}, 0.0005, -3.9995, true);
    contour1.addArc({29.0005, -3.9995}, {21, -1}, 3, 0, true);
    contours.push_back(contour1);


    Contour contour2;
    contour2.addLineSegment({0,0},{0,1});
    contour2.addLineSegment({0,1},{2,1});
    contour2.addLineSegment({2,1},{2,2});
    contour2.addLineSegment({2,2},{5,6});
    contours.push_back(contour2);

    Contour contour3;
    contour3.addLineSegment({0,0},{200,100});
    contour3.addLineSegment({200,100},{-5,-9});
    contour3.addLineSegment({-5,-11},{456,711});
    contour3.addLineSegment({456,711},{5,6});
    contours.push_back(contour3);

    int no_of_threads = 2;

    // ✅ Use `std::vector` instead of malloc
    std::vector<pthread_t> thread_handles(no_of_threads);
    std::vector<ThreadData> thread_data(no_of_threads);

    // Initialize thread data with unique IDs
    thread_data[0] = {0, &contours, 0.0001};
    thread_data[1] = {1, &contours, 0.0001};

    pthread_create(&thread_handles[0], nullptr, processContours, &thread_data[0]);
    pthread_create(&thread_handles[1], nullptr, processContours, &thread_data[1]);

    // Wait for threads to finish
    pthread_join(thread_handles[0], nullptr);
    pthread_join(thread_handles[1], nullptr);









    return 0;
}
