IF(Qwt6_Qt4_LIBRARY AND Qwt6_INCLUDE_DIR)
    SET(Qwt6_FIND_QUIETLY TRUE)
ENDIF(Qwt6_Qt4_LIBRARY AND Qwt6_INCLUDE_DIR)

IF(NOT QT4_FOUND)
	FIND_PACKAGE( Qt4 REQUIRED QUIET )
ENDIF(NOT QT4_FOUND)

IF( QT4_FOUND )
	# Is Qwt6 installed? Look for header files
	FIND_PATH( Qwt6_INCLUDE_DIR qwt.h 
               PATHS ${QT_INCLUDE_DIR} /usr/local/qwt/include /usr/local/include /usr/include/qwt /usr/include /usr/local/qwt-6.0.1 /usr/local/qwt-6.0.0
               PATH_SUFFIXES qwt qwt6 qwt-qt4 qwt6-qt4 include qwt/include qwt6/include qwt-qt4/include qwt6-qt4/include lib/qwt.framework/Versions/6/Headers ENV PATH)
	
	# Find Qwt version
	IF( Qwt6_INCLUDE_DIR )
		FILE( READ ${Qwt6_INCLUDE_DIR}/qwt_global.h QWT_GLOBAL_H )
		STRING( REGEX MATCH "#define *QWT_VERSION *(0x06*)" QWT_IS_VERSION_6 ${QWT_GLOBAL_H})
		
		IF( QWT_IS_VERSION_6 )
		STRING(REGEX REPLACE ".*#define[\\t\\ ]+QWT_VERSION_STR[\\t\\ ]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\".*" "\\1" Qwt_VERSION "${QWT_GLOBAL_H}")

		# Find Qwt6 library linked to Qt4
		FIND_LIBRARY( Qwt6_Qt4_TENTATIVE_LIBRARY NAMES qwt6-qt4 qwt-qt4 qwt6 qwt PATHS /usr/local/qwt-6.0.0/lib /usr/local/qwt-6.0.1/lib /usr/local/qwt/lib /usr/local/lib /usr/lib ${QT_LIBRARY_DIR})
		IF( UNIX AND NOT CYGWIN)
			IF( Qwt6_Qt4_TENTATIVE_LIBRARY )
			        IF( APPLE) 
				EXECUTE_PROCESS( COMMAND otool -L ${Qwt6_Qt4_TENTATIVE_LIBRARY}/Versions/6/qwt OUTPUT_VARIABLE Qwt_Qt4_LIBRARIES_LINKED_TO )
				ELSE( APPLE )
				EXECUTE_PROCESS( COMMAND "ldd" ${Qwt6_Qt4_TENTATIVE_LIBRARY} OUTPUT_VARIABLE Qwt_Qt4_LIBRARIES_LINKED_TO )
				ENDIF( APPLE )
				STRING( REGEX MATCH "QtCore" Qwt6_IS_LINKED_TO_Qt4 ${Qwt_Qt4_LIBRARIES_LINKED_TO})
				IF( Qwt6_IS_LINKED_TO_Qt4 )
					SET( Qwt6_Qt4_LIBRARY ${Qwt6_Qt4_TENTATIVE_LIBRARY} )
					SET( Qwt6_Qt4_FOUND TRUE )
					IF (NOT Qwt6_FIND_QUIETLY)
						MESSAGE( STATUS "Found Qwt: ${Qwt6_Qt4_LIBRARY}" )
					ENDIF (NOT Qwt6_FIND_QUIETLY)
				ENDIF( Qwt6_IS_LINKED_TO_Qt4 )
			ENDIF( Qwt6_Qt4_TENTATIVE_LIBRARY )
		ELSE( UNIX AND NOT CYGWIN)
		# Assumes qwt.dll is in the Qt dir
			SET( Qwt6_Qt4_LIBRARY ${Qwt6_Qt4_TENTATIVE_LIBRARY} )
			SET( Qwt6_Qt4_FOUND TRUE )
			IF (NOT Qwt6_FIND_QUIETLY)
				MESSAGE( STATUS "Found Qwt version ${Qwt_VERSION} linked to Qt4" )
			ENDIF (NOT Qwt6_FIND_QUIETLY)
		ENDIF( UNIX AND NOT CYGWIN)
		
				
		ENDIF( QWT_IS_VERSION_6 )
		
		IF( Qwt6_Qt4_FOUND )
			SET( Qwt6_FOUND TRUE )
		ENDIF( Qwt6_Qt4_FOUND )
		
		MARK_AS_ADVANCED( Qwt6_INCLUDE_DIR Qwt6_Qt4_LIBRARY )
	ENDIF( Qwt6_INCLUDE_DIR )

   	IF (NOT Qwt6_FOUND AND Qwt6_FIND_REQUIRED)
      		MESSAGE(FATAL_ERROR "Could not find Qwt 6.x")
   	ENDIF (NOT Qwt6_FOUND AND Qwt6_FIND_REQUIRED)

ENDIF( QT4_FOUND )
