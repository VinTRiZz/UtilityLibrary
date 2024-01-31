#include "figures.h"

using namespace Utility;


void CustomFigure::setPoints(const QVector<Utility::Point> &pointVect) { m_points = pointVect; }
void CustomFigure::setup() { /* Does nothing because setPoints is used to setup actually */ }

const QVector<Utility::Point> &BezierCustomFigure::points()
{
    QVector<Utility::Point> bezierPart;
    for (int i = 0; i < m_points.size(); i+=3)
    {
        for (int j = 0; ((i + j) < m_points.size()) && (j < 4); j++)
            bezierPart.push_back(m_points[i + j]);

        // Get parts
        bezierPart = Utility::createBezierPointsQ(bezierPart, m_accuracy);

        // Copy to buffer
        for (auto & p : bezierPart)
            buffer.push_back(p);
        bezierPart.clear();
    }
    return buffer;
}
void BezierCustomFigure::setPoints(const QVector<Utility::Point> &pointVect) { m_points = pointVect; }
void BezierCustomFigure::setup() {  }


void Rectangle::configure(const Utility::Point &leftBottom, const Utility::Point &rightTop) { m_leftBottom = leftBottom; m_rightTop = rightTop; }
void Rectangle::configure(float x1, float y1, float x2, float y2) { m_leftBottom.x = x1; m_leftBottom.y = y1; m_rightTop.x = x2; m_rightTop.y = y2; }
void Rectangle::setup() { m_points = Utility::createRectPointsQ(m_leftBottom, m_rightTop); }


void Circle::configure(const double radius, const Utility::Point &center) { m_radius = radius; m_center = center; }
void Circle::configure(const double radius, const float x, const float y) { m_radius = radius; m_center.x = x; m_center.y = y; }
void Circle::setup() { m_points = Utility::createEllypsePointsQ(m_center, m_radius, m_radius, m_accuracy); }


void Ellypse::configure(const double a, const double b, const Utility::Point &center) { m_a = a; m_b = b; m_center = center; }
void Ellypse::configure(const double a, const double b, const float x, const float y) { m_a = a; m_b = b; m_center.x = x; m_center.y = y; }
void Ellypse::setup() { m_points = Utility::createEllypsePointsQ(m_center, m_a, m_b, m_accuracy); }
