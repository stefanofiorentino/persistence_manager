function(fetch_content_http_parser)
    include(FetchContent)

    FetchContent_Declare(
            http_parser
            GIT_REPOSITORY https://github.com/nodejs/http-parser.git
            GIT_TAG v2.9.4
            GIT_SHALLOW 1
    )

    FetchContent_GetProperties(http_parser)

    if(NOT http_parser_POPULATED)
        FetchContent_Populate(http_parser)
    endif()

endfunction()