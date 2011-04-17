/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.03.2011 16:20:39
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef LAPACKSERIALIZATION_GY0F0E7V
#define LAPACKSERIALIZATION_GY0F0E7V

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/split_free.hpp>
#include <lapackpp/gmd.h>
#include <lapackpp/lavd.h>
#include <QVector>

/**
 * \file
 * Serializácia typov, ktoré používa lapack
 */

namespace boost {
namespace serialization {

/**
 * Serializácia generickej matice.
 */
template <class Archive> void save(Archive &ar, const LaGenMatDouble &data, const uint /* version */)
{
	int cols = data.cols();
	int rows = data.rows();
	ar << cols;
	ar << rows;
	for (int i = 0; i < data.rows(); ++i) {
		for (int j = 0; j < data.cols(); ++j) {
			ar << data(i, j);
		}
	}
}

/**
 * Deserializácia generickej matice.
 */
template <class Archive> void load(Archive &ar, LaGenMatDouble &data, const uint /* version */)
{
	int cols;
	int rows;
	ar >> cols;
	ar >> rows;
	data.resize(rows, cols);
	double item;
	for (int i = 0; i < data.rows(); ++i) {
		for (int j = 0; j < data.cols(); ++j) {
			ar >> item;
			data(i, j) = item;
		}
	}
}

/**
 * Serializácia a deserializácia generickej matice.
 */
template <class Archive> void serialize(Archive &ar, LaGenMatDouble &data, const uint version)
{
	split_free(ar, data, version);
}

/**
 * Serializácia a deserializácia vektoru.
 */
template <class Archive> void serialize(Archive &ar, LaVectorDouble &data, const uint /*version*/)
{
	ar & boost::serialization::base_object<LaGenMatDouble>(data);
}

} /* end of namespace serialization */
} /* end of namespace boost */

#endif /* end of include guard: LAPACKSERIALIZATION_GY0F0E7V */

