
cmake_minimum_required(VERSION 3.5)

include_directories(.. ../External)
add_definitions(-D_LINUX)

set(CMAKE_CXX_STANDARD 14)

add_library(StormNet STATIC
            NetBitReader.cpp
            NetBitReaderBuffer.cpp
            NetBitReaderVector.cpp
            NetBitWriter.cpp
            NetBitWriterVector.cpp
            NetReflectionTypeDatabase.cpp
            NetTransmitter.cpp
            NetTransmitterMemory.cpp
            NetTransmitterNull.cpp
            NetTransmitterReplayStream.cpp)
            
add_library(StormNetBindingsEnet STATIC
            NetClientBackendEnet.cpp
            NetServerBackendEnet.cpp
            NetTransmitterEnet.cpp)
                        
            