function(glsl_to_cpp_header ARG_GLSL_FILE ARG_CPP_VARIABLE_NAME OUT_CPP_HEADER_FILE)
    # Compute output header path
    get_filename_component(GLSL_FILE_NAME_WE ${ARG_GLSL_FILE} NAME_WE)
    set(CPP_HEADER_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/${GLSL_FILE_NAME_WE}.h")
    set(${OUT_CPP_HEADER_FILE} ${CPP_HEADER_FILENAME} PARENT_SCOPE)

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${ARG_GLSL_FILE}.cmake"
            "file(READ \"${CMAKE_CURRENT_SOURCE_DIR}/${ARG_GLSL_FILE}\" SHADER_CONTENT)\n"
            "file(WRITE \"${CPP_HEADER_FILENAME}\" \"const char* ${ARG_CPP_VARIABLE_NAME} = R\\\"(\${SHADER_CONTENT})\\\";\")\n"
    )

    # Add a custom command to generate the header
    add_custom_command(
            OUTPUT ${CPP_HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/${ARG_GLSL_FILE}.cmake"
            DEPENDS ${ARG_GLSL_FILE}
            COMMENT "Embedding shader ${ARG_GLSL_FILE} into ${CPP_HEADER_FILENAME}"
            VERBATIM
    )

    add_custom_target(embed_shader_${GLSL_FILE_NAME_WE} ALL DEPENDS ${CPP_HEADER_FILENAME})
endfunction()