# CMake最低版本号要求
cmake_minimum_required(VERSION 3.8)


# 项目名称
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
project(HookMouse VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})


# 输出提示信息
message(STATUS "Operation System : ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_INSTALL_PREFIX : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_CXX_COMPILER : ${CMAKE_CXX_COMPILER}")


# 配置文件
if (_X_SERIES_BUILD_SHARED_LIBS STREQUAL "OFF" OR _X_SERIES_BUILD_STATIC_LIBS STREQUAL "ON")
	set(XANADU_CORE_BUILD_REPLACE _XANADU_CORE_BUILD_STATIC)
	set(X_SERIES_LIBRARY_TYPE "STATIC")
else()
	set(XANADU_CORE_BUILD_REPLACE _XANADU_CORE_BUILD_SHARED)
	set(X_SERIES_LIBRARY_TYPE "SHARED")
endif ()
set(XANADU_CORE_INCLUDE_REPLACE ${CMAKE_INSTALL_PREFIX}/include)
set(XANADU_CORE_LIBRARY_REPLACE ${CMAKE_INSTALL_PREFIX}/lib)
configure_file("${CMAKE_SOURCE_DIR}/config/Find${PROJECT_NAME}.cmake.in" "${CMAKE_BINARY_DIR}/Find${PROJECT_NAME}.cmake")
configure_file("${CMAKE_SOURCE_DIR}/config/${PROJECT_NAME}.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc")
configure_file("${CMAKE_SOURCE_DIR}/config/VersionInfo.rc.in" "${CMAKE_BINARY_DIR}/VersionInfo.rc")


# 设置模块搜索路径
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/lib/cmake)


# 配置编译环境
if (MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall)
endif()
if (X_SERIES_LIBRARY_TYPE STREQUAL "STATIC")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -static")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -static")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Od")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Od")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")


# 设置忽略路径
set(CMAKE_IGNORE_PATH ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
message(STATUS "CMAKE_IGNORE_PATH : ${CMAKE_IGNORE_PATH}")


# 项目依赖
include_directories(${PROJECT_SOURCE_DIR}/include)


# C++支持
set(CMAKE_CXX_STANDARD 17)


# 头文件搜索目录


# 链接库搜索目录


# 宏定义
if (X_SERIES_LIBRARY_TYPE STREQUAL "STATIC")
	add_definitions(-D_XANADU_CORE_BUILD_STATIC)
else()
	add_definitions(-D_XANADU_CORE_BUILD_SHARED)
endif()
add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-D_XANADU_EXCEPTION_OUTPUT)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
add_definitions(-D_XANADU_LOG_ENABLE)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_X_SERIES_BUILD_DEBUG)
else ()
	add_definitions(-D_X_SERIES_BUILD_RELEASE)
endif ()


# 包含源文件
aux_source_directory(./source SRC)
if (MSVC)
	list(APPEND SRC ${CMAKE_BINARY_DIR}/VersionInfo.rc)
endif ()


# 链接库
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
else ()
endif ()
if (WIN32)
	link_libraries("Ws2_32.lib")
	link_libraries("Iphlpapi.lib")
	link_libraries("Wtsapi32.lib")
	link_libraries("Userenv.lib")
elseif (MINGW)
	link_libraries("Ws2_32")
	link_libraries("Iphlpapi")
	link_libraries("Wtsapi32")
	link_libraries("Userenv")
	link_libraries("stdc++")
else ()
	if (${CMAKE_CXX_COMPILER} MATCHES "android")
		link_libraries("android")
	else()
		link_libraries("pthread")
	endif ()
	link_libraries("dl")
endif ()


# 构建
if (X_SERIES_LIBRARY_TYPE STREQUAL "STATIC")
	add_library(${PROJECT_NAME} STATIC ${SRC})
else()
	add_library(${PROJECT_NAME} SHARED ${SRC})
endif()


# 设置项目属性
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 2.0.0 SOVERSION 2)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX D)


if (X_SERIES_LIBRARY_TYPE STREQUAL "STATIC")
	# 设置安装文件(静态库)
	install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
	# 设置安装文件(动态库)
	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()
# 设置安装文件(可执行文件)
#install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# 设置安装文件(与库关联的PUBLIC头文件)
#install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
# 设置安装文件(与库关联的PRIVATE头文件)
#install(TARGETS ${PROJECT_NAME} PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
# 安装头文件
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
# 安装头文件
#install(FILES ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
# 配置模块文件
install(FILES ${CMAKE_BINARY_DIR}/Find${PROJECT_NAME}.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
