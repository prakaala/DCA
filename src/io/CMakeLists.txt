add_subdirectory(hdf5)
add_subdirectory(json)

#don't use this variable use the dca_io target
set(DCA_IO_LIBS "dca_hdf5;json")

if (DCA_HAVE_ADIOS2)
  add_subdirectory(adios2)
  set(DCA_IO_LIBS "${DCA_IO_LIBS};dca_adios2")
endif()

add_library(dca_io reader.cpp io_types.cpp)

dca_gpu_runtime_link(dca_io)
dca_gpu_blas_link(dca_io)

target_link_libraries(dca_io PUBLIC ${DCA_IO_LIBS})
