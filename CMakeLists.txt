cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

find_package(cetmodules)

project(trk-hit-dispatch VERSION 0.0.1)

include(CetCMakeEnv)
cet_cmake_env()
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

cet_set_compiler_flags(DIAGS VIGILANT
 WERROR
 NO_UNDEFINED
 EXTRA_FLAGS -pedantic
 -Wno-unused-parameter -Wno-type-limits -Wno-ignored-qualifiers -Wno-unused-but-set-parameter -Wno-extra -Wno-non-virtual-dtor -Wno-implicit-fallthrough -Wno-deprecated-copy
)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/TrkHitDispatch SYMBOLIC)


find_package(Offline REQUIRED EXPORT)

include(ArtDictionary)
include(BuildPlugins)

add_subdirectory(src)

install_headers(SUBDIRNAME TrkHitDispatch SUBDIRS inc)
install_fhicl(SUBDIRS fcl SUBDIRNAME TrkHitDispatch/fcl)

cet_cmake_config()
