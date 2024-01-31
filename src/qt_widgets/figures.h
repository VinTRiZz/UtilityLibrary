#ifndef FIGURES_H
#define FIGURES_H

#include "point.h"

namespace Utility
{

class AbstractFigure
{
public:
    AbstractFigure() {}
    virtual ~AbstractFigure() {}

    virtual void addPoint(const Point p) { m_points.push_back(p); }
    virtual const QVector<Point> & points() { if (!m_points.size()) setup(); return m_points; }
    auto pointCount() const { return m_points.size(); }
    void resetPoints() { m_points.clear(); }

    // In figures all angles used in degrees
    void setRotationAngle(float degree) { m_angle = degree; }
    float rotationAngleD() const { return m_angle; }

    bool isFilled() const { return m_filled; }
    void setFilled(bool t = true) { m_filled = t; }

    double accuracy() const { return m_accuracy; }
    void setAccuracy(double a) { m_accuracy = a; }

protected:
    QVector<Point> m_points;

    float m_angle {0};
    bool m_filled {false};
    double m_accuracy {0.01};

    // Defined in child
    virtual void setup() = 0;
};


class CustomFigure : public AbstractFigure
{
public:
    CustomFigure() = default;
    ~CustomFigure() = default;

    void setPoints(const QVector<Point> & pointVect);

private:
    void setup() override;
};


class BezierCustomFigure : public AbstractFigure
{
public:
    BezierCustomFigure() = default;
    ~BezierCustomFigure() = default;

    const QVector<Point> & points() override;
    void setPoints(const QVector<Point> &pointVect);

private:
    void setup() override;
    QVector<Point> buffer;
};


class Rectangle : public AbstractFigure
{
public:
    Rectangle() = default;
    ~Rectangle() = default;

    void configure(const Point & leftBottom, const Point & rightTop);
    void configure(float x1, float y1, float x2, float y2);

protected:
    void setup() override;
    Point m_leftBottom;
    Point m_rightTop;
};


class Circle : public AbstractFigure
{
public:
    Circle() = default;
    ~Circle() = default;

    void configure(const double radius, const Point & center);
    void configure(const double radius, const float x, const float y);

private:
    void setup() override;
    double m_radius {0};
    Point m_center;
};


class Ellypse : public AbstractFigure
{
public:
    Ellypse() = default;
    ~Ellypse() = default;

    void configure(const double a, const double b, const Point & center);
    void configure(const double a, const double b, const float x, const float y);

private:
    void setup() override;
    double m_a {0};
    double m_b {0};
    Point m_center;
};

}

#endif // FIGURES_H
