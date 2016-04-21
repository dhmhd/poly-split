#include "renderarea.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

#include <cstdio>
#include <cfloat>

#include <line.h>
#include <vector.h>
#include <polygon.h>

std::vector<Polygon> polygons;
std::vector<QColor> polygons_colors;

double squareToCut;
int selectedPolygon;

int showInfo = 0;
int showHelp = 1;

double scale = 1;
double offset_x = 0;
double offset_y = 0;

int mouseLeftPress = 0;
int mouse_x = 0;
int mouse_y = 0;
Vector mouse;

double pointSize = 5;

int selectedPoint = -1;


void drawPoly(QPainter &painter, const Polygon &poly)
{
    QPolygonF polypon;
    for(unsigned i = 0; i < poly.size(); i++)
    {
        polypon << QPointF(poly[i].x, poly[i].y);
    }

    painter.drawPolygon(polypon);
}


RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

    polygons_colors.push_back(Qt::darkRed);
    polygons_colors.push_back(Qt::green);
    polygons_colors.push_back(Qt::darkGreen);
    polygons_colors.push_back(Qt::blue);
    polygons_colors.push_back(Qt::darkBlue);
    polygons_colors.push_back(Qt::cyan);
    polygons_colors.push_back(Qt::darkCyan);
    polygons_colors.push_back(Qt::magenta);
    polygons_colors.push_back(Qt::darkMagenta);
    polygons_colors.push_back(Qt::darkYellow);
    polygons_colors.push_back(Qt::gray);
    polygons_colors.push_back(Qt::darkGray);

    initPolygons();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    if(showHelp)
    {
        int y = 0, dy = painter.fontMetrics().height() + 1.0;
        painter.drawText(10, y += dy, "Cut area square: " + QString::number(squareToCut));
        painter.drawText(10, y += dy, "Q/W - increase/decrease cut area square to 100");
        painter.drawText(10, y += dy, "q/w - increase/decrease cut area square to 10");
        painter.drawText(10, y += dy, "a/s - swith between areas");
        painter.drawText(10, y += dy, "r - to restore initial polygon");
        painter.drawText(10, y += dy, "i - show/hide polygons square value");
        painter.drawText(10, y += dy, "c - to cut area as black cut line shows");
        painter.drawText(10, y += dy, "h - show/hide this text");
        painter.drawText(10, y += dy, "Mouse wheel to adjust scale");
        painter.drawText(10, y += dy, "Left mouse click and drag'n'drop on background to move all scene");
        painter.drawText(10, y += dy, "Left mouse click and drag'n'drop on vertex to move vertices");
        painter.drawText(10, y += dy, "Right mouse click to select nearest polygon");
        painter.drawText(10, y += dy, "Middle mouse click to split edge of selected polygon");
    }

    painter.save();
    painter.translate(offset_x, offset_y);
    painter.scale(scale, scale);

    painter.setRenderHint(QPainter::Antialiasing, true);

    for(size_t i = 0; i < polygons.size(); i++)
    {
        painter.setPen(QPen(polygons_colors[i % polygons_colors.size()], 1));
        QColor c = QColor(polygons_colors[i % polygons_colors.size()]);
        c.setAlpha(40);
        painter.setBrush(c);
        drawPoly(painter, polygons[i]);

        if(showInfo)
        {
            Vector p = polygons[i].countCenter();
            painter.drawText(QPointF(p.x, p.y), QString::number(polygons[i].countSquare()));
        }
    }

    Polygon poly1, poly2;
    Line cut;
    if(polygons[selectedPolygon].split(squareToCut, poly1, poly2, cut))
    {
        painter.setPen(QPen(Qt::black, 1.5));
        painter.drawLine(QPointF(cut.getStart().x, cut.getStart().y), QPointF(cut.getEnd().x, cut.getEnd().y));
    }

    if(showInfo)
    {
        painter.setPen(QPen(QColor(0, 0, 0, 50), 1));
        Vector np = polygons[selectedPolygon].findNearestPoint(mouse);
        painter.drawLine(QPointF(mouse.x, mouse.y), QPointF(np.x, np.y));
    }

    painter.setPen(QPen(QColor(250, 0, 0, 100), 3));
    painter.setBrush(Qt::transparent);
    drawPoly(painter, polygons[selectedPolygon]);
    for(size_t i = 0; i < polygons[selectedPolygon].size(); i++)
    {
        Vector p = polygons[selectedPolygon][i];
        painter.drawEllipse(QPointF(p.x, p.y), pointSize, pointSize);
    }

    painter.restore();

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void RenderArea::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Q)
    {
        squareToCut += event->modifiers() == Qt::SHIFT ? 100 : 10;
        repaint();
    }
    if(event->key() == Qt::Key_W)
    {
        double t = squareToCut - (event->modifiers() == Qt::SHIFT ? 100 : 10);
        squareToCut = t < 10 ? 10 : t;
        repaint();
    }
    if(event->key() == Qt::Key_C)
    {
        Polygon poly1, poly2;
        Line cut;
        if(polygons[selectedPolygon].split(squareToCut, poly1, poly2, cut))
        {
            polygons[selectedPolygon] = poly1;
            polygons.push_back(poly2);

            if(poly1.countSquare() < poly2.countSquare())
            {
                selectedPolygon = polygons.size() - 1;
            }

            repaint();
        }
    }
    if(event->key() == Qt::Key_P)
    {
        for(size_t i = 0; i < polygons[selectedPolygon].size(); i++)
        {
            Vector p = polygons[selectedPolygon][i];
            fprintf(stdout, "polygons[0].push_back(Vector(%.*e, %.*e));\n", 50, p.x, 50, p.y);
        }
        fflush(stdout);
    }
    if(event->key() == Qt::Key_A)
    {
        if(selectedPolygon > 0)
        {
            selectedPolygon--;
        }
        repaint();
    }
    if(event->key() == Qt::Key_S)
    {
        if(selectedPolygon < (int)polygons.size() - 1)
        {
            selectedPolygon++;
        }
        repaint();
    }
    if(event->key() == Qt::Key_R)
    {
        initPolygons();
        repaint();
    }
    if(event->key() == Qt::Key_I)
    {
        showInfo = !showInfo;
        repaint();
    }
    if(event->key() == Qt::Key_H)
    {
        showHelp = !showHelp;
        repaint();
    }
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    mouse_x = event->x();
    mouse_y = event->y();

    mouse = Vector((event->x() - offset_x) / scale, (event->y() - offset_y) / scale);

    if(event->button() == Qt::LeftButton)
    {
        for(size_t i = 0; i < polygons[selectedPolygon].size(); i++)
        {
            Vector p = polygons[selectedPolygon][i];
            if((mouse - p).length() < pointSize)
            {
                selectedPoint = i;
            }
        }

        mouseLeftPress = 1;
    }
    if(event->button() == Qt::MiddleButton)
    {
        polygons[selectedPolygon].splitNearestEdge(mouse);
    }
    if(event->button() == Qt::RightButton)
    {
        double minDist = DBL_MAX;
        for(size_t i = 0; i < polygons.size(); i++)
        {
            double dist = polygons[i].findDistance(mouse);
            if(dist < minDist)
            {
                minDist = dist;
                selectedPolygon = i;
            }
        }
        squareToCut = polygons[selectedPolygon].countSquare() / 2.0;
        repaint();
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseLeftPress && selectedPoint != -1)
    {
        polygons[selectedPolygon][selectedPoint].x = polygons[selectedPolygon][selectedPoint].x + (event->x() - mouse_x) / scale;
        polygons[selectedPolygon][selectedPoint].y = polygons[selectedPolygon][selectedPoint].y + (event->y() - mouse_y) / scale;
    }
    else if(mouseLeftPress)
    {
        offset_x = offset_x + (event->x() - mouse_x);
        offset_y = offset_y + (event->y() - mouse_y);
    }

    mouse_x = event->x();
    mouse_y = event->y();
    mouse = Vector((event->x() - offset_x) / scale, (event->y() - offset_y) / scale);
    repaint();
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouseLeftPress = 0;
        selectedPoint = -1;
    }
}

void RenderArea::wheelEvent(QWheelEvent *event)
{
    scale += 5 * scale / event->delta();
    repaint();
}

void RenderArea::initPolygons()
{
    polygons.clear();
    polygons.push_back(Polygon());

    polygons[0].push_back(Vector(450.0, 100.0));
    polygons[0].push_back(Vector(900.0, 100.0));
    polygons[0].push_back(Vector(900.0, 400.0));
    polygons[0].push_back(Vector(450.0, 400.0));

    squareToCut = polygons[0].countSquare() / 47.0;
    selectedPolygon = 0;
}
