set(CMAKE_CXX_STANDARD 17)

find_package(OpenVDB REQUIRED)
find_package(ECM REQUIRED NO_MODULE)
find_package(TBB REQUIRED)
find_package(Boost REQUIRED)
find_package(EIGEN3 REQUIRED NO_MODULE)

set(WINDING_DIR ${VCGDIR}/wrap/WindingNumber CACHE INTERNAL "")
file(GLOB WNFILES ${WINDING_DIR}/*.cpp)

set(SOURCES filter_openvdb.cpp ${WNFILES})

set(HEADERS filter_openvdb.h)

add_meshlab_plugin(filter_openvdb ${SOURCES} ${HEADERS})

set(EIGEN_DIR  = "${VCGDIR}/eigenlib" CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_FIND_MODULE_DIR})

target_include_directories(filter_openvdb PRIVATE ${VCGDIR})
target_include_directories(filter_openvdb PRIVATE ${TBB_INCLUDE_DIRS})
target_include_directories(filter_openvdb PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(filter_openvdb PRIVATE ${OpenVDB_INCLUDE_DIR})
target_include_directories(filter_openvdb PRIVATE ${EIGEN_DIR})
target_include_directories(filter_openvdb PRIVATE ${WINDING_DIR})

target_link_libraries(filter_openvdb PRIVATE
  ${OpenVDB_LIBRARIES}
  tbb
  ${Boost_LIBRARIES}
)

# target_compile_definitions(filter_openvdb PRIVATE QT_NO_KEYWORDS)