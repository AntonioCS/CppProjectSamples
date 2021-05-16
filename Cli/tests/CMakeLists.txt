project(unit_tests DESCRIPTION "Tests")


#file(GLOB_RECURSE TEST_FILES_SRC CONFIGURE_DEPENDS
#	src/main.cpp
#    src/MyCodeLibTest.cpp
#)

add_executable(unit_tests
    src/main.cpp
    src/MyCodeLibTest.cpp
)

target_compile_options(unit_tests
 PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Werror -Wall -Wextra
	>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W4
	>
)

target_include_directories(unit_tests
	PRIVATE
		${PROJECT_INCLUDE_DIR}
)

find_package(Catch2 CONFIG REQUIRED)

target_link_libraries(unit_tests
	PRIVATE
        Catch2::Catch2
        common
)

include(Catch)
