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
#include <boost/serialization/nvp.hpp>
#include <lapackpp/gmd.h>
#include <lapackpp/lavd.h>

/**
 * \file
 * Serializácia typov, ktoré používa lapack
 */

namespace boost {
namespace serialization {

/**
 * Serializácia generickej matice.
 */
template <class Archive> void save(Archive &ar, const LaGenMatDouble &data, const unsigned int /* version */)
{
	int cols = data.cols();
	int rows = data.rows();
	ar << BOOST_SERIALIZATION_NVP(cols);
	ar << BOOST_SERIALIZATION_NVP(rows);
	for (int i = 0; i < data.rows(); ++i) {
		for (int j = 0; j < data.cols(); ++j) {
			ar << boost::serialization::make_nvp("i", data(i, j));
		}
	}
}

/**
 * Deserializácia generickej matice.
 */
template <class Archive> void load(Archive &ar, LaGenMatDouble &data, const unsigned int /* version */)
{
	int cols;
	int rows;
	ar >> BOOST_SERIALIZATION_NVP(cols);
	ar >> BOOST_SERIALIZATION_NVP(rows);
	data.resize(rows, cols);
	double item;
	for (int i = 0; i < data.rows(); ++i) {
		for (int j = 0; j < data.cols(); ++j) {
			ar >> boost::serialization::make_nvp("i", item);
			data(i, j) = item;
		}
	}
}

/**
 * Serializácia a deserializácia generickej matice.
 */
template <class Archive> void serialize(Archive &ar, LaGenMatDouble &data, const unsigned int version)
{
	split_free(ar, data, version);
}

/**
 * Serializácia a deserializácia vektoru.
 */
template <class Archive> void serialize(Archive &ar, LaVectorDouble &data, const unsigned int /*version*/)
{
	ar & boost::serialization::make_nvp("base", boost::serialization::base_object<LaGenMatDouble>(data));
}

} /* end of namespace serialization */
} /* end of namespace boost */

#endif /* end of include guard: LAPACKSERIALIZATION_GY0F0E7V */

