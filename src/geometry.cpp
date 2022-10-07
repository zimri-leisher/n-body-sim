#include "geometry.h"

bool sim::geometry::IntersectsPointRectangleInc(int px, int py, int rx1, int ry1, int rx2, int ry2) {
    if (rx1 > rx2) {
        if (px < rx2 || px > rx1) {
            return false;
        }
    }
    if (px < rx1 || px > rx2) {
        return false;
    }
    if (ry1 > ry2) {
        if (py < ry2 || py > ry1) {
            return false;
        }
    }
    if (py < ry1 || py > ry2) {
        return false;
    }
    return true;
}

bool sim::geometry::IntersectsLineLineInc(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {

    return false;
}

bool sim::geometry::IntersectsLineRectangleInc(int lx1, int ly1, int lx2, int ly2, int rx1, int ry1, int rx2, int ry2) {
    if(IntersectsPointRectangleInc(lx1, ly1, rx1, ry1, rx2, ry2)) {
        return true;
    }
    if(IntersectsPointRectangleInc(lx2, ly2, rx1, ry1, rx2, ry2)) {
        return true;
    }
    float m = float(ly2 - ly1) / float(lx2 - lx1);
    int lineYAtRectX1 = int(m * float(rx1)) + ly1;
    int lineYAtRectX2 = int(m * float(rx2)) + ly1;
    if(lineYAtRectX1 < ry1 || lineYAtRectX1 > ry2) {
        return false;
    }
    if(lineYAtRectX2 < ry1 || lineYAtRectX2 > ry2) {
        return false;
    }
    return false;
}
