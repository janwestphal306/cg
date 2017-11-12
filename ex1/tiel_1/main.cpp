#include <cassert>
#include <cmath>
#include <iostream>

#include <QCoreApplication>
#include <QColor>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QImage>
#include <QTextStream>

#include "miniz.hpp"
#include "crc.h"

bool parseLineInPpm(QImage &image, int &x, int &y, int &width, int &height, QString &line, int &stateOfParsing, int &multiplier, int &currentColor, int &red, int &green) {
	line = line.simplified();
	QStringList list = line.split(" ");

	for (int i = 0; i < list.size(); i++) {
		if (list.at(i).startsWith("#")) {
			return true;
		}

		bool validInput;
		int input = list.at(i).toInt(&validInput);
		if (!validInput) {
			return false;
		}
		switch (stateOfParsing) {
			// image width
			case 1:
				width = input;
				stateOfParsing++;
				break;
			// image height
			case 2:
				height = input;
				image = QImage(width, height, QImage::Format_RGB32);
				stateOfParsing++;
				break;
			// max value of input
			case 3:
				multiplier = 255 / input;
				stateOfParsing++;
				break;
			// parsing of colors
			case 4:
				if (y >= image.height()) {
					return false;
				}
				switch (currentColor) {
					// red
					case 0:
						red = input * multiplier;
						currentColor++;
						break;
					// green
					case 1:
						green = input * multiplier;
						currentColor++;
						break;
					// blue -> color can be adjust to pixel
					case 2:
						int blue = input * multiplier;
						currentColor = 0;
						QRgb value = qRgb(red, green, blue);
						image.setPixel(x, y, value);

						x++;
						if (x >= image.width()) {
							y++; x = 0;
						}

						break;
				}
				break;
		}
	}
	return true;
}

QImage loadPpm(const QString &filename)
{
	// Exercise 1.1 (4 Points)
	// TODO: Implement a reader to load an image in Netpbm format. Only support of P3 is required for this exercise.
	//       In case of an error (e.g. if opening a file failed) return an invalid image (QImage.isNull() returns true in this case)
	QFile inputFile(filename);
	QImage image = QImage();
	// int width, height;

	if (inputFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&inputFile);

		int x = 0, y = 0, width = 0, height = 0, stateOfParsing = 0, multiplier = 0, currentColor = 0, red = 0, green = 0;
		while (!in.atEnd()) {
			QString line = in.readLine();
			if (stateOfParsing == 0) {
				if (line.toStdString() == "P3") { stateOfParsing = 1; continue; }
				else { return QImage(); }
			}
			if (!parseLineInPpm(image, x, y, width, height, line, stateOfParsing, multiplier, currentColor, red, green)) {
				return QImage();
			}
		}
		if (stateOfParsing == 4 && currentColor == 0 && x == 0 && y == image.height()) {
			return image;
		}
	}
	return QImage();
	/*
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		int i = 0;
		while (!in.atEnd() && i <= 2) {
			// skip comments
			QString line = in.readLine().simplified();
			QStringList list = line.split(" ");
			switch (i) {
			case 0:
				if (line.toStdString() != "P3") {
					return QImage();
				}
				break;

			case 1:
				width = list.at(0).toInt();
				height = list.at(1).toInt();
				image = QImage(width, height, QImage::Format_RGB32);
				break;
			case 2:
				// maximum value for each pixel, do nothing for now
				
				break;
			}
			i++;
		}
		if (i <= 2) {
			return QImage();
		}
		int x = 0, y = 0, color_index = 0, red, green, blue;
		int line_index = 0;
		while (!in.atEnd())
		{
			line_index++;
			if (y >= image.height()) {
				return QImage();
			}
			QString line = in.readLine().simplified();
			QStringList list = line.split(" ");
			for (int i = 0; i < list.size(); i++) {
				if (line_index < 5) {
					bool valid;
					std::cout << list.at(i).toInt(&valid) << " ";
					std::cout << valid << std::endl;
				}
				switch (color_index) {
					case 0: red = list.at(i).toInt(); break;
					case 1:	green = list.at(i).toInt(); break;
					case 2:	blue = list.at(i).toInt(); 
						QRgb value = qRgb(red, green, blue);
						image.setPixel(x, y, value);
						// std::cout << x << " " << y << " " << red << " " << green << " " << " " << blue << std::endl;
						x++;
						if (x >= image.width()) {
							y++; x = 0;
						}
						break;
				}
				color_index++;
				if (color_index > 2) {
					color_index = 0;
				}
			}
		}
		inputFile.close();
	}
	return image;
	*/
}

QImage loadImage(const QString &filename)
{
	QFileInfo fileinfo(filename);
	auto extension = fileinfo.suffix().toLower();
	if (extension == "ppm")
	{
		return loadPpm(filename);
	}
	
	return QImage(filename);
}

QByteArray computeDeflate(QByteArray::ConstIterator begin, QByteArray::ConstIterator end)
{
	unsigned long compressedLength = mz_compressBound(end - begin);
	QByteArray compressed(compressedLength, Qt::Uninitialized);

	int resultCode = mz_compress2(
		reinterpret_cast<unsigned char*>(compressed.data()),
		&compressedLength,
		reinterpret_cast<const unsigned char*>(begin),
		end - begin,
		MZ_BEST_COMPRESSION
	);

	Q_ASSERT(resultCode == 0);

	return QByteArray(compressed.data(), compressedLength);
}

unsigned long computeCRC(const QByteArray & data)
{
	return crc(reinterpret_cast<const unsigned char*>(data.data()), data.size());
}

bool storePng(const QString &filename, const QImage &image)
{
	// Exercise 1.2 (6 Points)
	// TODO: Implement a writer to store an image as PNG. Do *NOT* use image.save(filename) here - we want to see an own implementation by you! ;-)
	//       Use a QDataStream with BigEndian-ByteOrder. Care that you are using types that QDataStream is supporting. E.g. you must cast an int to quint8, if you want to store only a single byte.
	//       Use computeDeflate() to compress data and computeCRC() to calculate checksum.
	//       Return true in case storing PNG was successfull, otherwise false;

	return true;
}

bool storeImage(const QString &filename, const QImage &image)
{
	QFileInfo fileinfo(filename);
	auto extension = fileinfo.suffix().toLower();
	if (extension == "png")
	{
		return storePng(filename, image);
	}
	return image.save(filename);
}


void applyGrayscale(QImage &image)
{
	// Exercise 1.3 (1 Point)
	// TODO: Implement gray scaling on each pixel
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

void applyDesaturation(QImage &image)
{
	// Exercise 1.4 (1 Point)
	// TODO: Implement desaturation on each pixel
}

void applyQuantizationErrorToPixel(QImage &image, int x, int y, int quantError, double multiplier) {
	if (x < 0 || x >= image.width() || y < 0 || y >= image.height()) {
		return;
	}
	int oldPixelValue = image.pixelColor(x, y).red();
	int newPixelValue = oldPixelValue + quantError * multiplier;
	if (newPixelValue < 0) { newPixelValue = 0; }
	if (newPixelValue > 255) { newPixelValue = 255; }
	image.setPixelColor(x, y, QColor(newPixelValue, newPixelValue, newPixelValue));
}

void applyDithering(QImage &image, int ditheringLevel)
{
	// Exercise 1.3 (4 Points)
	// TODO: Implement dithering on each pixel
	applyGrayscale(image);

	for (int y = 0; y < image.height(); y++) {
		for (int x = 0; x < image.width(); x++) {

			int oldPixelValue = image.pixelColor(x, y).red();
			// scale old pixel value between 0 and ditheringLevel-1
			int newIndexPixelValue = floor(oldPixelValue / (256.0 / ditheringLevel));
			// scale index value between 0 and 255
			int newPixelValue = round(newIndexPixelValue * (255.0 / (ditheringLevel-1)));

			image.setPixelColor(x, y, QColor(newPixelValue, newPixelValue, newPixelValue));
			int quantError = oldPixelValue - newPixelValue;
			
			applyQuantizationErrorToPixel(image, x + 1, y, quantError, (7.0 / 16.0));
			applyQuantizationErrorToPixel(image, x - 1, y + 1, quantError, (3.0 / 16.0));
			applyQuantizationErrorToPixel(image, x, y + 1, quantError, (5.0 / 16.0));
			applyQuantizationErrorToPixel(image, x + 1, y + 1, quantError, (1.0 / 16.0));
		}
	}	
}


bool applyEffect(QImage &image, const QString &effect)
{
	if (effect == "grayscale")
	{
		applyGrayscale(image);
		return true;
	}

	if (effect == "dithering")
	{
		applyDithering(image, 4);
		return true;
	}

	if (effect == "desaturation")
	{
		applyDesaturation(image);
		return true;
	}

	return false;
}

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	QCommandLineParser parser;

	parser.addHelpOption();
	parser.addPositionalArgument("input", "Input file", "<input>");
	parser.addPositionalArgument("output", "Output file", "<output>");
	parser.addPositionalArgument("effect", "Effect, possible values: dithering, grayscale, desaturation", "[effect]");

	parser.process(app);

	QStringList arguments = parser.positionalArguments();

	if (arguments.size() < 2 || arguments.size() > 3)
	{
		parser.showHelp(1);

		return 2;
	}

	auto image = loadImage(arguments.at(0));

	if (image.isNull())
	{
		qCritical() << "Could not load image!";
		return 1;
	}

	if (arguments.size() == 3)
	{
		if (!applyEffect(image, arguments.at(2).toLower()))
		{
			qCritical() << "Unknown effect!";
			return 1;
		}
	}

	if (!storeImage(arguments.at(1), image))
	{
		qCritical() << "Could not store image!";
		return 1;
	}

	return 0;
}
