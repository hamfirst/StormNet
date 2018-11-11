cmake_minimum_required(VERSION 3.1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/External)
set(CMAKE_CXX_STANDARD 17)

set(GENERIC_SRC_StormNet 
            NetReflectionBitBuffer.cpp
            NetBitReaderVector.cpp
            NetRandom.cpp
            NetTransmitter.cpp
            NetBitWriterVector.cpp
            NetBitWriter.cpp
            NetTransmitterReplayStream.cpp
            NetBitReaderBuffer.cpp
            NetTransmitterMemory.cpp
            NetReflectionTypeDatabase.cpp
            NetBitReader.cpp
            NetTransmitterNull.cpp
            )

set(GENERIC_HEADER_StormNet 
            NetReflectionNumber.h
            NetReflection.h
            NetProtocol.h
            NetStateStore.h
            NetReflectionList.h
            NetReflectionPolymorphic.h
            NetServerBackend.h
            NetPipeVariant.h
            NetServerInterface.h
            NetReflectionRandom.h
            NetPipeFullState.h
            NetBitUtil.h
            NetProtocolReceiver.h
            NetClientInterface.h
            NetSerializeDelta.h
            NetReflectionString.h
            NetPipeMode.h
            NetProtocolFuncs.h
            NetServerBackendEnet.h
            NetReflectionTypeDatabase.h
            NetMetaUtil.h
            NetTransmitter.h
            NetReflectionTuple.h
            NetBindings.h
            NetReflectionMap.h
            NetClientBackendEnet.h
            NetReflectionPtr.h
            NetReflectionFixedPointLUT.h
            NetReflectionStaticList.h
            NetReflectionTypeDatabaseRegister.h
            NetReplayStreamReader.h
            NetBitReaderVector.h
            NetReflectionEnum.h
            NetReflectionFixedPointRange.h
            NetDeserialize.h
            NetReflectionBitBuffer.h
            NetBindingHelpers.h
            NetLerp.h
            NetTransmitterNull.h
            NetReflectionStruct.h
            NetBitReader.h
            NetServer.h
            NetSerialize.h
            NetReflectionFixedPoint.h
            NetMessageSender.h
            NetPipeBuffer.h
            NetTransmitterEnet.h
            NetTransmitterReplayStream.h
            NetClient.h
            NetReflectionCommon.h
            NetReflectionArrayList.h
            NetBitWriterVector.h
            NetPipeMessage.h
            NetPipeFullStateSeq.h
            NetReflectionOptional.h
            NetRandom.h
            NetClientBackend.h
            NetBitWriter.h
            NetPipeDeltaStateReliable.h
            NetException.h
            NetPipeDeltaState.h
            NetBitReaderBuffer.h
            NetProtocolSender.h
            NetDeserializeDelta.h
            NetTransmitterMemory.h
            NetReflectionSparseList.h
            NetReflectionDependent.h NetPoolAllocator.h)
           

add_library(StormNet STATIC ${GENERIC_SRC_StormNet} ${GENERIC_HEADER_StormNet})

if(MSVC)
  target_sources(StormNet INTERFACE $<INSTALL_INTERFACE:StormNet.natvis> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/StormNet.natvis>)
endif()

if(NOT WEB)

  add_library(StormNetBindingsEnet STATIC
    ./NetClientBackendEnet.cpp
    ./NetServerBackendEnet.cpp
    ./NetTransmitterEnet.cpp
    )

endif()
