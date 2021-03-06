

if(DEFINED __INCLUDED_GR_VERSION_CMAKE)
    return()
endif()
set(__INCLUDED_GR_VERSION_CMAKE TRUE)

#eventually, replace version.sh and fill in the variables below
set(MAJOR_VERSION ${VERSION_INFO_MAJOR_VERSION})
set(API_COMPAT    ${VERSION_INFO_API_COMPAT})
set(MINOR_VERSION ${VERSION_INFO_MINOR_VERSION})
set(MAINT_VERSION ${VERSION_INFO_MAINT_VERSION})

########################################################################
# Extract the version string from git describe.
########################################################################
find_package(Git)

if(GIT_FOUND AND EXISTS ${CMAKE_SOURCE_DIR}/.git)
    message(STATUS "Extracting version information from git describe...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --always --abbrev=8 --long
        OUTPUT_VARIABLE GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    set(GIT_DESCRIBE "v${MAJOR_VERSION}.${API_COMPAT}.x-xxx-xunknown")
endif()

########################################################################
# Use the logic below to set the version constants
########################################################################
if("${MINOR_VERSION}" STREQUAL "git")
    # VERSION: 3.3git-xxx-gxxxxxxxx
    # DOCVER:  3.3git
    # LIBVER:  3.3git
    set(VERSION "${GIT_DESCRIBE}")
    set(DOCVER  "${MAJOR_VERSION}.${API_COMPAT}${MINOR_VERSION}")
    set(LIBVER  "${MAJOR_VERSION}.${API_COMPAT}${MINOR_VERSION}")
    set(RC_MINOR_VERSION "0")
    set(RC_MAINT_VERSION "0")
elseif("${MAINT_VERSION}" STREQUAL "git")
    # VERSION: 3.3.1git-xxx-gxxxxxxxx
    # DOCVER:  3.3.1git
    # LIBVER:  3.3.1git
    set(VERSION "${GIT_DESCRIBE}")
    set(DOCVER  "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}${MAINT_VERSION}")
    set(LIBVER  "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}${MAINT_VERSION}")
    math(EXPR RC_MINOR_VERSION "${MINOR_VERSION} - 1")
    set(RC_MAINT_VERSION "0")
else()
    # This is a numbered release.
    # VERSION: 3.3.1{.x}
    # DOCVER:  3.3.1{.x}
    # LIBVER:  3.3.1{.x}
    if("${MAINT_VERSION}" STREQUAL "0")
        set(VERSION "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}")
    else()
        set(VERSION "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}.${MAINT_VERSION}")
    endif()
    set(DOCVER "${VERSION}")
    set(LIBVER "${VERSION}")
    set(RC_MINOR_VERSION ${MINOR_VERSION})
    set(RC_MAINT_VERSION ${MAINT_VERSION})
endif()
