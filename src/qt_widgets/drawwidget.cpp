#include "drawwidget.h"

#include <QDebug>

#include <QPainter>
#include <QPaintEngine>
#include <QMouseEvent>

namespace DrawWidget
{

struct PaintConfig
{
    QWidget * pWidget {nullptr};
    QPainter * painter {nullptr};

    QBrush backgroundBrush; // Background color

    QBrush mainBrush;   // Defines main color
    QBrush secondBrush; // Defines second color

    QPen mainPen;       // For main DrawWidget
    QPen secondPen;     // For additional DrawWidget (second color)

    QFont currentFont; // For text printing

    void setupPainter() { if (painter) delete painter; painter = new QPainter(pWidget); painter->setRenderHint(QPainter::Antialiasing); }
    void start() { if (!painter->isActive()) painter->begin(pWidget); }
    void stop() { if (painter->isActive()) painter->end(); delete painter; painter = 0; }

    void fillRect(const QRect & rect, bool isBackground = false)
    {
        // Fill background
        if (isBackground)
        {
            const QBrush brushBuf = painter->brush();
            painter->setBrush(backgroundBrush);
            painter->drawRect(rect);
            painter->setBrush(brushBuf);
        }
        else
            painter->drawRect(rect);
    }

    void setDefault()
    {
        mainPen     = QPen(QColor(30, 30, 30),      3, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap);
        secondPen   = QPen(QColor(225, 225, 225),   3, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap);

        backgroundBrush = QBrush(QColor(250, 250, 250));

        mainBrush = QBrush( QColor(30, 30, 30) );
        secondBrush = QBrush( QColor(225, 225, 225) );

        currentFont.setPixelSize(15);
        currentFont.setBold(false);
    }

    void setup(const uint penType, QColor color = QColor())
    {
        if (color.isValid())
            mainPen.setColor(color);

        painter->setBrush(mainBrush);
        painter->setFont(currentFont);

        switch (penType)
        {
        case 0:
            painter->setPen(mainPen);
            break;

        case 1:
            painter->setPen(secondPen);
            break;
        }
    }

    void drawText(const QString & text, const QRect & pos)
    {
        painter->setPen(mainPen);
        painter->drawText(pos, Qt::AlignCenter, text);
    }

    void drawFigure(const QVector<Utility::Point> & points, const bool isFilled)
    {
        if (isFilled)
        {
            QVector<QPoint> equVect;
            for (auto & p : points)
                equVect.push_back( QPoint(p.x, p.y) );

            painter->drawPolygon(equVect);
        }
        else
        {
            for (int i = 0; i < points.size() - 1; i++)
                painter->drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
        }
    }
};

}

using namespace DrawWidget;

struct Utility::DrawWidget::DrawViewWidget::Impl
{
    bool drawLock {false};
    PaintConfig pconf;
    QVector<AbstractFigure *> figures;
    CustomFigure * pClickFigure;
};

Utility::DrawWidget::DrawViewWidget::DrawViewWidget(QWidget *parent) :
    QWidget(parent),
    d {new Impl}
{
    d->pconf.pWidget = this;
    d->pconf.setDefault();

    d->pClickFigure = new CustomFigure();
    d->figures.push_back(d->pClickFigure);
}

Utility::DrawWidget::DrawViewWidget::~DrawViewWidget()
{
    for (auto f : d->figures)
        delete f;
}

void Utility::DrawWidget::DrawViewWidget::addFigure(AbstractFigure *f) { d->figures.push_back(f); }
void Utility::DrawWidget::DrawViewWidget::removeFigure(AbstractFigure *f) { d->figures.removeOne(f); }
void Utility::DrawWidget::DrawViewWidget::removeFigure(int figureIndex) { if (figureIndex < d->figures.size()) d->figures.removeAt(figureIndex); }

void Utility::DrawWidget::DrawViewWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    while (d->drawLock);
    d->drawLock = true;
    d->pconf.setupPainter();
    d->pconf.start();
    drawData();
    d->pconf.stop();
    d->drawLock = false;
}

void Utility::DrawWidget::DrawViewWidget::mousePressEvent(QMouseEvent *e)
{
    while (d->drawLock);
    d->drawLock = true;
    if (e->button() == Qt::RightButton)
    {
        d->pClickFigure = new CustomFigure;
        d->figures.push_back(d->pClickFigure);
    }
    else
        d->pClickFigure->addPoint(Utility::Point(e->x(), e->y()));
    d->drawLock = false;
    repaint();
}

void Utility::DrawWidget::DrawViewWidget::drawData()
{
    d->pconf.fillRect(rect(), true);
    d->pconf.setup(0);

    for (auto * pFigure : d->figures)
        d->pconf.drawFigure(pFigure->points(), pFigure->isFilled());
}

