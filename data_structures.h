#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

struct LineCoefficients {
    double m;
    double b;
};

struct Point {
    double x;
    double y;

    Point()
        : x(0), y(0)
    {}

    Point(const double &x, const double &y)
        : x(x), y(y)
    {}

    inline double dist2(const Point & other) const {
        return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
    }

    inline double dist(const Point & other) const {
        return sqrt(dist2(other));
    }

    Point operator - (const Point & other) const{
        return Point(x - other.x, y - other.y);
    }

    Point operator + (const Point & other) const {
        return Point(x + other.x, y + other.y);
    }

    Point operator / (const int & val) const {
        return Point(x / val, y / val);
    }

    bool operator == (const Point & other) const {
        return x == other.x && y == other.y;
    }

    static vector<Point> generatePoints(const int & count) {
        // srand((unsigned) time(0));
        vector<Point> points(count);
        for (auto & point : points) {
            point.x = rand();
            point.y = rand();
        }

        return points;
    }

    static vector<Point> parseFile(const string & path) {
        fstream file(path);
        int pointsCount = 0;
        file >> pointsCount;
        vector<Point> points(pointsCount);
        for (auto & point : points) {
            file >> point.x;
            file >> point.y;
        }

        return points;
    }

    static bool areOnSameLine(const Point& A, const Point& B, const Point& C) {
        Point AB = B - A;
        Point AC = C - A;

        return (
            (AC.x == 0 && AB.x == 0 && (AC.y != 0 || AB.y == 0))
            || (AC.y == 0 && AB.y == 0 && (AC.x != 0 || AB.x == 0))
            || (AC.x != 0 && AC.y != 0 && abs((AB.x * AC.y) - (AB.y * AC.x)) < 0.00001)
        );
    }

    static LineCoefficients getLineBetweenTwoPoints(const Point& A, const Point& B) {
        LineCoefficients coefs;
        if (A.y == B.y) {
            coefs.m = 0;
            coefs.b = A.y;
        } else {
            coefs.m = (A.x - B.x)/(B.y - A.y);
            double xM = (A.x + B.x)/2;
            double yM = (A.y + B.y)/2;
            coefs.b = yM - coefs.m * xM;
        }
        return coefs;
    }
};

struct Circle {
    Point center;
    double radius;
    double radius2;

    Circle(const Point & center, double radius)
        : center(center), radius(radius), radius2(radius * radius)
    {}

    Circle()
        : center(0, 0), radius(-1), radius2(1)
    {}

    inline bool isPointInside(Point point) {
        return point.dist(center) <= radius + 0.00001;
    }

    inline bool areAllPointsInside(const vector<Point> &points) {
        for (auto & point : points) {
            if (!isPointInside(point)) {
                return false;
            }
        }

        return true;
    }

    static Circle constructFrom2Points(Point A, Point B) {
        if (A == B) {
            return Circle(A, 0);
        }

        Point O = (A + B) / 2;
        return Circle(O, O.dist(A));
    }

    static Circle constructFrom3Points(const Point& A,const Point& B,const Point& C) {

        Point O;
        LineCoefficients AB = Point::getLineBetweenTwoPoints(A,B);
        LineCoefficients AC = Point::getLineBetweenTwoPoints(A,C);

        O.x = (AB.b - AC.b)/(AC.m - AB.m);
        O.y = AB.m * O.x + AB.b;

        return Circle(O, O.dist(A));
    }

    string toString() {
        std::ostringstream stringStream;
        stringStream << "Center: (" << center.x << ", " << center.y << "), R = " << radius;
        return stringStream.str();
    }
};
