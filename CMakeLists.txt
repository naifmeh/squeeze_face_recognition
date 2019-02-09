cmake_minimum_required(VERSION 2.8)
project( squeezeCNN )
find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
#add_definitions(-DEBUG)
#add_definitions(-DISPLAY)
add_definitions(-DISPLAY_STREAM)
#add_definitions(-DVERBOSE)
add_definitions(-DMULTITHREAD)
set(CMAKE_CXX_FLAGS "-O3")
set(CURL_LIBRARY "-lcurl") 
find_package(CURL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrestclient-cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrdkafka -lz -lpthread -lrt")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS filesystem system) 

find_package(OpenMP)

add_executable( squeezecnn ./src/mainAlgo.cpp ./src/face_extraction.cpp ./src/training_generator.cpp ./src/utils.cpp ./src/face_tracking.cpp ./src/network_utils.cpp ./src/video_consumer.cpp ./src/base64.cpp)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries( squeezecnn ${OpenCV_LIBS} )
    TARGET_LINK_LIBRARIES( squeezecnn LINK_PUBLIC ${Boost_LIBRARIES} )   
endif()
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    target_link_libraries( squeezecnn ${CURL_LIBRARIES})
endif()

target_link_libraries( squeezecnn rdkafka )
target_link_libraries( squeezecnn cppkafka )
target_link_libraries( squeezecnn restclient-cpp )

set(CMAKE_BUILD_TYPE Debug)


