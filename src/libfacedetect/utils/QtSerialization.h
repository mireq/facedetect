/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  11.05.2011 19:17:46
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef  QTSERIALIZATION_H
#define  QTSERIALIZATION_H

#include <boost/serialization/list.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <QColor>
#include <QFlags>
#include <QList>
#include <QPair>

namespace boost {
namespace serialization {

template <class Archive> inline void serialize(Archive &ar, QColor &data, const unsigned int version)
{
	Q_UNUSED(version);
	int r = data.red();
	int g = data.green();
	int b = data.blue();
	int a = data.alpha();
	ar & boost::serialization::make_nvp("r", r);
	ar & boost::serialization::make_nvp("g", g);
	ar & boost::serialization::make_nvp("b", b);
	ar & boost::serialization::make_nvp("a", a);
	data = qRgba(r, g, b, a);
}

template <class Archive, class U> inline void serialize(Archive &ar, QFlags<U> &data, const unsigned int version)
{
	Q_UNUSED(version);
	int num = int(data);
	ar & boost::serialization::make_nvp("data", num);
	data = QFlags<U>(num);
}

template <class Archive, class U, class V> inline void serialize(Archive &ar, QPair<U,V> &data, const unsigned int version)
{
	Q_UNUSED(version);
	ar & boost::serialization::make_nvp("first", data.first);
	ar & boost::serialization::make_nvp("second", data.second);
}

template <class Archive, class U> inline void save(Archive &ar, const QList<U> &data, const unsigned int version)
{
	Q_UNUSED(version);
	boost::serialization::stl::save_collection<Archive, QList<U>>(ar, data);
}

template <class Archive, class U> inline void load(Archive &ar, QList<U> &data, const unsigned int version)
{
	Q_UNUSED(version);
	boost::serialization::stl::load_collection<Archive, QList<U>, boost::serialization::stl::archive_input_seq<Archive, QList<U>>, boost::serialization::stl::no_reserve_imp<QList<U>>>(ar, data);
}

template<class Archive, class U> inline void serialize(Archive &ar, QList<U> &data, const unsigned int file_version)
{
	boost::serialization::split_free(ar, data, file_version);
}

template <class Archive, class U> inline void save(Archive &ar, const QVector<U> &data, const unsigned int version)
{
	Q_UNUSED(version);
	boost::serialization::stl::save_collection<Archive, QVector<U>>(ar, data);
}

template <class Archive, class U> inline void load(Archive &ar, QVector<U> &data, const unsigned int version)
{
	Q_UNUSED(version);
	boost::serialization::stl::load_collection<Archive, QVector<U>, boost::serialization::stl::archive_input_seq<Archive, QVector<U>>, boost::serialization::stl::reserve_imp<QVector<U>>>(ar, data);
}

template<class Archive, class U> inline void serialize(Archive &ar, QVector<U> &data, const unsigned int file_version)
{
	boost::serialization::split_free(ar, data, file_version);
}

} /* end of namespace serialization */
} /* end of namespace boost */

BOOST_SERIALIZATION_COLLECTION_TRAITS(QList)
BOOST_SERIALIZATION_COLLECTION_TRAITS(QVector)

#endif   /* ----- #ifndef QTSERIALIZATION_H  ----- */

