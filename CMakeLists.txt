cmake_minimum_required (VERSION 3.14)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(BOT_NAME "capbot")

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(${BOT_NAME})

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")
# message(STATUS "src: ${coresrc}\nsrc/db: ${dbsrc}")
message(STATUS "src: ${SOURCE_FILES}")
# add_executable(${BOT_NAME} ${coresrc} ${dbsrc})
add_executable(${BOT_NAME} ${SOURCE_FILES})

string(ASCII 27 Esc)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set_target_properties(${BOT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    # CXX_FLAGS "-fcoroutines"
)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(DPP)
if(APPLE)
	if(CMAKE_APPLE_SILICON_PROCESSOR)
		set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
	else()
		set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
	endif()
	find_package(OpenSSL REQUIRED)
else()
	find_package(OpenSSL REQUIRED)
endif()

target_include_directories(${BOT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
)

target_compile_definitions(${BOT_NAME}
    PUBLIC DPP_CORO=ON
)

# this is for clang/clang++ users, and Docker
target_compile_options(${BOT_NAME} PRIVATE -stdlib=libc++)

target_link_options(${BOT_NAME} PRIVATE -stdlib=libc++)

# target_compile_options(${BOT_NAME}
#     PRIVATE -fcoroutines
# )


target_link_libraries(${BOT_NAME}
    dl
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_CRYPTO_LIBRARY} 
    ${OPENSSL_SSL_LIBRARY}
    fmt::fmt
)

if (DPP_FOUND)
    target_link_libraries(${BOT_NAME} ${DPP_LIBRARIES})
    target_include_directories(${BOT_NAME} PUBLIC ${DPP_INCLUDE_DIR})
else()
    message(WARNING "Could not find DPP install. Building from source instead.")
    option(DPP_BUILD_TEST "" OFF)
    include(FetchContent)

    FetchContent_Declare(
            libdpp
            GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git
            GIT_TAG master)

    FetchContent_GetProperties(libdpp)
    if(NOT libdpp_POPULATED)
        FetchContent_Populate(libdpp)
        target_include_directories(${BOT_NAME} PUBLIC
            ${libdpp_SOURCE_DIR}/include
        )
        add_subdirectory(
            ${libdpp_SOURCE_DIR}
            ${libdpp_BINARY_DIR}
            EXCLUDE_FROM_ALL)
    endif()

    target_link_libraries(${BOT_NAME} dpp)
endif()