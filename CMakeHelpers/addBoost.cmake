cmake_minimum_required(VERSION 2.8)

if(Y_BOOST_LIB_INCLUDE)
	set(Boost_LIBRARIES ${Y_BOOST_LIB_PATH})
	set(Boost_INCLUDE_DIRS ${Y_BOOST_LIB_INCLUDE})
	set(SX_BOOST_LIB_PATH ${Y_BOOST_LIB_PATH})
	set(SX_BOOST_LIB_INCLUDE ${Y_BOOST_LIB_INCLUDE})
	
	include_directories(${Boost_INCLUDE_DIRS})
	message("Y_BOOST_LIB_INCLUDE is set by user.")
	message("Path to libraries: ${Boost_LIBRARIES}")
	message("Path to includes: ${Boost_INCLUDE_DIRS}")
elseif(SX_BOOST_LIB_INCLUDE)
	set(Boost_LIBRARIES ${SX_BOOST_LIB_PATH})
	set(Boost_INCLUDE_DIRS ${SX_BOOST_LIB_INCLUDE})
	
	include_directories(${Boost_INCLUDE_DIRS})
	message("SX_BOOST_LIB_INCLUDE is set by user.")
	message("Path to libraries: ${Boost_LIBRARIES}")
	message("Path to includes: ${Boost_INCLUDE_DIRS}")
	
else()
	message("SX_BOOST_LIB_INCLUDE is not set by user. Searching for boost libraries.")
	set(Boost_USE_MULTITHREADED ON)
	find_package(Boost COMPONENTS date_time filesystem system program_options chrono thread random unit_test_framework)
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
		message("Boost found.")
		message("Path to libraries: ${Boost_LIBRARIES}")
		message("Path to includes: ${Boost_INCLUDE_DIRS}")
	else()
		message(STATUS "Boost libraries not found!")
	endif()
endif()
