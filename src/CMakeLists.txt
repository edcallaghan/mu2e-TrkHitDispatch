cet_make_library(
    SOURCE
      SConscript
      RobustDispatcher.cc

    LIBRARIES PUBLIC
      Offline::RecoDataProducts

    LIBRARY_NAME trkhitdispatch
)

art_dictionary( NO_CHECK_CLASS_VERSION # For some reason this segfaults
    CLASSES_DEF_XML ${CMAKE_CURRENT_SOURCE_DIR}/classes_def.xml
    CLASSES_H ${CMAKE_CURRENT_SOURCE_DIR}/classes.h
     DICTIONARY_LIBRARIES
      trk-hit-dispatch::trkhitdispatch
      Offline::RecoDataProducts
)

cet_build_plugin(TrkHitDispatch art::module
    REG_SOURCE TrkHitDispatch_module.cc
    LIBRARIES REG
      trk-hit-dispatch::trkhitdispatch
      Offline::RecoDataProducts
)

install_source(SUBDIRS src)
