set(TEST_INCLUDES ${DCA_INCLUDE_DIRS};${PROJECT_SOURCE_DIR})
set(TEST_LIBS     ${DCA_LIBS})

dca_add_gtest(ct_int_solver_methods_test
  FAST
  CUDA
  GTEST_MAIN
  INCLUDE_DIRS ${TEST_INCLUDES}
  LIBS     FFTW::Double ${TEST_LIBS} ctint g0_interpolation
)

add_subdirectory(structs)
add_subdirectory(walker)
