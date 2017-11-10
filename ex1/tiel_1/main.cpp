#include <cassert>
#include <cmath>

#include <QCoreApplication>
#include <QColor>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QImage>
#include <QTextStream>

#include "miniz.hpp"
#include "crc.h"

QImage loadPpm(const QString &filename)
{
	// Exercise 1.1 (4 Points)
	// TODO: Implement a reader to load an image in Netpbm format. Only support of P3 is required for this exercise.
	//       In case of an error (e.g. if opening a file failed) return an invalid image (QImage.isNull() returns true in this case)

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
}

void applyDesaturation(QImage &image)
{
	// Exercise 1.4 (1 Point)
	// TODO: Implement desaturation on each pixel
}

void applyDithering(QImage &image, int ditheringLevel)
{
	// Exercise 1.3 (4 Points)
	// TODO: Implement dithering on each pixel
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
