#ifndef DEPENDENCYGRAPHWIDGET_H
#define DEPENDENCYGRAPHWIDGET_H

#include <QWidget>
#include <memory>

#include "figures.h"
#include "point.h"

namespace Utility
{

namespace DrawWidget
{

class DrawViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawViewWidget(QWidget *parent = 0);
    ~DrawViewWidget();

    void addFigure(AbstractFigure * f);
    void removeFigure(AbstractFigure * f);
    void removeFigure(int figureIndex);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void paintEvent(QPaintEvent * e);
    void mousePressEvent(QMouseEvent * e);

    void drawData();
};

}

}

#endif // DEPENDENCYGRAPHWIDGET_H
