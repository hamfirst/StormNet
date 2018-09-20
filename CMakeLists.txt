cmake_minimum_required(VERSION 3.0)

include_directories(. ../. .././External)
set(CMAKE_CXX_STANDARD 17)

if (WEB)
  add_definitions(-D_WEB)
  set(PLATFORM_SRC_StormNet 
            )
elseif (MSVC)
  add_definitions(-D_WINDOWS)
  include_directories(.././External/Windows)
  set(PLATFORM_SRC_StormNet 
            )
elseif (APPLE)
  add_definitions(-D_MACOS)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_StormNet 
            )
elseif (UNIX)
  add_definitions(-D_LINUX)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_StormNet 
            )
endif()

add_library(StormNet STATIC
            ./NetBitReader.cpp
            ./NetBitReaderBuffer.cpp
            ./NetBitReaderVector.cpp
            ./NetBitWriter.cpp
            ./NetBitWriterVector.cpp
            ./NetRandom.cpp
            ./NetReflectionTypeDatabase.cpp
            ./NetTransmitter.cpp
            ./NetTransmitterMemory.cpp
            ./NetTransmitterNull.cpp
            ./NetTransmitterReplayStream.cpp
            )
            

if(NOT WEB)       
     
  add_library(StormNetBindingsEnet STATIC           
              ./NetClientBackendEnet.cpp
              ./NetServerBackendEnet.cpp
              ./NetTransmitterEnet.cpp            
              )      

endif()                  

                   