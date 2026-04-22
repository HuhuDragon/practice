cmake_minimum_required(VERSION 3.16)
project(GasDetectorSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geant4 REQUIRED)
find_package(yaml-cpp REQUIRED)

include(${Geant4_USE_FILE})

add_executable(gas_detector
    src/main.cc
    src/Config.cc
    src/DetectorConstruction.cc
    src/PrimaryGeneratorAction.cc
    src/EventAction.cc
    src/SteppingAction.cc
    src/RunAction.cc
    src/ActionInitialization.cc
)

target_include_directories(gas_detector PRIVATE include)
target_link_libraries(gas_detector PRIVATE ${Geant4_LIBRARIES} yaml-cpp)
