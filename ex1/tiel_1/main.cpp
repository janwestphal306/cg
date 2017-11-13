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
		// comment
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

	if (inputFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&inputFile);
		// x, y surpassing the image for parsing & width and height of the image
		int x = 0, y = 0, width = 0, height = 0;
		// current state of parsing:
		// 0: parse file format (only allow P3)
		// 1: parse width of image
		// 2: parse height of image
		// 3: parse max value -> multiplier := 255/(max value), so parsed colors are always in range 0 to 255
		// 4: parse colors for pixels
		int stateOfParsing = 0, multiplier = 0;
		// current color:
		// 0: red
		// 1: green
		// 2: blue and the pixel in image is colored
		int currentColor = 0, red = 0, green = 0;
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
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		return false;
	}
	QDataStream out(&file);

	// Write the "magic number"
	out << (quint8)137 << (quint8)80 << (quint8)78 << (quint8)71 << (quint8)13 << (quint8)10 << (quint8)26 << (quint8)10;

	QByteArray header;
	QDataStream headerStream(&header, QIODevice::WriteOnly);

	//IHDR Header
	out << (quint32)13;
	headerStream << (quint8)0x49;
	headerStream << (quint8)0x48;
	headerStream << (quint8)0x44;
	headerStream << (quint8)0x52;

	//Image Data
	headerStream << (quint32)image.width();
	headerStream << (quint32)image.height();
	headerStream << (quint8)8; // bit depth
	headerStream << (quint8)2; // color type
	headerStream << (quint8)0; // compression method
	headerStream << (quint8)0; // filter method
	headerStream << (quint8)0; // interlace method

	unsigned long headerCrc = computeCRC(header);
	out.writeRawData(header, header.size());
	out << (quint32)headerCrc;

	//IDAT
	QByteArray idat;

	int size = image.width()*image.height() * 3;
	out << (quint32)size;

	idat.append("IDAT");

	QByteArray rawData;
	QDataStream rawStream(&rawData, QIODevice::WriteOnly);

	for (int y = 0; y < image.height(); y++) {
		rawStream << (quint8)0; // leading 0 per row
		for (int x = 0; x < image.width(); x++) {
			QColor pixelColor = image.pixelColor(x, y);
			rawStream << (quint8)pixelColor.red();
			rawStream << (quint8)pixelColor.green();
			rawStream << (quint8)pixelColor.blue();
		}
	}
	rawData = computeDeflate(rawData.begin(), rawData.end());
	idat.append(rawData);

	unsigned long idatCrc = computeCRC(idat);
	out.writeRawData(idat, idat.size());
	out << (quint32)idatCrc;

	// IEND
	QByteArray iend;
	QDataStream iendStream(&iend, QIODevice::WriteOnly);

	out << (quint32)0;
	iend.append("IEND");

	unsigned long iendCrc = computeCRC(iend);
	out.writeRawData(iend, iend.size());
	out << (quint32)iendCrc;

	file.close();

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
	double intensity = 0.5;
	for (int y = 0; y < image.height(); y++) {
		for (int x = 0; x < image.width(); x++) {
			QColor rgb = image.pixelColor(x, y);
			double red = rgb.red(), green = rgb.green(), blue = rgb.blue();
			int grey = int(0.299 * red + 0.587 * green + 0.114 * blue);
			red = grey * intensity + red * (1 - intensity);
			green = grey * intensity + green * (1 - intensity);
			blue = grey * intensity + blue * (1 - intensity);
			image.setPixelColor(x, y, QColor(red, green, blue));
		}
	}
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
