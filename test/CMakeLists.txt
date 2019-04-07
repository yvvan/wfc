add_subdirectory (googletest)
enable_testing()
include_directories(
		PUBLIC include
		PUBLIC ${gtest_SOURCE_DIR})

################################
# Unit Tests
################################
# Add test cpp file

add_executable( runUnitTests
  src/main.cpp

  src/overlapping_pattern_extraction_test.cpp
  src/extraction_sample_data_test.cpp
)

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests
	PUBLIC gtest
	PUBLIC gtest_main
	PUBLIC wfc
	)

add_test( runUnitTests runUnitTests )
