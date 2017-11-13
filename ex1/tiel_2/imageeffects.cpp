#include "imageeffects.h"

#include <QColor>
#include <QImage>
#include <math.h>
#include <iostream>

namespace ImageEffects
{
	double gaussBlurFilter[3][3] = {
		{ 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 },
		{ 2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0 },
		{ 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 },
	};

	double embossContrastFilter[3][3] = {
		{ 2.0,  0, 0 },
		{ 0, -1.0, 0 },
		{ 0, 0, -1.0 },
	};

	double sharpnessFilter[3][3] = {
		{ -1, -1, -1 },
		{ -1,  9, -1 },
		{ -1, -1, -1 },
	};

	double horizontalSobelFilter[3][3] = {
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 },
	};

	double verticalSobelFilter[3][3] = {
		{ -1, -2, -1 },
		{ 0,  0,  0 },
		{ 1,  2,  1 },
	};

	QColor pixel(const QImage & image, int x, int y)
	{
		//TODO: Return value pixel if coordinate is valid; else nearest value of pixel
		if (x < 0) { x = 0; }
		if (x >= image.width()) { x = image.width() - 1; }

		if (y < 0) { y = 0; }
		if (y >= image.height()) { y = image.height() - 1; }
		
		return image.pixelColor(x, y);
	}

	// adjust rgb to scale 0 to 255
	QColor adjustColor(int red, int green, int blue) {
		if (red > 255) { red = 255; }
		if (green > 255) { green = 255; }
		if (blue > 255) { blue = 255; }
		if (red < 0) { red = 0; }
		if (green < 0) { green = 0; }
		if (blue < 0) { blue = 0; }
		return QColor(red, green, blue);
	}
	
	// color values red, green, blue can still have values < 0 or > 255
	void applyColorOfPixelByMatrix(const QImage &inputImage, int x, int y, double matrix[][3], int k, int &red, int &green, int &blue) {
		int sumRed = 0, sumGreen = 0, sumBlue = 0;
		for (int u = -k; u <= k; u++) {
			for (int v = -k; v <= k; v++) {
				double multiplier = matrix[u + k][v + k];
				QColor pixelColor = pixel(inputImage, u + x, v + y);
				sumRed += multiplier * pixelColor.red();
				sumGreen += multiplier * pixelColor.green();
				sumBlue += multiplier * pixelColor.blue();
			}
		}
		red = sumRed; green = sumGreen; blue = sumBlue;
	}

	// apply matrix to a single pixel
	void applyMatrixToPixel(const QImage &inputImage, QImage &outputImage, int x, int y, double matrix[][3], int k) {
		int red = 0, green = 0, blue = 0;
		applyColorOfPixelByMatrix(inputImage, x, y, matrix, k, red, green, blue);
		QColor newColor = adjustColor(red, green, blue);
		outputImage.setPixelColor(x, y, newColor);
	}

	// apply a full matrix on an image
	QImage applyMatrixToImage(const QImage &inputImage, double matrix[][3], int size) {
		QImage outputImage = QImage(inputImage.width(), inputImage.height(), QImage::Format_RGB32);

		int k = (size - 1) / 2;

		for (int y = 0; y < inputImage.height(); y++) {
			for (int x = 0; x < inputImage.width(); x++) {
				applyMatrixToPixel(inputImage, outputImage, x, y, matrix, k);
			}
		}

		return outputImage;
	}

	QImage blur(const QImage &image)
	{
		//TODO: Implement Gauss filter here
		return applyMatrixToImage(image, gaussBlurFilter, 3);
	}

	QImage hypotheticalContrast(const QImage &image)
	{
		//TODO: Implement Emboss filter here
		return applyMatrixToImage(image, embossContrastFilter, 3);
	}

	QImage sharpness(const QImage &image)
	{
		//TODO: Implement Sharpness filter here
		return applyMatrixToImage(image, sharpnessFilter, 3);
	}
	
	void grayscale(QImage &image) {
		for (int y = 0; y < image.height(); y++) {
			for (int x = 0; x < image.width(); x++) {
				QColor rgb = image.pixelColor(x, y);
				double red = rgb.red(), green = rgb.green(), blue = rgb.blue();
				// old: 0.299r + 0.587g + 0.114b
				// new: 0.2126r + 0.7152g + 0.0722b
				int grey = int(0.299 * red + 0.587 * green + 0.114 * blue);
				image.setPixelColor(x, y, QColor(grey, grey, grey));
			}
		}
	}

	QImage edgeDetection(const QImage &image)
	{
		//TODO: Implement Sobel operator here
		QImage outputImage = QImage(image.width(), image.height(), QImage::Format_RGB32);

		for (int y = 0; y < image.height(); y++) {
			for (int x = 0; x < image.width(); x++) {
				int redX = 0, greenX = 0, blueX = 0;
				int redY = 0, greenY = 0, blueY = 0;

				applyColorOfPixelByMatrix(image, x, y, horizontalSobelFilter, 1, redX, greenX, blueX);
				applyColorOfPixelByMatrix(image, x, y, verticalSobelFilter, 1, redY, greenY, blueY);

				int red = sqrt(pow(redX, 2) + pow(redY, 2));
				int green = sqrt(pow(greenX, 2) + pow(greenY, 2));
				int blue = sqrt(pow(blueX, 2) + pow(blueY, 2));
				
				QColor newColor = adjustColor(red, green, blue);
				outputImage.setPixelColor(x, y, newColor); 
			}
		}

		grayscale(outputImage);
		return outputImage;
	}

	QImage colorBasedSharpness(const QImage &image)
	{
		//TODO: Implement color based filter here. Use the sharpness filter for foreground colors and the blur filter for background colors.
		QImage outputImage = QImage(image.width(), image.height(), QImage::Format_RGB32);
		
		for (int y = 0; y < image.height(); y++) {
			for (int x = 0; x < image.width(); x++) {
				if (image.pixelColor(x, y).red() < 100) {
					applyMatrixToPixel(image, outputImage, x, y, gaussBlurFilter, 1);
				}
				else {
					applyMatrixToPixel(image, outputImage, x, y, sharpnessFilter, 1);
				}
			}
		}

		return outputImage;
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
