#include "line.h"

#include <vector.h>
#include <cassert>

Line::Line()
{

}

Line::Line(const Vector &start, const Vector &end)
    : start(start), end(end)
{
    A = start.y - end.y;
    B = end.x - start.x;
    C = start.x * end.y - end.x * start.y;
}

Line::Line(double A, double B, double C)
    : A(A), B(B), C(C)
{
    if(fabs(A) <= POLY_SPLIT_EPS && fabs(B) >= POLY_SPLIT_EPS)
    {
        start.x = -1000;
        start.y = -(C / B);

        end.x = 1000;
        end.y = start.y;
    }
    else if(fabs(B) <= POLY_SPLIT_EPS && fabs(A) >= POLY_SPLIT_EPS)
    {
        start.x = -(C / A);
        start.y = -1000;

        end.x = start.x;
        end.y = 1000;
    }
    else
    {
        start.x = -1000;
        start.y = -((A * start.x + C) / B);

        end.x = 1000;
        end.y = -((A * end.x + C) / B);
    }
}

double Line::getDistance(const Vector &point) const
{
    double n = A * point.x + B * point.y + C;
    double m = sqrt(A * A + B * B);
    assert(m != 0);
    return n / m;
}

Vector Line::getLineNearestPoint(const Vector &point) const
{
    Vector dir(B, -A);
    double u = (point - start).dot(dir) / dir.squareLength();
    return start + dir * u;
}

Vector Line::getSegmentNearestPoint(const Vector &point) const
{
    Vector dir(B, -A);
    double u = (point - start).dot(dir) / dir.squareLength();
    if(u < 0)
        return start;
    else if(u > 1)
        return end;
    else
        return start + dir * u;
}

int Line::pointSide(const Vector &point) const
{
    double s = A * (point.x - start.x) + B * (point.y - start.y);
    return (s > 0 ? 1 : (s < 0 ? -1 : 0));
}

#define inside(v, min, max) (((min) <= (v) + (POLY_SPLIT_EPS)) && ((v) <= (max) + (POLY_SPLIT_EPS)))
#define det(a, b, c, d) (((a) * (d)) - ((b) * (c)))
#define minimum(a, b) (((a) > (b)) ? (b) : (a))
#define maximum(a, b) (((a) < (b)) ? (b) : (a))

int Line::crossLineSegment(const Line &line, Vector &result) const
{
    double d = det(A, B, line.A, line.B);
    if(d == 0)
        return 0;

    result.x = -det(C, B, line.C, line.B) / d;
    result.y = -det(A, C, line.A, line.C) / d;

    return inside(result.x, minimum(line.start.x, line.end.x), maximum(line.start.x, line.end.x)) &&
            inside(result.y, minimum(line.start.y, line.end.y), maximum(line.start.y, line.end.y));
}

int Line::crossSegmentSegment(const Line &line, Vector &result) const
{
    double d = det(A, B, line.A, line.B);
    if(d == 0)
        return 0;

    result.x = -det(C, B, line.C, line.B) / d;
    result.y = -det(A, C, line.A, line.C) / d;

    return inside(result.x, minimum(start.x, end.x), maximum(start.x, end.x)) &&
           inside(result.y, minimum(start.y, end.y), maximum(start.y, end.y)) &&
           inside(result.x, minimum(line.start.x, line.end.x), maximum(line.start.x, line.end.x)) &&
           inside(result.y, minimum(line.start.y, line.end.y), maximum(line.start.y, line.end.y));
}

int Line::crossLineLine(const Line &line, Vector &result) const
{
    double d = det(A, B, line.A, line.B);
    if(d == 0)
        return 0;

    result.x = -det(C, B, line.C, line.B) / d;
    result.y = -det(A, C, line.A, line.C) / d;

    return 1;
}

Line Line::getBisector(const Line &l1, const Line &l2)
{
    double q1 = sqrt(l1.A * l1.A + l1.B * l1.B);
    double q2 = sqrt(l2.A * l2.A + l2.B * l2.B);

    double A = l1.A / q1 - l2.A / q2;
    double B = l1.B / q1 - l2.B / q2;
    double C = l1.C / q1 - l2.C / q2;

    return Line(A, B, C);
}

double Line::getTanAngle(const Line &l1, const Line &l2)
{
    return (l1.A * l2.B - l2.A * l1.B) / (l1.A * l2.A + l1.B * l2.B);
}

Vector Line::getStart() const
{
    return start;
}

Vector Line::getEnd() const
{
    return end;
}

double Line::length() const
{
    double x = end.x - start.x;
    double y = end.y - start.y;
    return sqrt(x * x + y * y);
}

double Line::squareLength() const
{
    double x = end.x - start.x;
    double y = end.y - start.y;
    return x * x + y * y;
}

Line Line::reverse() const
{
    return Line(end, start);
}

Vector Line::getPointAlong(double t) const
{
    return start + (end - start).norm() * t;
}

Line Line::directedLine(const Vector &p, const Vector &d)
{
    return Line(p, p + d);
}
