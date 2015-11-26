

if(DEFINED __INCLUDED_GR_PLATFORM_CMAKE)
    return()
endif()
set(__INCLUDED_GR_PLATFORM_CMAKE TRUE)

########################################################################
# Setup additional defines for OS types
########################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif()

if(LINUX AND EXISTS "/etc/debian_version")
    set(DEBIAN TRUE)
endif()

if(LINUX AND EXISTS "/etc/redhat-release")
    set(REDHAT TRUE)
endif()

if(LINUX AND EXISTS "/etc/slackware-version")
    set(SLACKWARE TRUE)
endif()

########################################################################
# when the library suffix should be 64 (applies to redhat linux family)
########################################################################
if (REDHAT OR SLACKWARE)
    set(LIB64_CONVENTION TRUE)
endif()

if(NOT DEFINED LIB_SUFFIX AND LIB64_CONVENTION AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    set(LIB_SUFFIX 64)
endif()
set(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")
