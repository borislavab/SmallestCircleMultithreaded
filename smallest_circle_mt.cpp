#include "smallest_circle.h"

#include <iostream>

#include <thread>
#include <queue>

extern bool quietMode;

#define MY_PRINT(x)\
if (!quietMode) {\
    cout << x << endl;\
}\

struct PointDistance {
    Point A;
    Point B;
    double distance;

    PointDistance() {
        distance = -1;
    }
};

void findFarthestPointsThreadMain(
    const vector<Point>& points,
    PointDistance * result,
    int threadIndex,
    int threadCount,
    int taskSize
) {
    MY_PRINT("Thread " << threadIndex << " started.")
    int pointsCount = points.size();
    int currentPointInTask = 0;
    int currentThreadIndex = 0;
    for (int i = 0; i < pointsCount - 1; ++i) {
        if (currentThreadIndex == threadIndex) {
            const Point& A = points[i];
            for (int j = i + 1; j < pointsCount; ++j) {
                const Point& B = points[j];
                double distance = A.dist(B);
                if (distance > result->distance) {
                    result->A = A;
                    result->B = B;
                    result->distance = distance;
                }
            }
        }
        currentPointInTask++;
        if(currentPointInTask >= taskSize) {
            currentPointInTask = 0;
            currentThreadIndex = (currentThreadIndex + 1) % threadCount;
        }
    }
    MY_PRINT("Thread " << threadIndex << " stopped.")
}

void arePointsInCircleThreadMain(
    const vector<Point>& points,
    vector<bool>* result,
    Circle circle,
    int threadIndex,
    int threadCount,
    int taskSize
) {
    int pointsCount = points.size();
    int currentPointInTask = 0;
    int currentThreadIndex = 0;
    for (int i = 0; i < pointsCount; ++i) {
        if (currentThreadIndex == threadIndex) {
            if (!circle.isPointInside(points[i])) {
                (*result)[threadIndex] = false;
                return;
            }
        }
        currentPointInTask++;
        if(currentPointInTask >= taskSize) {
            currentPointInTask = 0;
            currentThreadIndex = (currentThreadIndex + 1) % threadCount;
        }
    }
    (*result)[threadIndex] = true;
}

void findSmallest3PointCircleThreadMain(
    const vector<Point>& points, 
    Circle* result, 
    int threadIndex,
    int threadCount,
    int taskSize
) {
    int pointsCount = points.size();
    int currentPointInTask = 0;
    int currentThreadIndex = 0;
    for (int i = 0; i < pointsCount - 2; ++i) {
        if (currentThreadIndex == threadIndex) {
            const Point &currentA = points[i];
            for (int j = i + 1; j < pointsCount - 1; ++j) {
                const Point &currentB = points[j];
                for (int k = j + 1; k < pointsCount; ++k) {
                    const Point &currentC = points[k];
                    if (Point::areOnSameLine(currentA, currentB, currentC)) {
                        continue;
                    }
                    Circle circleFrom3Points = Circle::constructFrom3Points(currentA, currentB, currentC);
                    if (result->radius >= 0 && circleFrom3Points.radius >= result->radius) {
                        continue;
                    }
                    bool areAllPointsInside = circleFrom3Points.areAllPointsInside(points);
                    if (areAllPointsInside) {
                        *result = circleFrom3Points;
                    }
                }
            }  
        }
        currentPointInTask++;
        if(currentPointInTask >= taskSize) {
            currentPointInTask = 0;
            currentThreadIndex = (currentThreadIndex + 1) % threadCount;
        }
    }
}

Circle findSmallestCircleMT(const vector<Point> & points, int threadCount, int granularity) {

    int taskCount = threadCount * granularity;
    int taskSize = points.size() / taskCount;

    vector<thread> workers;

    vector<PointDistance> distanceThreadResults(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        workers.push_back(
            thread(
                findFarthestPointsThreadMain, 
                points, 
                &distanceThreadResults[i], 
                i, 
                threadCount, 
                taskSize
            ));
    }

    for (auto& worker : workers) {
        worker.join();
    }

    PointDistance* farthestPointsDistance = &distanceThreadResults[0];
    for (int i = 1; i < distanceThreadResults.size(); ++i) {
        if (distanceThreadResults[i].distance > farthestPointsDistance->distance) {
            farthestPointsDistance = &distanceThreadResults[i];
        }
    }

    Circle smallestCircle2Points = Circle::constructFrom2Points(farthestPointsDistance->A, farthestPointsDistance->B);
    
    vector<bool> arePointsInCircleThreadResults(threadCount);
    workers.clear();
    for (int i = 0; i < threadCount; ++i) {
        workers.push_back(
            thread(
                arePointsInCircleThreadMain, 
                points, 
                &arePointsInCircleThreadResults, 
                smallestCircle2Points, 
                i, 
                threadCount, 
                taskSize
            ));
    }

    for (auto & worker : workers) {
        worker.join();
    }

    bool areAllPointsInside = true;
    for (int i = 0; i < arePointsInCircleThreadResults.size(); ++i) {
        if (!arePointsInCircleThreadResults[i]) {
            areAllPointsInside = false;
            break;
        }
    }

    if (areAllPointsInside) {
        return smallestCircle2Points;
    }

    workers.clear();
    vector<Circle> smallestCircleThreadResults(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        workers.push_back(
            thread(
                findSmallest3PointCircleThreadMain, 
                points, 
                &smallestCircleThreadResults[i], 
                i, 
                threadCount, 
                taskSize
            ));
    }

    for (auto & worker : workers) {
        worker.join();
    }

    Circle smallestCircle3Points;
    for (auto & circle : smallestCircleThreadResults) {
        if (circle.radius >= 0 && 
            (smallestCircle3Points.radius < 0 || circle.radius < smallestCircle3Points.radius)) {
            smallestCircle3Points = circle;
        }
    }
    return smallestCircle3Points;
}
