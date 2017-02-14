include_directories(..)
if (WIN32)
    include_directories(../include)
endif (WIN32)

add_executable(audioprobe audioprobe.cpp)
target_link_libraries(audioprobe rtaudio_static ${LINKLIBS})

add_executable(playsaw playsaw.cpp)
target_link_libraries(playsaw rtaudio_static ${LINKLIBS})

add_executable(playraw playraw.cpp)
target_link_libraries(playraw rtaudio_static ${LINKLIBS})

add_executable(record record.cpp)
target_link_libraries(record rtaudio_static ${LINKLIBS})

add_executable(duplex duplex.cpp)
target_link_libraries(duplex rtaudio_static ${LINKLIBS})

add_executable(testall testall.cpp)
target_link_libraries(testall rtaudio_static ${LINKLIBS})

add_executable(teststops teststops.cpp)
target_link_libraries(teststops rtaudio_static ${LINKLIBS})

add_executable(grabber grabber.cpp)
target_link_libraries(grabber rtaudio_static ${LINKLIBS})




add_executable(recordwav recordwav.cpp)
find_library(SNDFILE_LIB NAMES sndfile sndfile-1 libsndfile libsndfile-1 PATHS "C:/Program Files (x86)/Mega-Nerd/libsndfile/lib" )
target_include_directories (recordwav PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${DRIVER_INCS} "C:/Program Files (x86)/Mega-Nerd/libsndfile/include" )
target_link_libraries(recordwav rtaudio_static ${LINKLIBS} ${SNDFILE_LIB})


