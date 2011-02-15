/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 17:20:19
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "ImageDataReader.h"

namespace FaceDetect {

ImageDataReader::ImageDataReader(QObject *parent):
	TrainingDataReader(parent)
{
}

ImageDataReader::~ImageDataReader()
{
}

std::size_t ImageDataReader::inputVectorSize() const
{
	return 0;
}

std::size_t ImageDataReader::outputVectorSize() const
{
	return 0;
}

std::size_t ImageDataReader::trainingSetSize() const
{
	return 0;
}

LaColVectorDouble ImageDataReader::inputVector(std::size_t /*sample*/) const
{
	return LaColVectorDouble();
}

LaColVectorDouble ImageDataReader::outputVector(std::size_t /*sample*/) const
{
	return LaColVectorDouble();
}

} /* end of namespace FaceDetect */

