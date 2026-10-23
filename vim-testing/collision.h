#pragma once

namespace collision {
	double dist(double x1, double y1, double x2, double y2);

	//rcx, rcy: center of rect
	//rw, rh: size of rect
	//t: rotation of rect, in radians
	//cx, cy: center of circle
	//cr: radius of circle
	bool rotatedRectCircle(double rcx, double rcy, double rw, double rh, double t, double cx, double cy, double cr);

	bool circleCircle(double x1, double y1, double r1, double x2, double y2, double r2, double* outDist = nullptr);
};
