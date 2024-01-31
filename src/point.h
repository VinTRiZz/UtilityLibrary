#ifndef POINT_H
#define POINT_H

// For factorial function and Qt includes
#include "main_utils.h"

namespace Utility
{

// Class to work as points in code, partly equal QPointF
struct Point
{
    double x {0};
    double y {0};

    Point() {}
    Point(double x, double y) : x(x), y(y) {}
    Point(const Point & p) { x = p.x, y = p.y; }
    Point(Point && p) { x = p.x, y = p.y; }

#ifdef QT_CORE_LIB
    Point(const QPointF & p) { x = p.x(), y = p.y(); }
    Point(QPointF && p) { x = p.x(), y = p.y(); }
#endif // QT_CORE_LIB

    Point & operator=(const Point & p) { x = p.x, y = p.y; return *this; }
    Point & operator=(Point && p) { x = p.x, y = p.y; return *this; }
    bool operator==(const Point & p) { return (x == p.x) && (y == p.y); }
    bool operator!=(const Point & p) { return (x != p.x) || (y != p.y); }
    ~Point() {}

    std::string str(const uint strSize, const char delim) const;
    Point & fromStr(const std::string & pointStr, const char delim);

    Point offset(double xo, double yo) const { return Point(x + xo, y + yo); }

private:
    // Util
    void trimNumber(std::string & buffer, const uint strSize) const;
};


// Conversion functions
void calculateImmediate(Point & p1, Point & p2, const double radius); // Points are centers of circles with radius, writes immediate points to them
void rotatePoint(Point & p, const Point &rotateCenter, const double rotateAngleInRad); // Turns point into it's rotation equ






// Point generator functions

// Qt-based
#ifdef QT_CORE_LIB
QVector<Point> createEllypsePointsQ(const Point center, const double a, const double b, double accuracy);
QVector<Point> createRectPointsQ(const Point leftBottom, const Point rightTop);
Point calculateCenterQ(const QVector<Point> & points); // Uses average summary
#endif // QT_CORE_LIB

// std-based
std::vector<Point> createEllypsePoints(const Point center, const double a, const double b, double accuracy);
std::vector<Point> createRectPoints(const Point leftBottom, const Point rightTop);
Point calculateCenter(const std::vector<Point> & points); // Uses average summary

// Bezier function creator
double bezierBasis(long pointIndex, long numberOfPoints, double curvePos); // Index -- from 0 to numberOfPoints, curvePos -- from 0 to 1, curve position
std::vector<Point> createBezierPoints(const std::vector<Point> & basisPoints, const double accuracy);
#ifdef QT_CORE_LIB
QVector<Point> createBezierPointsQ(const QVector<Point> & basisPoints, const double accuracy);
#endif // QT_CORE_LIB







// Std using
std::string convertRectToString(const Point & leftDownCorner, const Point & rightTopCorner, const uint strSize, const char rectDelim = 0, const char pointDelim = 0);
std::pair<Point, Point> convertRectFromString(const std::string &rectStr, const char rectDelim = 0, const char pointDelim = 0);

// Qt using
std::string convertRectToStringQ(const QRectF & rect, const uint strSize, const char rectDelim = 0, const char pointDelim = 0);
QRectF convertRectFromStringQ(const std::string &rectStr, const char rectDelim = 0, const char pointDelim = 0);

}

#endif // POINT_H
