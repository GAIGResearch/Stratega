# CMakeList.txt : CMake project for StrategyGameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_executable (Tests "main.cpp" "include/FMEvaluator.h" "include/FMEvaluationResults.h" "src/FMEvaluator.cpp" "src/FMEvaluationResults.cpp")
target_include_directories(Tests PUBLIC include)
target_link_libraries(Tests Stratega)
