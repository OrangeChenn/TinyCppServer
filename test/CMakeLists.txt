file(GLOB ORANGE_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")
######################################################################################################################
# DEPENDENCIES
######################################################################################################################

######################################################################################################################
# MAKE TARGETS
######################################################################################################################

##########################################
# "make check-tests"
##########################################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

##########################################
# "make server client ..."
##########################################
foreach (orange_test_source ${ORANGE_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(orange_test_filename ${orange_test_source} NAME)
    string(REPLACE ".cpp" "" orange_test_name ${orange_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${orange_test_name} EXCLUDE_FROM_ALL ${orange_test_source})
    add_dependencies(build-tests ${orange_test_name})
    add_dependencies(check-tests ${orange_test_name})

    target_link_libraries(${orange_test_name} orange_shared)

    # Set test target properties and dependencies.
    set_target_properties(${orange_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        COMMAND ${orange_test_name}
    )

endforeach(orange_test_source ${ORANGE_TEST_SOURCES})