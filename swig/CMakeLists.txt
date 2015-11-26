

########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs 2)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)

set(GR_SWIG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)

########################################################################
# Setup swig generation
########################################################################
foreach(incdir ${GNURADIO_RUNTIME_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/gnuradio/swig)
endforeach(incdir)

set(GR_SWIG_LIBRARIES gnuradio-ieee802_22)
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/ieee802_22_swig_doc.i)
set(GR_SWIG_DOC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ieee802-22/
    ${CMAKE_CURRENT_BINARY_DIR}/../include/ieee802-22/
)

set(GR_SWIG_TARGET_DEPS ieee802_22_generated_includes)

GR_SWIG_MAKE(ieee802_22_swig ieee802_22_swig.i)

########################################################################
# Install the build swig module
########################################################################
GR_SWIG_INSTALL(TARGETS ieee802_22_swig DESTINATION ${GR_PYTHON_DIR}/ieee802_22)

########################################################################
# Install swig .i files for development
########################################################################
install(
    FILES
    ieee802_22_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/ieee802_22_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/ieee802_22/swig
)
