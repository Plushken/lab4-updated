#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::green);
    darkPalette.setColor(QPalette::ToolTipText, Qt::green);
    darkPalette.setColor(QPalette::Text, Qt::green);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::green);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::red);
    qApp->setPalette(darkPalette);

    currentGraph = -1;

    ui->widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->widget->setInteraction(QCP::iRangeDrag,true);
    ui->widget->setInteraction(QCP::iRangeZoom,true);
    ui->widget->xAxis->setRange(-1000, 1000);
    ui->widget->yAxis->setRange(-1000, 1000);

    QFont font;
    font.setBold(true);

    QValidator *coordValid = new QRegularExpressionValidator(QRegularExpression("^([-]?[1-9][0-9]*)|(0)$"));
    QValidator *radValid = new QRegularExpressionValidator(QRegularExpression("^([1-9][0-9]*)$"));

    ui->lineEdit->setValidator(coordValid);
    ui->lineEdit_2->setValidator(coordValid);
    ui->lineEdit_3->setValidator(coordValid);
    ui->lineEdit_4->setValidator(coordValid);
    ui->lineEdit_5->setValidator(coordValid);
    ui->lineEdit_6->setValidator(coordValid);

    ui->lineEdit_7->setValidator(radValid);

    connect(ui->radioButton, &QRadioButton::clicked, this, &Widget::  lineData);
    connect(ui->radioButton_2, &QRadioButton::clicked, this, &Widget::lineData);
    connect(ui->radioButton_3, &QRadioButton::clicked, this, &Widget::lineData);
    connect(ui->radioButton_4, &QRadioButton::clicked, this, &Widget::lineData);
    connect(ui->radioButton_5, &QRadioButton::clicked, this, &Widget::circleData);

    ui->radioButton->setChecked(true);
    emit ui->radioButton->clicked();

    connect(ui->pushButton_2, &QPushButton::clicked, this, &Widget::clearAllGraphics);

    connect(ui->pushButton, &QPushButton::clicked, this, &Widget::drawPlot);

    ui->lineEdit->setText("0");
    ui->lineEdit_2->setText("400");
    ui->lineEdit_3->setText("-800");
    ui->lineEdit_4->setText("450");
    ui->lineEdit_5->setText("-40");
    ui->lineEdit_6->setText("80");
    ui->lineEdit_7->setText("250");

}

Widget::~Widget()
{
    delete ui;
}

void Widget::lineOrCircleDataVisible(bool line)
{
    ui->lineEdit->setReadOnly(!line);
    ui->lineEdit_2->setReadOnly(!line);
    ui->lineEdit_3->setReadOnly(!line);
    ui->lineEdit_4->setReadOnly(!line);

    ui->lineEdit_5->setReadOnly(line);
    ui->lineEdit_6->setReadOnly(line);
    ui->lineEdit_7->setReadOnly(line);

}

void Widget::lineData()
{
    lineOrCircleDataVisible(true);
}

void Widget::circleData()
{
    lineOrCircleDataVisible(false);
}

void Widget::clearAllGraphics()
{
    ui->widget->clearGraphs();
    currentGraph = -1;
    ui->widget->replot();
}

void Widget::drawPlot()
{
    int x0 = ui->lineEdit->text().toInt();
    int x1 = ui->lineEdit_4->text().toInt();
    int y0 = ui->lineEdit_2->text().toInt();
    int y1 = ui->lineEdit_3->text().toInt();
    int x = ui->lineEdit_5->text().toInt();
    int y = ui->lineEdit_6->text().toInt();
    int rad = ui->lineEdit_7->text().toInt();
    currentGraph++;
    ui->widget->addGraph();
    ui->widget->graph(currentGraph)->setAntialiased(true);

    if (ui->radioButton_5->isChecked())
    {
        bresenhamCircleAlgorithm(x, y, rad);
    }
    else
    {
        if (ui->radioButton->isChecked())
            stepAlgorithm(x0, y0, x1, y1);
        else if (ui->radioButton_2->isChecked())
            ddaAlgorithm(x0, y0, x1, y1);
        else if (ui->radioButton_3->isChecked())
            bresenhamLineAlgorithm(x0, y0, x1, y1);
        else
            castlePitwayAlgorithm(x0, y0, x1, y1);
    }
}

void Widget::stepAlgorithm(int x0, int y0, int x1, int y1)
{
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    if (x0 == x1)
    {
        ui->widget->graph(currentGraph)->addData(x0, y0);
        ui->widget->graph(currentGraph)->addData(x1, y1);
        ui->widget->replot();
    }
    else
    {
        double k = (double)(y1-y0)/(double)(x1-x0);
        double b = y0 - k * x0;
        ui->widget->graph(currentGraph)->addData(x0, y0);
        int x = x0;
        int y = y0;
        while(x < x1)
        {
            x++;
            y=round(k * x + b);
            ui->widget->graph(currentGraph)->addData(x, y);
            ui->widget->replot();
        }
    }
}

void Widget::ddaAlgorithm(int x0, int y0, int x1, int y1)
{
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float x = x0;
    float y = y0;
    float dx = x1 - x0;
    float dy = y1 - y0;
    float l = std::max(dx, dy);
    ui->widget->graph(currentGraph)->addData(x0, y0);
    int i = 0;
    while (i < l)
    {
        y += dy/l;
        x += dx/l;
        ui->widget->graph(currentGraph)->addData(trunc(x), trunc(y));
        ui->widget->replot();
        ++i;
    }
}

void Widget::bresenhamLineAlgorithm(int x0, int y0, int x1, int y1)
{
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        ui->widget->graph(currentGraph)->addData(steep ? y : x, steep ? x : y);
        ui->widget->replot();
        error -= dy;
        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

void Widget::bresenhamCircleAlgorithm(int x0, int y0, int rad)
{
    if (rad == 1)
    {
        ui->widget->addGraph();
        ui->widget->addGraph();
        ui->widget->addGraph();

        ui->widget->graph(currentGraph)->addData (x0 + 1, y0);
        ui->widget->graph(currentGraph)->addData (x0, y0 + 1);
        ui->widget->graph(currentGraph+1)->addData (x0, y0 + 1);
        ui->widget->graph(currentGraph+1)->addData (x0 - 1, y0);
        ui->widget->graph(currentGraph+2)->addData (x0 - 1, y0);
        ui->widget->graph(currentGraph+2)->addData (x0, y0 - 1);
        ui->widget->graph(currentGraph+3)->addData (x0, y0 - 1);
        ui->widget->graph(currentGraph+3)->addData (x0 + 1, y0);
        ui->widget->replot();
        currentGraph += 3;
        return;
    }

    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget->addGraph();

    int x = rad;
    int y = 0;
    int radiusError = 1 - x;

    ui->widget->graph(currentGraph+1)->addData(y + x0, x + y0);
    ui->widget->graph(currentGraph+2)->addData(-x + x0, y + y0);
    ui->widget->graph(currentGraph+4)->addData(-x + x0, -y + y0);
    ui->widget->graph(currentGraph+7)->addData(y + x0, -x + y0);

    xCoord.push_back(x);
    yCoord.push_back(y);

    while (x >= y)
    {
        y++;
        if (radiusError < 0)
        {
            radiusError += 2 * y + 1;
        }
        else
        {
            x--;
            radiusError += 2 * (y - x + 1);
        }
        ui->widget->graph(currentGraph+1)->addData(y + x0, x + y0);
        ui->widget->graph(currentGraph+2)->addData(-x + x0, y + y0);
        ui->widget->graph(currentGraph+4)->addData(-x + x0, -y + y0);
        ui->widget->graph(currentGraph+7)->addData(y + x0, -x + y0);
        ui->widget->replot();

        xCoord.push_back(x);
        yCoord.push_back(y);
    }

    for(int i = xCoord.size() - 1; i >= 0; i--)
    {
        ui->widget->graph(currentGraph)->addData(xCoord[i] + x0, yCoord[i] + y0);
        ui->widget->graph(currentGraph+3)->addData(-yCoord[i] + x0, xCoord[i]  + y0);
        ui->widget->graph(currentGraph+5)->addData(-yCoord[i] + x0, -xCoord[i]  + y0);
        ui->widget->graph(currentGraph+6)->addData(xCoord[i]  + x0, -yCoord[i] + y0);
        ui->widget->replot();
    }

    currentGraph+=7;
    xCoord.clear();
    yCoord.clear();
}

void Widget::castlePitwayAlgorithm(int x0, int y0, int x1, int y1)
{
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    bool inverse = x0 < x1 && y0 > y1 ? true : false;
    int x = abs(abs(x1 - x0) - abs(y1 - y0));
    int y = abs(y1 - y0);
    if (x == 0)
    {
        bool up = y1 - y0 > 0;
        x = x0;
        y = y0;
        ui->widget->graph(currentGraph)->addData(steep ? y : x, steep ? x : y);
        while (x < x1)
        {
            x++;
            if (up)
                y++;
            else
                y--;
            ui->widget->graph(currentGraph)->addData(steep ? y : x, steep ? x : y);
            ui->widget->replot();
        }
        return;
    }

    else if (y == 0)
    {
        x = x0;
        y = y0;
        ui->widget->graph(currentGraph)->addData(steep ? y : x, steep ? x : y);
        while (x < x1)
        {
            x++;
            ui->widget->graph(currentGraph)->addData(steep ? y : x, steep ? x : y);
            ui->widget->replot();
        }
        return;
    }

    QVector<char>m1, m2, temp;
    m1.push_back('s');
    m2.push_back('d');
    while(x != y)
    {
        if (x > y)
        {
            x -= y;
            temp = m2;
            std::reverse(temp.begin(), temp.end());
            m2 = m1 + temp;
        }
        else
        {
            y -= x;
            temp = m1;
            std::reverse(temp.begin(), temp.end());
            m1 = m2 + temp;
        }
    }

    std::reverse(m1.begin(), m1.end());
    temp = m2 + m1;

    int steps = x;
    x = x0;
    y = y0;

    ui->widget->graph(currentGraph)->addData(steep ? y0 : x0, steep ? x0 : y0);
    while (steps > 0)
    {
        for (int i = 0; i < temp.size(); i++)
        {
            if (temp[i] == 's')
                x++;
            else
            {
                x++;
                if(inverse)
                    y--;
                else
                    y++;
            }
            ui->widget->graph(currentGraph)->addData(steep ? y : x, steep ? x : y);
            ui->widget->replot();
        }
        steps--;
    }
}



