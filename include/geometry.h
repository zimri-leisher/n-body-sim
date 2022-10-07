
#ifndef N_BODY_SIM_GEOMETRY_H
#define N_BODY_SIM_GEOMETRY_H

namespace sim {
    namespace geometry {
        bool IntersectsPointRectangleInc(int px, int py, int rx1, int ry1, int rx2, int ry2);
        bool IntersectsLineLineInc(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
        bool IntersectsLineRectangleInc(int lx1, int ly1, int lx2, int ly2, int rx1, int ry1, int rx2, int ry2);
    }
}

#endif //N_BODY_SIM_GEOMETRY_H
