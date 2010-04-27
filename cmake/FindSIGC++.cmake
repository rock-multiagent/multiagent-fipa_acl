FIND_PATH(SIGC++_INCLUDE_DIR sigc++/sigc++.h
  /usr/include/sigc++-2.0
  /usr/lib/sigc++-2.0/include
  /usr/local/include
  /usr/include
)

FIND_PATH(SIGC++CONFIG_INCLUDE_DIR sigc++config.h
  /usr/include/sigc++-2.0
  /usr/lib/sigc++-2.0/include
  /usr/local/include
  /usr/include
)


FIND_LIBRARY(SIGC++_LIBRARY
  NAMES sigc-2.0
  PATHS /usr/lib /usr/local/lib 
)

IF (SIGC++_LIBRARY AND SIGC++_INCLUDE_DIR AND SIGC++CONFIG_INCLUDE_DIR)
    SET( SIGC++_FOUND "YES" )
ENDIF()
    
IF (SIGC++_FOUND)
    set(SIGC++_INCLUDES ${SIGC++_INCLUDE_DIR} ${SIGC++CONFIG_INCLUDE_DIR})
   IF (NOT SIGC++_FIND_QUIETLY)
      MESSAGE(STATUS:\n
         "FOUND SIGC++: 
             LIBRARIES: ${SIGC++_LIBRARY}
             INCLUDES: ${SIGC++_INCLUDE_DIR}, ${SIGC++CONFIG_INCLUDE_DIR}")
   ENDIF ()
ELSE ()
   IF (SIGC++_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Couldn't find sigc++ development files")
   ENDIF ()
ENDIF ()
