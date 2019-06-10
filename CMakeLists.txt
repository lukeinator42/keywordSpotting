CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(PROJECTNAME keywordSpotter)
PROJECT(${PROJECTNAME})

FIND_PACKAGE(SNDFILE REQUIRED)
FIND_PACKAGE(TensorFlow REQUIRED)
FIND_PACKAGE(Portaudio REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=${TensorFlow_ABI}")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=${TensorFlow_ABI}" )

TensorFlow_REQUIRE_C_LIBRARY()
TensorFlow_REQUIRE_SOURCE()

SET(PROJECT_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/include)

# Find .cpp and .h files automatically.  This is a bit lazy,
# and in principle it would be better to list these files manually.
SET(lib_source 
  ./src/lib/sound/Sound.cpp
  ./src/lib/utils/WavUtils.cpp
  ./src/lib/EmbeddingNetwork.cpp
  ./src/lib/features/Spectrogram.cpp
  ./src/lib/features/FftRealPair.cpp
  ./src/lib/EmbeddingDatabase.cpp
)

SET(folder_source 
  ./src/test_tensorflow.cpp
  ./src/write_spectrogram.cpp
  ./src/record_spectrogram.cpp
  ./src/test_portaudio.cpp
  ./src/test_embedding_database.cpp
)

SET(lib_header 
  ./include/keywordSpotter/lib/EmbeddingNetwork.h
  ./include/keywordSpotter/lib/sound/Sound.h
  ./include/keywordSpotter/lib/utils/WavUtils.h
  ./include/keywordSpotter/lib/features/Spectrogram.h
  ./include/keywordSpotter/lib/features/FftRealPair.h
  ./include/keywordSpotter/lib/EmbeddingDatabase.h
)

MESSAGE("folder_header ${folder_header}")

# Declare groups of source and header files -- makes things pretty in MSVC.
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Add our include files into our compiler's search path.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include ${SNDFILE_INCLUDE_DIR} ${PORTAUDIO_LIBRARIES} TensorFlow_DEP)

# Create everything needed to build our library
ADD_LIBRARY(${PROJECTNAME} ${lib_source} ${lib_header})
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${SNDFILE_LIBRARY} ${PORTAUDIO_LIBRARIES} TensorFlow_DEP)

FOREACH( testsourcefile ${folder_source} )
  string( REPLACE ".cpp" "" testname2 ${testsourcefile} )
  string( REPLACE "./src/" "" testname ${testname2} )

  ADD_EXECUTABLE( ${testname} ${testsourcefile})
  # Make sure YourLib is linked to each app
  target_link_libraries( ${testname} ${PROJECTNAME} ${SNDFILE_LIBRARY} ${PORTAUDIO_LIBRARIES} TensorFlow_DEP)
endforeach( testsourcefile ${folder_source} )




