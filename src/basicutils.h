#ifndef BASICUTILS_H
#define BASICUTILS_H

// STD includes
#include <string>
#include <fstream>

// Qt includes
#ifdef QT_CORE_LIB
#include <QString>
#include <QPoint>
#include <QVector>
#include <QFile>
#include <QRectF>
#endif // QT_CORE_LIB

// Defines struct Point and removes depending functions if undefined
#define BU_POINT_FUNCTIONALITY

// Point struct generation functions, like createEllypsePoints(), used for drawing
#define BU_POINT_GENERATION_FUNCTIONS

// Conversions to std::string like Point(x==19.35, y==91,33) ---> "16,35_91,33"
// and back to Point struct
#define BU_POINT_TO_STRING_CONVERTION

// Process invoking and gathering it's output by one function
#define BU_PROCESS_INVOKING

// Save and load functions with and without Qt file
#define BU_FILE_READ_WRITE


namespace Utility
{

// Etc functions
uint factorial(long n);






#ifdef BU_POINT_FUNCTIONALITY
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
#ifdef BU_POINT_GENERATION_FUNCTIONS

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

#endif // BU_FUNCTION_POINT_GENERATION







#ifdef BU_POINT_TO_STRING_CONVERTION
// Std using
std::string convertRectToString(const Point & leftDownCorner, const Point & rightTopCorner, const uint strSize, const char rectDelim = 0, const char pointDelim = 0);
std::pair<Point, Point> convertRectFromString(const std::string &rectStr, const char rectDelim = 0, const char pointDelim = 0);

// Qt using
std::string convertRectToStringQ(const QRectF & rect, const uint strSize, const char rectDelim = 0, const char pointDelim = 0);
QRectF convertRectFromStringQ(const std::string &rectStr, const char rectDelim = 0, const char pointDelim = 0);
#endif // BU_POINT_TO_STRING_CONVERTION

#endif // BU_POINT_FUNCTIONALITY






#ifdef BU_PROCESS_INVOKING
// Defines time before invoke function stops
const int PROCESS_START_TIMEOUT {1000};

#ifdef QT_CORE_LIB
bool invokeQ(const QString & program, const QStringList & args, const int timeout, QString * output = nullptr, QString * errorOutput = nullptr);
#endif // QT_CORE_LIB
#endif // BU_PROCESS_INVOKING





#ifdef BU_FILE_READ_WRITE
// File read/write functions for fstream, can open files immediately
const size_t FILE_READ_BUFFER_SIZE = 64;
bool saveData(const std::string & filename, const std::string & dataBuf);
bool saveData(std::fstream * of, const std::string & dataBuf);
bool loadData(const std::string & filename, std::string & dataBuf);
bool loadData(std::fstream * f, std::string & dataBuf);

bool saveDataQ(const QString & filename, const QString & dataBuf);
bool saveDataQ(QFile * of, const QString & dataBuf);
bool loadDataQ(const QString & filename, QString & dataBuf);
bool loadDataQ(QFile * f, QString & dataBuf);
#endif // BU_FILE_READ_WRITE








}
#endif // BASICUTILS_H
