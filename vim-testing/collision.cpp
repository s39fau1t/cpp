#include "collision.h"

#include <cmath>

namespace collision {
	namespace {
		double closest(double min, double max, double x) {
			double closest;

			if(x < min) {
				closest = min;
			}else if(x > max) {
				closest = max;
			}else {
				closest = x;
			}

			return closest;
		}
	}

	double dist(double x1, double y1, double x2, double y2) {
		return std::hypot(x1 - x2, y1 - y2);
	}

	//http://www.migapro.com/circle-and-rotated-rectangle-collision-detection/
	bool rotatedRectCircle(double rcx, double rcy, double rw, double rh, double t, double cx, double cy, double cr) {
		//Rotate the circle to where it would be if t = 0
		double unrotatedCx = std::cos(t) * (cx - rcx) - std::sin(t) * (cy - rcy) + rcx;
		double unrotatedCy = std::sin(t) * (cx - rcx) + std::cos(t) * (cy - rcy) + rcy;

		//Now we can find x and y of rect because t = 0
		double rx = rcx - rw / 2;
		double ry = rcy - rh / 2;

		//Closest point on the rect
		double closestX = closest(rx, rx + rw, unrotatedCx);
		double closestY = closest(ry, ry + rh, unrotatedCy);

		//Now just circle/point
		return dist(unrotatedCx, unrotatedCy, closestX, closestY) < cr;
	}

	bool circleCircle(double x1, double y1, double r1, double x2, double y2, double r2, double* outDist) {
		double distance = dist(x1, y1, x2, y2);

		if(outDist != nullptr) {
			*outDist = distance;
		}

		return distance < r1 + r2;
	}
};
