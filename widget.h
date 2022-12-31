#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    int lastX0;
    int lastY0;
    int lastX1;
    int lastY1;
    int lastR;

    int currentGraph;

    QVector<int> xCoord;
    QVector<int> yCoord;

    void lineOrCircleDataVisible(bool line);

    void stepAlgorithm(int x0, int y0, int x1, int y1);
    void ddaAlgorithm(int x0, int y0, int x1, int y1);
    void bresenhamLineAlgorithm(int x0, int y0, int x1, int y1);
    void bresenhamCircleAlgorithm(int x0, int y0, int rad);
    void castlePitwayAlgorithm(int x0, int y0, int x1, int y1);

private slots:
    void lineData();
    void circleData();

    void clearAllGraphics();

    void drawPlot();
};
#endif // WIDGET_H
