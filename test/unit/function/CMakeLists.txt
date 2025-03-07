# Function library's unit tests

add_subdirectory(domains)
add_subdirectory(util)

dca_add_gtest(function_library_test
  GTEST_MAIN
  LIBS function)

if (function_library_test)
  # We want to test self-assignment and self-move.
  target_compile_options(function_library_test PRIVATE -Wno-self-assign-overloaded -Wno-self-move)
endif()

dca_add_gtest(set_to_zero_test GTEST_MAIN)

set(FUNCTION_CONSTRUCTION_TEST_LIBS "parallel_mpi_concurrency;function")
if(DCA_HAVE_GPU)
  set(FUNCTION_CONSTRUCTION_TEST_LIBS "gpu_utils;${FUNCTION_CONSTRUCTION_TEST_LIBS}")
endif(DCA_HAVE_GPU)

dca_add_gtest(function_construction_test
  MPI MPI_NUMPROC 4
  LIBS ${FUNCTION_CONSTRUCTION_TEST_LIBS})

dca_add_gtest(function_slice_test
  GTEST_MAIN
  LIBS function)
