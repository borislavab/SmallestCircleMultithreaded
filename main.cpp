#include <iostream>
#include <chrono>
#include <thread>

#include "data_structures.h"
#include "smallest_circle.h"

bool quietMode = false;

class Stopwatch {
public:
    Stopwatch() {
        Reset();
    }

    double ElapsedTime() {
        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double, std::milli>(currentTime - startedAt).count();
        Reset();
        return elapsedTime;
    }

    void Reset() {
        startedAt = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::high_resolution_clock::time_point startedAt;
};

int main(int argc, char** argv) {
    int threadCount = thread::hardware_concurrency(),
        pointsCount = 0,
        granularity = 1;
    string dataFilePath, outputFilePath;
    for (int i = 1; i < argc; ++i) {
        if (string("-i") == argv[i]) {
            dataFilePath = argv[i + 1];
        } else if (string("-n") == argv[i]) {
            istringstream stream(argv[i + 1]);
            stream >> pointsCount;
        } else if (string("-t") == argv[i]) {
            istringstream stream(argv[i + 1]);
            stream >> threadCount;
        } else if (string("-g") == argv[i]) {
            istringstream stream(argv[i + 1]);
            stream >> granularity;
        } else if (string("-o") == argv[i]) {
            outputFilePath = argv[i + 1];
        } else if (string("-q") == argv[i]) {
            quietMode = true;
        }
    }

    vector<Point> points;
    if (!dataFilePath.empty()) {
        points = Point::parseFile(dataFilePath);
    } else {
        points = Point::generatePoints(pointsCount);
    }

    pointsCount = points.size();
    if (pointsCount == 0) {
        cout << "No points specified." << endl;
        return 1;
    }

    if (threadCount > pointsCount) {
        threadCount = pointsCount;
        granularity = 1;
    }

    if (threadCount * granularity > pointsCount) {
        granularity = pointsCount / threadCount; // maximum granularity which makes sense
    }

    if (granularity == 0) {
        granularity = 1;
    }

    cout << "n = " << pointsCount << "; t = " << threadCount << "; g = " << granularity << endl; 
    Stopwatch stopwatch;
    Circle smallestCircle;
    if (pointsCount == 1) {
        smallestCircle = Circle(points[0], 0);
    } else {
        //  smallestCircle = findSmallestCircleST(points);
        smallestCircle = findSmallestCircleMT(points, threadCount, granularity);
    }

    double timeElapsedMS = stopwatch.ElapsedTime();
    cout << "Smallest circle: " << smallestCircle.toString() << endl;
    cout << "Found in: " << timeElapsedMS << " milliseconds" << endl;
    return 0;
}
