
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
            NetClientBackendEnet.cpp
            NetReflectionTypeDatabase.cpp
            NetServerBackendEnet.cpp
            NetTransmitter.cpp
            NetTransmitterEnet.cpp
            NetTransmitterMemory.cpp
            NetTransmitterNull.cpp
            NetTransmitterReplayStream.cpp)
            