set(EXEC_NAME Tubi_Subs)
add_executable(
    ${EXEC_NAME}
    main.cpp
)

target_link_libraries( 
    ${EXEC_NAME}
    PRIVATE
    bfg::lyra
    spdlog::spdlog
    ${PROJ_NAME}_project_warnings
    ${PROJ_NAME}_project_libraries
    ${PROJ_NAME}_project_options
)

################################################################################
# Installation
################################################################################
include(GNUInstallDirs)

install(
   TARGETS             ${EXEC_NAME}
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)