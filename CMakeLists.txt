project(FaceDetector C CXX)
cmake_minimum_required(VERSION 2.4)
cmake_policy(SET CMP0003 NEW)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -march=native -O2 -Wall -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG   "-std=c++0x -march=native -O2 -g3 -ggdb -Wall -Wextra -DDEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

# ak kompilujeme s debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Configured to compile including full debug information in the executables")
	add_definitions(-DQT_DEBUG)
else(CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Configured to compile including debug information in the executables")
	set(CMAKE_BUILD_TYPE Release)
	add_definitions(-DQT_NO_DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

find_package(Qt4 4.7 COMPONENTS QtCore QtDeclarative QtGui QtOpengl QtSvg QtXml QtXmlPatterns REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost 1.32.0 COMPONENTS serialization)
find_package(Qwt6 REQUIRED)
pkg_check_modules(LAPACKPP REQUIRED lapackpp)

set(QT_USE_DECLARATIVE 1)
set(QT_USE_SVG 1)
set(QT_USE_OPENGL 1)
set(QT_USE_XML 1)
set(QT_USE_XMLPATTERNS 1)

include(${QT_USE_FILE})

include_directories(
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_BINARY_DIR}/src"
	${QT_INCLUDES}
	${BZIP2_INCLUDE_DIR}
	${LAPACKPP_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${Qwt6_INCLUDE_DIR}
)

set(facedetect_SRCS
	src/main.cpp
	src/core/DetectorImageProvider.cpp
	src/core/FaceBrowserModel.cpp
	src/core/FaceImageProvider.cpp
	src/core/FilterImageProvider.cpp
	src/libfacedetect/Align.cpp
	src/libfacedetect/BPNeuralNet.cpp
	src/libfacedetect/FaceDetector.cpp
	src/libfacedetect/FaceFileScanner.cpp
	src/libfacedetect/FaceStructuredNet.cpp
	src/libfacedetect/FileScanner.cpp
	src/libfacedetect/GaborFilter.cpp
	src/libfacedetect/GrayscaleFilter.cpp
	src/libfacedetect/IlluminationFilter.cpp
	src/libfacedetect/ImageFileScanner.cpp
	src/libfacedetect/ImageFilter.cpp
	src/libfacedetect/ImageSegmenter.cpp
	src/libfacedetect/NetTrainer.cpp
	src/libfacedetect/NeuralNet.cpp
	src/libfacedetect/SobelFilter.cpp
	src/libfacedetect/TrainingDataReader.cpp
	src/libfacedetect/TrainingImageDatabase.cpp
	src/libfacedetect/utils/PolygonRasterizer.cpp
	src/gui/console/ConsoleInterface.cpp
	src/gui/qml/QmlWin.cpp
	src/gui/qml/plugins/QmlCursorArea.cpp
	src/gui/qml/plugins/QmlFaceDetectPlugin.cpp
	src/gui/qml/plugins/QmlFaceDetectPlugin.cpp
	src/gui/qml/plugins/QmlFileChooser.cpp
	src/gui/qml/plugins/QmlQwtPlotCurve.cpp
	src/gui/qml/plugins/QmlQwtPlotWidget.cpp
)

set(facedetect_HDRS
	src/core/DetectorImageProvider.h
	src/core/FaceImageProvider.h
	src/core/FilterImageProvider.h
	src/libfacedetect/GaborFilter.h
	src/libfacedetect/GrayscaleFilter.h
	src/libfacedetect/IlluminationFilter.h
	src/libfacedetect/ImageFilter.h
	src/libfacedetect/ImageFilterBase.h
	src/libfacedetect/ImageSegmenter.h
	src/libfacedetect/SobelFilter.h
	src/libfacedetect/utils/LapackSerialization.h
	src/libfacedetect/utils/PolygonRasterizer.h
	src/libfacedetect/utils/QtSerialization.h
)

set(facedetect_MOC_HDRS
	src/core/FaceBrowserModel.h
	src/libfacedetect/Align.h
	src/libfacedetect/BPNeuralNet.h
	src/libfacedetect/FaceDetector.h
	src/libfacedetect/FaceFileScanner.h
	src/libfacedetect/FaceStructuredNet.h
	src/libfacedetect/FileScanner.h
	src/libfacedetect/ImageFileScanner.h
	src/libfacedetect/NetTrainer.h
	src/libfacedetect/NeuralNet.h
	src/libfacedetect/TrainingDataReader.h
	src/libfacedetect/TrainingImageDatabase.h
	src/gui/console/ConsoleInterface.h
	src/gui/qml/plugins/QmlCursorArea.h
	src/gui/qml/plugins/QmlFaceDetectPlugin.h
	src/gui/qml/plugins/QmlFileChooser.h
	src/gui/qml/plugins/QmlQwtPlotCurve.h
	src/gui/qml/plugins/QmlQwtPlotWidget.h
	src/gui/qml/QmlWin.h
)

set(facedetect_UIS
)

set(facedetect_QRCS
	src/gui/qml/qml.qrc
)

set(facedetect_LANG
	locale/facedetect_sk_SK.ts
)

qt4_wrap_cpp(facedetect_MOC_SRCS ${facedetect_MOC_HDRS})
qt4_wrap_ui(facedetect_UI_SRCS ${facedetect_UIS})
qt4_add_resources(facedetect_QRC_SRCS ${facedetect_QRCS})
qt4_add_translation(facedetect_LANG_SRCS ${facedetect_LANG})

add_executable(
	facedetect
	${facedetect_SRCS}
	${facedetect_HDRS}
	${facedetect_MOC_HDRS}
	${facedetect_MOC_SRCS}
	${facedetect_UIS}
	${facedetect_UI_SRCS}
	${facedetect_QRCS}
	${facedetect_QRC_SRCS}
	${facedetect_LANG_SRCS}
)

target_link_libraries(facedetect
	${QT_LIBRARIES}
	${BZIP2_LIBRARIES}
	${LAPACKPP_LIBRARIES}
	${Boost_LIBRARIES}
	${Qwt6_LIBRARY}
)

