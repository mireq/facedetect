/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.03.2011 16:20:39
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <lapackpp/gmd.h>
#include <lapackpp/lavd.h>

namespace boost {
namespace serialization {

template <class Archive> void save(Archive &ar, const LaGenMatDouble &data, unsigned int /* version */)
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

template <class Archive> void load(Archive &ar, LaGenMatDouble &data, unsigned int /* version */)
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

template <class Archive> void serialize(Archive &ar, LaVectorDouble &data, unsigned int /*version*/)
{
	ar & boost::serialization::base_object<LaGenMatDouble>(data);
}

} /* end of namespace serialization */
} /* end of namespace boost */

BOOST_SERIALIZATION_SPLIT_FREE(LaGenMatDouble);

