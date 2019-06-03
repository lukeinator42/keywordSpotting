CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(PROJECTNAME keywordSpotter)
PROJECT(${PROJECTNAME})

FIND_PACKAGE(SNDFILE REQUIRED)

SET(PROJECT_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/include)

# Find .cpp and .h files automatically.  This is a bit lazy,
# and in principle it would be better to list these files manually.
SET(folder_source 
  ./src/sound/Sound.cpp
  ./src/utils/WavUtils.cpp
  ./src/EmbeddingNetwork.cpp
  ./src/features/Spectrogram.cpp
  ./src/features/FftRealPair.cpp
  ./src/main.cpp
)

SET(folder_header 
  ./include/keywordSpotter/EmbeddingNetwork.h
  ./include/keywordSpotter/sound/Sound.h
  ./include/keywordSpotter/utils/WavUtils.h
  ./include/keywordSpotter/features/Spectrogram.h
  ./include/keywordSpotter/features/FftRealPair.h
)

MESSAGE("folder_header ${folder_header}")

# Declare groups of source and header files -- makes things pretty in MSVC.
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Add our include files into our compiler's search path.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include ${SNDFILE_INCLUDE_DIR})

# Create everything needed to build our library
#ADD_LIBRARY(${PROJECTNAME} ${folder_source} ${folder_header})

ADD_EXECUTABLE(${PROJECTNAME} ${folder_source} ${folder_header})

TARGET_LINK_LIBRARIES(${PROJECTNAME} ${SNDFILE_LIBRARY})


