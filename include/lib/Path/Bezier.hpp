#include "DiscretePath.hpp"

class CubicBezier{
    public:
    struct Knot{
        Knot(okapi::QLength iX, okapi::QLength iY, okapi::QAngle iAngle, okapi::QLength iMagnitude){
            x = iX, y = iY, angle = iAngle, magnitude = iMagnitude;
        }

        okapi::QLength x;
        okapi::QLength y;
        okapi::QAngle angle;
        okapi::QLength magnitude;
    };

    CubicBezier(std::vector<Knot>& iWaypoint){
        for(int i = 0; i < iWaypoint.size()-1; i++){
            Point point1(iWaypoint[i].x, iWaypoint[i].y);
            Point point2(iWaypoint[i+1].x, iWaypoint[i+1].y);
            p1.push_back(point1);
            p2.push_back(point2);
            c1.push_back(point1 + Point(iWaypoint[i].magnitude, Rotation(iWaypoint[i].angle)));
            c2.push_back(point2 + Point(iWaypoint[i+1].magnitude, Rotation(iWaypoint[i+1].angle-M_PI * okapi::radian)));
        }
    }   

    Point getPoint(double iT){
        int index = int(iT * p1.size());
        double t = iT * p1.size() - index;
        if(iT == 1){
            index = p1.size() - 1;
            t = 1;
        }
        return p1[index] * pow(1-t, 3) + c1[index] * 3 * pow(1-t, 2) * t + c2[index] * 3 * (1-t) * pow(t, 2) + p2[index] * pow(t, 3);
    }

    DiscretePath generate(int iStep, bool iEnd = true){
        std::vector<Point> ret;
        double inc = 1 / iStep;

        for(int i = 0; i < iStep; i++){
            double t = i * inc;
            ret.push_back(getPoint(t));
        }

        if(iEnd){
            ret.push_back(getPoint(1));
        }

        return DiscretePath(ret);
    }

    DiscretePath generate(okapi::QLength iStep, bool iEnd = true){
        std::vector<Point> ret;
        okapi::QLength totalDist = 0 * okapi::meter;

        for(double t = 0; t < 1; t += 0.01){
            totalDist += getPoint(t).distTo(getPoint(t + 0.01));
        }

        okapi::QLength distPerSegment = totalDist / ceil((totalDist / iStep).convert(okapi::number));
        okapi::QLength traversed = 0 * okapi::meter;
        ret.push_back(getPoint(0));

        for(double t = 0; t < 1; t += 0.001){
            traversed += getPoint(t).distTo(getPoint(t + 0.001));
            if(traversed >= distPerSegment){
                traversed = 0 * okapi::meter;
                ret.push_back(getPoint(t));
            }
        }

        if(ret.back().distTo(getPoint(1)) < distPerSegment / 2){
            ret.pop_back();
        }

        if(iEnd){
            ret.push_back(getPoint(1));
        }

        return DiscretePath(ret);
    }

    private:
    std::vector<Point> p1;
    std::vector<Point> p2;
    std::vector<Point> c1;
    std::vector<Point> c2;
};