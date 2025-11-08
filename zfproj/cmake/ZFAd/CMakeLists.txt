cmake_minimum_required(VERSION 3.16)
project(ZFAd)
set(ZF_ROOT_PATH "${PROJECT_SOURCE_DIR}/../../../../../../ZFFramework")
include("${ZF_ROOT_PATH}/tools/spec/cmake/util.cmake")
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

include("${PROJECT_SOURCE_DIR}/zfprojConfig.cmake" OPTIONAL)

file(GLOB_RECURSE ZF_SRC_FILES RELATIVE "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/../../../zfsrc/*.c"
    "${PROJECT_SOURCE_DIR}/../../../zfsrc/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../../zfsrc_ext/*.c"
    "${PROJECT_SOURCE_DIR}/../../../zfsrc_ext/*.cpp"
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
    target_link_libraries(ZFAd ${ZF_LIB_NAME_})
endfunction()

ZFAddLib(ZFCore)
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
if(WIN32)
    execute_process(COMMAND "${ZF_ROOT_PATH}\\tools\\util\\copy_res.bat" "${PROJECT_SOURCE_DIR}\\..\\..\\..\\zfres\\" "${ZF_ROOT_PATH}\\_release\\cmake\\module\\ZFAd\\zfres")
else()
    execute_process(COMMAND sh "${ZF_ROOT_PATH}/tools/util/copy_res.sh" "${PROJECT_SOURCE_DIR}/../../../zfres/" "${ZF_ROOT_PATH}/_release/cmake/module/ZFAd/zfres")
endif()

install(
    DIRECTORY "${ZF_ROOT_PATH}/_release/cmake/module/ZFAd/"
    DESTINATION "${ZF_ROOT_PATH}/_release/cmake/all"
    )

