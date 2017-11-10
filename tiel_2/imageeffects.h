#pragma once

#include <type_traits>

#include <qmath.h>
#include <vector>
#include <algorithm>

class QColor;
class QImage;
class QSize;
class QPoint;

namespace ImageEffects
{

QColor pixel(const QImage & image, int x, int y);
//std::vector<QPoint> linePixels(int x0, int y0, int x1, int y1);

QImage blur(const QImage &image);
QImage hypotheticalContrast(const QImage &image);
QImage sharpness(const QImage &image);
QImage edgeDetection(const QImage &image);
QImage colorBasedSharpness(const QImage &image);

//QImage flowField(const QSize & targetSize);
//QImage flowFieldFilter(const QSize & targetSize, const QImage &image);

} // namespace ImageEffects
