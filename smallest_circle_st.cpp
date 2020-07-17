#include "smallest_circle.h"

Circle findSmallestCircleST(const vector<Point> & points) {

    Point A = points[0], B = points[1];
    double distance = A.dist(B);
    for (int i = 0; i < points.size(); ++i) {
        for (int j = i + 1; j < points.size(); ++j) {
            const Point &currentA = points[i];
            const Point &currentB = points[j];
            double currentDistance = currentA.dist(currentB);
            if (distance < currentDistance) {
                distance = currentDistance;
                A = currentA;
                B = currentB;
            }
        }
    }

    Circle smallestCircle2Points = Circle::constructFrom2Points(A, B);
    bool areAllPointsInside = smallestCircle2Points.areAllPointsInside(points);
    if (areAllPointsInside) {
        return smallestCircle2Points;
    }

    Circle smallestCircle3Points;
    for (int i = 0; i < points.size(); ++i) {
        for (int j = i + 1; j < points.size(); ++j) {
            for (int k = j + 1; k < points.size(); ++k) {
                const Point &currentA = points[i];
                const Point &currentB = points[j];
                const Point &currentC = points[k];
                if (Point::areOnSameLine(currentA, currentB, currentC)) {
                    continue;
                }
                Circle currentCircle = Circle::constructFrom3Points(currentA, currentB, currentC);
                if (smallestCircle3Points.radius >= 0 && smallestCircle3Points.radius <= currentCircle.radius) {
                    continue;
                }
                areAllPointsInside = currentCircle.areAllPointsInside(points);
                if (areAllPointsInside) {
                    smallestCircle3Points = currentCircle;
                }
            }
        }
    }

    return smallestCircle3Points;
}
