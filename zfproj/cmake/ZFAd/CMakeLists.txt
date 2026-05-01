cmake_minimum_required(VERSION 3.16)
project(ZFAd)
set(ZF_ROOT_PATH "${PROJECT_SOURCE_DIR}/../../../../../../ZFFramework")
include("${ZF_ROOT_PATH}/tools/spec/cmake/util.cmake")
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

include("${PROJECT_SOURCE_DIR}/zfprojConfig.cmake" OPTIONAL)

zfprojSrcFiles(
    ZF_SRC_FILES
    "${CMAKE_BINARY_DIR}/zfgensrc_ZFAd.cpp"
    "${ZF_ROOT_PATH}"
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/../../.."
    )
if(COMMAND zfprojConfigBefore_ZFAd)
    zfprojConfigBefore_ZFAd(ZFAd ZF_SRC_FILES)
endif()
add_library(ZFAd SHARED ${ZF_SRC_FILES})
zfprojStripFILE(ZFAd)
set_target_properties(ZFAd PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(ZFAd PUBLIC ZFLIB_ZFAd=ZF_ENV_EXPORT)
if(COMMAND zfprojConfigAfter_ZFAd)
    zfprojConfigAfter_ZFAd(ZFAd)
endif()

target_include_directories(ZFAd PRIVATE "${PROJECT_SOURCE_DIR}/../../../zfsrc")

function(ZFAddLib ZF_LIB_NAME_)
    target_include_directories(ZFAd PRIVATE "${ZF_ROOT_PATH}/_release/cmake/all/include")
    target_link_directories(ZFAd PUBLIC "${ZF_ROOT_PATH}/_release/cmake/module/${ZF_LIB_NAME_}/lib")
    target_link_libraries(ZFAd PRIVATE ${ZF_LIB_NAME_})
endfunction()

ZFAddLib(ZFCore)
ZFAddLib(ZFUtility)
ZFAddLib(ZFAlgorithm)
ZFAddLib(ZFUIKit)

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/../../../zfsrc/"
    DESTINATION "${ZF_ROOT_PATH}/_release/cmake/module/ZFAd/include"
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
    )
install(
    TARGETS ZFAd
    DESTINATION "${ZF_ROOT_PATH}/_release/cmake/module/ZFAd/lib"
    )
install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/../../../zfres/"
    DESTINATION "${ZF_ROOT_PATH}/_release/cmake/module/ZFAd/zfres"
    PATTERN "ZF_PUT_RES_FILES_HERE" EXCLUDE
    )

install(
    DIRECTORY "${ZF_ROOT_PATH}/_release/cmake/module/ZFAd/"
    DESTINATION "${ZF_ROOT_PATH}/_release/cmake/all"
    )

