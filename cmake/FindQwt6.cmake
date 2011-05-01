# Tento modul vyhľadáva Qwt 6
#
# Qwt6_INCLUDE_DIR - Adresár, v ktorom sa nachádza qwt.h
# Qwt6_LIBRARY - Knižnica libwqt6
# Qwt6_FOUND - Nastaví sa na TRUE, ak bola knižnica Qwt6 nájdená

# Ak boli cesty nastavené nemusí sa nič hľadať
if (Qwt6_INCLUDE_DIR AND Qwt_LIBRARY)
	set(Qwt6_FIND_QUIETLY TRUE)
endif (Qwt6_INCLUDE_DIR AND Qwt_LIBRARY)

# Vyhľadanie závislosti Qt4
if (NOT QT4_FOUND)
	find_package(Qt4 4.5 REQUIRED)
endif (NOT QT4_FOUND)

# Ukončenie ak nebolo nájdené Qt4
if(NOT QT4_FOUND)
	if (Qwt6_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find Qt4")
	endif (Qwt6_FIND_REQUIRED)
endif(NOT QT4_FOUND)

if (QT4_FOUND)
	# Nájdenie hlavičkových súborov
	find_path(Qwt6_INCLUDE_DIR qwt.h PATHS /usr/local /usr ${QT_INCLUDE_DIR} PATH_SUFFIXES qwt6 qwt include qwt6/include qwt/include ENV PATH)
	if (Qwt6_INCLUDE_DIR)
		file(READ ${Qwt6_INCLUDE_DIR}/qwt_global.h QWT_GLOBAL_H)
		string(REGEX MATCH "#define *QWT_VERSION *(0x06*)" QWT_IS_VERSION_6 ${QWT_GLOBAL_H})
		# Nájdenie knižníc
		if (QWT_IS_VERSION_6)
			find_library(Qwt6_LIBRARY NAMES qwt6-qt4 qwt6 qwt-qt4 qwt PATHS /usr/local/qwt/lib /usr/local/lib /usr/lib ${QT_LIBRARY_DIR})
		endif (QWT_IS_VERSION_6)
		if (Qwt6_LIBRARY)
			# Označenie úspešnosti hľadania
			set(Qwt6_FOUND TRUE)
			# Výpis stavu hľadania
			if (NOT Qwt6_FIND_QUIETLY)
				message(STATUS "Found Qwt: ${Qwt6_LIBRARY}")
			endif (NOT Qwt6_FIND_QUIETLY)
		endif (Qwt6_LIBRARY)
	endif (Qwt6_INCLUDE_DIR)
	if (NOT Qwt6_FOUND AND Qwt6_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find Qwt6")
	endif(NOT Qwt6_FOUND AND Qwt6_FIND_REQUIRED)
endif (QT4_FOUND)

