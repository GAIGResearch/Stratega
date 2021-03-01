# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)

add_executable (Tests "main.cpp" "include/FMEvaluator.h" "include/FMEvaluationResults.h" "src/FMEvaluator.cpp" "src/FMEvaluationResults.cpp")
target_include_directories(Tests PUBLIC include)
target_link_libraries(Tests Stratega)

find_package(Threads)
if(TARGET Threads::Threads)
      target_link_libraries(Tests Threads::Threads)
endif()
