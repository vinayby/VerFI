cmake_minimum_required (VERSION 3.9)

# projectname is the same as the main-executable
project(verfi_a)

add_definitions('-Wall')
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
add_definitions(-DALL_SIMS_USE_SAME_RND_DECISIONS)
#set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(OpenMP)

add_executable(${PROJECT_NAME} VerFI.cpp ReadDesignFile.cpp Util.cpp SimulationTools.cpp ReadLibraryFile.cpp ReadCellReportFile.cpp)

if(OpenMP_CXX_FOUND)
  #target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX curses)
  target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX )
endif()
add_custom_target(${PROJECT_NAME}-symlink ALL ln --force -s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})
