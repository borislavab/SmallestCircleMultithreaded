#pragma once

#include "data_structures.h"

Circle findSmallestCircleST(const vector<Point> & points);
Circle findSmallestCircleMT(const vector<Point> & points, int threadCount, int granularity);
