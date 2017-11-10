#include "imageeffects.h"

#include <QColor>
#include <QImage>

namespace ImageEffects
{

	QColor pixel(const QImage & image, int x, int y)
	{
		//TODO: Return value pixel if coordinate is valid; else nearest value of pixel
		return QColor();
	}

	QImage blur(const QImage &image)
	{
		//TODO: Implement Gauss filter here
		return QImage();
	}

	QImage hypotheticalContrast(const QImage &image)
	{
		//TODO: Implement Emboss filter here
		return QImage();
	}

	QImage sharpness(const QImage &image)
	{
		//TODO: Implement Sharpness filter here
		return QImage();
	}

	QImage edgeDetection(const QImage &image)
	{
		//TODO: Implement Sobel operator here
		return QImage();
	}

	QImage colorBasedSharpness(const QImage &image)
	{
		//TODO: Implement color based filter here. Use the sharpness filter for foreground colors and the blur filter for background colors.
		return QImage();
	}

	/*QImage flowField(const QSize & targetSize)
	{
		//TODO: Create a radial centered flow field with the vector length decreasing with increasing distance to the center.
		//Save the x- and y- values of the normalized vector as R and G components and store the vectors' length in the B component of each pixel.

		return QImage();
	}

	QImage flowFieldFilter(const QSize & targetSize, const QImage &image)
	{
		//TODO: Apply a mean filter using the flow field and the linePixels method.
		//Use the B component of each pixel as a length in [0,32].
		//Set the color of each pixel in the resulting image to the mean value of the colors of all pixels on a line through
		//this pixel with an orientation described by the vector in the flow field and the length from the B component.

		QImage flowFieldImage = flowField(targetSize);

		return QImage();
	}*/

	/*std::vector<QPoint> linePixels(int x0, int y0, int x1, int y1)
	{
		//TODO: Implement the Bresenham line rasterization algorithm.
		//Return a std::vector with the coordinates of all pixels that belong to the given line.

		return std::vector<QPoint>();
	}
	*/

}
