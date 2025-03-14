// Copyright (C) 2018 ETH Zurich
// Copyright (C) 2018 UT-Battelle, LLC
// All rights reserved.
//
// See LICENSE for terms of usage.
// See CITATION.md for citation guidelines, if DCA++ is used for scientific publications.
//
// Author: Giovanni Balduzzi (gbalduzz@itp.phys.ethz.ch)
//
// Type definitions for statistical tests on a square lattice.

#ifndef DCA_TEST_INTEGRATION_STATISTICAL_TESTS_SQUARE_LATTICE_SQUARE_LATTICE_SETUP_HPP
#define DCA_TEST_INTEGRATION_STATISTICAL_TESTS_SQUARE_LATTICE_SQUARE_LATTICE_SETUP_HPP

#include <string>
#include <iostream>
#include <cmath>

#include "gtest/gtest.h"

#include "dca/config/threading.hpp"
#include "dca/phys/dca_data/dca_data.hpp"
#include "dca/linalg/util/util_cublas.hpp"
#include "dca/phys/dca_loop/dca_loop_data.hpp"
#include "dca/phys/dca_step/cluster_solver/ctaux/ctaux_cluster_solver.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/ctint_cluster_solver.hpp"
#include "dca/phys/dca_step/cluster_solver/stdthread_qmci/stdthread_qmci_cluster_solver.hpp"
#include "dca/phys/domains/cluster/symmetries/point_groups/2d/2d_square.hpp"
#include "dca/math/random/random.hpp"
#include "dca/math/statistical_testing/function_cut.hpp"
#include "dca/math/statistical_testing/statistical_testing.hpp"
#include "dca/phys/models/analytic_hamiltonians/square_lattice.hpp"
#include "dca/phys/models/tight_binding_model.hpp"
#include "dca/phys/parameters/parameters.hpp"
#include "dca/profiling/null_profiler.hpp"
#include "dca/util/git_version.hpp"
#include "dca/util/modules.hpp"
#include "dca/testing/dca_mpi_test_environment.hpp"
#include "dca/testing/minimalist_printer.hpp"

namespace dca {
namespace testing {
// dca::testing::

constexpr int n_frequencies = 10;

#ifdef DCA_HAVE_GPU
constexpr dca::linalg::DeviceType device = dca::linalg::GPU;
#else
constexpr dca::linalg::DeviceType device = dca::linalg::CPU;
#endif  // DCA_HAVE_GPU

const std::string test_directory =
    DCA_SOURCE_DIR "/test/integration/statistical_tests/square_lattice/";

using Model =
    dca::phys::models::TightBindingModel<dca::phys::models::square_lattice<dca::phys::domains::D4>>;
using RandomNumberGenerator = dca::math::random::StdRandomWrapper<std::mt19937_64>;

using dca::ClusterSolverId;

template <ClusterSolverId CS_NAME = ClusterSolverId::CT_AUX>
using ParametersType =
    dca::phys::params::Parameters<dca::testing::DcaMpiTestEnvironment::ConcurrencyType, Threading,
                                  dca::profiling::NullProfiler, Model, RandomNumberGenerator, CS_NAME>;

template <ClusterSolverId name>
using DcaData = dca::phys::DcaData<ParametersType<name>>;

template <ClusterSolverId name = ClusterSolverId::CT_AUX>
struct ClusterSolverSelector;
template <>
struct ClusterSolverSelector<ClusterSolverId::CT_AUX> {
  using type = dca::phys::solver::CtauxClusterSolver<device, ParametersType<ClusterSolverId::CT_AUX>,
                                                     DcaData<ClusterSolverId::CT_AUX>>;
};
template <>
struct ClusterSolverSelector<ClusterSolverId::CT_INT> {
  using type =
      dca::phys::solver::CtintClusterSolver<device, ParametersType<ClusterSolverId::CT_INT>, true>;
};
template <ClusterSolverId name = ClusterSolverId::CT_AUX>
using QuantumClusterSolver = typename ClusterSolverSelector<name>::type;

template <ClusterSolverId name = ClusterSolverId::CT_AUX>
using ThreadedSolver = dca::phys::solver::StdThreadQmciClusterSolver<QuantumClusterSolver<name>>;

using SigmaCutDomain = dca::math::util::SigmaCutDomain<dca::math::util::details::Kdmn<>>;
using SigmaDomain = dca::math::util::SigmaDomain<dca::math::util::details::Kdmn<>>;
using CovarianceDomain = dca::math::util::CovarianceDomain<dca::math::util::details::Kdmn<>>;
using dca::math::util::cutFrequency;

}  // namespace testing
}  // namespace dca

#endif  // DCA_TEST_INTEGRATION_STATISTICAL_TESTS_SQUARE_LATTICE_SQUARE_LATTICE_SETUP_HPP
