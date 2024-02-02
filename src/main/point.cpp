#include "point.h"
using namespace Utility;

#include <algorithm>
#include <utility>
#include <math.h>

// Qt includes
#ifdef QT_CORE_LIB
#include <QDebug>
#endif // QT_CORE_LIB

void Utility::Point::trimNumber(std::string &buffer, const uint strSize) const
{
    while (*(buffer.end() - 1) == '0')
        buffer.pop_back();
    if ((*(buffer.end() - 1) == '.') || (*(buffer.end() - 1) == ','))
        buffer.pop_back();
    while (buffer.size() < strSize)
        buffer = '0' + buffer;
    while (buffer.size() > strSize)
        buffer.erase(buffer.begin(), buffer.begin() + 1);
}

std::string Utility::Point::str(const uint strSize, const char delim) const
{
    if (strSize < 2) return "";

    std::string buffer;
    std::string result;

    double trimmedX = x, trimmedY = y;

    const long unsigned pow_ten = pow(10, strSize);
    const double maxValue = pow_ten;
    if (x > maxValue) trimmedX = maxValue; else if (x < -maxValue) trimmedX = -maxValue;
    if (y > maxValue) trimmedY = maxValue; else if (y < -maxValue) trimmedY = -maxValue;

    bool isMinus = trimmedX < 0;
    if (isMinus) trimmedX *= -1;
    buffer = std::to_string( trimmedX );
    trimNumber(buffer, strSize);
    if (isMinus) buffer[0] = '-';

    result += buffer;

    if (delim)  result += delim;

    isMinus = trimmedY < 0;
    if (isMinus) trimmedY *= -1;
    buffer = std::to_string( trimmedY );
    trimNumber(buffer, strSize);
    if (isMinus) buffer[0] = '-';

    result += buffer;

    return result;
}

Point & Utility::Point::fromStr(const std::string & pointStr, const char delim)
{
    x = y = 0;
    try {
        const uint strSize = pointStr.size() / 2;
        if (pointStr.length() < strSize)
            return *this;

        std::string buffer(pointStr.begin(), pointStr.begin() + strSize);
        x = std::stod(buffer);

        buffer = std::string(pointStr.begin() + strSize + (delim != 0), pointStr.end());
        y = std::stod(buffer);

    } catch (std::invalid_argument & ex)
    {
        qDebug() << "Got invalid string for std::stod in point string:" << pointStr.c_str();
    }

    return *this;
}

void Utility::calculateImmediate(Point &p1, Point &p2, const double radius)
{
    double dx, dy, straight, k;

    dy = p2.y - p1.y;
    dx = p2.x - p1.x;

    straight = sqrt(dy * dy + dx * dx);

    k = radius / straight;

    dy = dy * k;
    dx = dx * k;

    p1.x += dx; p1.y += dy;
    p2.x -= dx; p2.y -= dy;
}

void Utility::rotatePoint(Point &p, const Point & rotateCenter, const double rotateAngleInRad)
{
    Point bufferPoint = p;

    p.x = (bufferPoint.x - rotateCenter.x) * cos(rotateAngleInRad) - (bufferPoint.y - rotateCenter.y) * sin(rotateAngleInRad) + rotateCenter.x;
    p.y = (bufferPoint.x - rotateCenter.x) * sin(rotateAngleInRad) + (bufferPoint.y - rotateCenter.y) * cos(rotateAngleInRad) + rotateCenter.y;
}

QVector<Point> Utility::createEllypsePointsQ(const Point center, const double a, const double b, double accuracy)
{
    QVector<Point> pointsBuffer;

    // Ellypse formula:
    // y = (b / a) * \/(a^2 - x^2)
    const double del_ab = a / b;
    const double a_pow2 = a * a;

    for (double i = 0; i <= a; i+= accuracy)
    {
        Point anotherPoint;
        anotherPoint.x = center.x + i;
        anotherPoint.y = center.y + del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.push_front(anotherPoint);

        anotherPoint.x = center.x - i;
        anotherPoint.y = center.y + del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.push_back(anotherPoint);
    }

    for (double i = a; i >= 0; i-= accuracy)
    {
        Point anotherPoint;
        anotherPoint.x = center.x - i;
        anotherPoint.y = center.y - del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.push_back(anotherPoint);

        anotherPoint.x = center.x + i;
        anotherPoint.y = center.y - del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.push_front(anotherPoint);
    }
    return pointsBuffer;
}

QVector<Point> Utility::createRectPointsQ(const Point leftBottom, const Point rightTop)
{
    QVector<Point> rectPoints;

    // Start Point
    rectPoints.push_back(leftBottom);

    // First Point
    Point bufferPoint;
    bufferPoint.x = rightTop.x;
    bufferPoint.y = leftBottom.y;
    rectPoints.push_back(bufferPoint);

    // Second Point
    rectPoints.push_back(rightTop);

    // Last Point
    bufferPoint.x = leftBottom.x;
    bufferPoint.y = rightTop.y;
    rectPoints.push_back(bufferPoint);

    // Zamkn
    rectPoints.push_back(leftBottom);

    return rectPoints;
}

std::vector<Point> createEllypsePoints(const Point center, const double a, const double b, double accuracy)
{
    std::vector<Point> pointsBuffer;

    // Ellypse formula:
    // y = (b / a) * \/(a^2 - x^2)
    const double del_ab = a / b;
    const double a_pow2 = a * a;

    for (double i = 0; i <= a; i+= accuracy)
    {
        Point anotherPoint;
        anotherPoint.x = center.x + i;
        anotherPoint.y = center.y + del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.insert(pointsBuffer.begin(), anotherPoint);

        anotherPoint.x = center.x - i;
        anotherPoint.y = center.y + del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.push_back(anotherPoint);
    }

    for (double i = a; i >= 0; i-= accuracy)
    {
        Point anotherPoint;
        anotherPoint.x = center.x - i;
        anotherPoint.y = center.y - del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.push_back(anotherPoint);

        anotherPoint.x = center.x + i;
        anotherPoint.y = center.y - del_ab * sqrt(abs(a_pow2 - i * i));
        pointsBuffer.insert(pointsBuffer.begin(), anotherPoint);
    }
    return pointsBuffer;
}

std::vector<Point> createRectPoints(const Point leftBottom, const Point rightTop)
{
    std::vector<Point> rectPoints;

    // Start Point
    rectPoints.push_back(leftBottom);

    // First Point
    Point bufferPoint;
    bufferPoint.x = rightTop.x;
    bufferPoint.y = leftBottom.y;
    rectPoints.push_back(bufferPoint);

    // Second Point
    rectPoints.push_back(rightTop);

    // Last Point
    bufferPoint.x = leftBottom.x;
    bufferPoint.y = rightTop.y;
    rectPoints.push_back(bufferPoint);

    // Zamkn
    rectPoints.push_back(leftBottom);

    return rectPoints;
}

Point Utility::calculateCenterQ(const QVector<Point> &points)
{
    Point buffer;

    // Calculate, if all's good, return result
    const int vectSize = points.size();
    for (const Point & p : points)
    {
        buffer.x += p.x / vectSize;
        buffer.y += p.y / vectSize;
    }
    return buffer;
}

Point calculateCenter(const std::vector<Point> &points)
{
    Point buffer;

    // Calculate, if all's good, return result
    const int vectSize = points.size();
    for (const Point & p : points)
    {
        buffer.x += p.x / vectSize;
        buffer.y += p.y / vectSize;
    }
    return buffer;
}

double Utility::bezierBasis(long pointIndex, long numberOfPoints, double curvePos)
{
    //      n!
    // ---------- * t ^ i * (1 - t) ^ (n - i)
    // i!(n - i)!

    numberOfPoints--;

    double powResult_1 = 1, powResult_2 = 1;

    for (uint i = 0; i < pointIndex; i++)
        powResult_1 *= curvePos;

    const double oneMinusCurvePos = 1.0 - curvePos;
    for (uint i = 0; i < (numberOfPoints - pointIndex); i++)
        powResult_1 *= oneMinusCurvePos;

    if ((pointIndex == 0) || (numberOfPoints == pointIndex))
        return powResult_1 * powResult_2;

    return (factorial(numberOfPoints) / (factorial(pointIndex) * factorial(numberOfPoints - pointIndex))) * powResult_1 * powResult_2;
}

#ifdef QT_CORE_LIB
QVector<Point> Utility::createBezierPointsQ(const QVector<Point> &basisPoints, const double accuracy)
{
    if (accuracy <= 0)
        return QVector<Point>();

    QVector<Point> result;
    double base;

    Point buffer;
    for (double t = accuracy; t <= 1; t+= accuracy)
    {
        buffer = Point();
        for (int i = 0; i < basisPoints.size(); i++)
        {
            base = bezierBasis(i, basisPoints.size(), t);
            buffer.x += basisPoints[i].x * base;
            buffer.y += basisPoints[i].y * base;
        }
        result.push_back(buffer);
    }

    return result;
}
#endif // QT_CORE_LIB

std::vector<Point> createBezierPoints(const std::vector<Point> &basisPoints, const double accuracy)
{
    if (accuracy <= 0)
        return std::vector<Point>();

    std::vector<Point> result;
    double base;

    Point buffer;
    for (double t = accuracy; t <= 1; t+= accuracy)
    {
        buffer = Point();
        for (size_t i = 0; i < basisPoints.size(); i++)
        {
            base = bezierBasis(i, basisPoints.size(), t);
            buffer.x += basisPoints[i].x * base;
            buffer.y += basisPoints[i].y * base;
        }
        result.push_back(buffer);
    }

    return result;
}





std::string Utility::convertRectToString(const Point & leftDownCorner, const Point & rightTopCorner, const uint strSize, const char rectDelim, const char pointDelim)
{
    std::string result;
    result += leftDownCorner.str(strSize, pointDelim);
    result += rectDelim;
    result += rightTopCorner.str(strSize, pointDelim);
    return result;
}

std::pair<Point, Point> Utility::convertRectFromString(const std::string &rectStr, const char rectDelim, const char pointDelim)
{
    Point p1, p2;
    if (!rectStr.size())
        return std::make_pair(p1, p2);
    p1.fromStr(std::string(rectStr.begin(), rectStr.begin() + rectStr.size() / 2), pointDelim);
    p2.fromStr(std::string(rectStr.begin() + rectStr.size() / 2 + 1 + (rectDelim != 0), rectStr.end()), pointDelim);
    return std::make_pair(p1, p2);
}

#ifdef QT_CORE_LIB
std::string Utility::convertRectToStringQ(const QRectF & rect, const uint strSize, const char rectDelim, const char pointDelim)
{
    std::string result;
    Point p1(rect.left(), rect.bottom()), p2(rect.right(), rect.top());
    result += p1.str(strSize, pointDelim);
    result += rectDelim;
    result += p2.str(strSize, pointDelim);
    return result;
}

QRectF Utility::convertRectFromStringQ(const std::string &rectStr, const char rectDelim, const char pointDelim)
{
    auto rectPair = convertRectFromString(rectStr, rectDelim, pointDelim);
    return QRectF(rectPair.second.x, rectPair.first.y, rectPair.first.x - rectPair.second.x, rectPair.second.y - rectPair.first.y);
}
#endif // QT_CORE_LIB
