// Copyright (C) 2018 ETH Zurich
// Copyright (C) 2018 UT-Battelle, LLC
// All rights reserved.
// See LICENSE.txt for terms of usage./
//  See CITATION.md for citation guidelines, if DCA++ is used for scientific publications.
//
// Author: Giovanni Balduzzi (gbalduzz@itp.phys.ethz.ch)
//         Jérémie Bouquet (bouquetj@gmail.com)
//
//

#ifndef TEST_UNIT_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_WALKER_WALKER_WRAPPER_SUBMATRIX_HPP
#define TEST_UNIT_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_WALKER_WALKER_WRAPPER_SUBMATRIX_HPP

#include "dca/config/haves_defines.hpp"
#include "dca/distribution/dist_types.hpp"
#include "dca/linalg/device_type.hpp"
#include "dca/phys/dca_data/dca_data.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/walker/ctint_walker_cpu_submatrix.hpp"
#ifdef DCA_HAVE_GPU
#include "dca/phys/dca_step/cluster_solver/ctint/walker/ctint_walker_gpu_submatrix.hpp"
#endif

namespace testing {
namespace phys {
namespace solver {
namespace ctint {
// testing::phys::solver::ctint::

using namespace dca::phys::solver::ctint;
using dca::linalg::CPU;
using dca::linalg::GPU;
using dca::linalg::DeviceType;
using dca::DistType;

template <class Parameters, dca::linalg::DeviceType device, typename Real, DistType DIST = DistType::NONE>
struct WalkerSelector;

template <class Parameters, typename Real, DistType DIST>
struct WalkerSelector<Parameters, CPU, Real, DIST> {
  // Fix rng order for testing.
  using type = CtintWalkerSubmatrixCpu<Parameters, Real, DIST>;
};

#ifdef DCA_HAVE_GPU
template <class Parameters, typename Real, DistType DIST>
struct WalkerSelector<Parameters, GPU, Real, DIST> {
  using type = CtintWalkerSubmatrixGpu<Parameters, Real, DIST>;
};
#endif  // DCA_HAVE_GPU

using namespace dca::phys::solver::ctint;
template <class Parameters, DeviceType device_t = CPU, typename Real = double, DistType DIST = DistType::NONE>
struct WalkerWrapperSubmatrix : public WalkerSelector<Parameters, device_t, Real, DIST>::type {
  using BaseClass = typename WalkerSelector<Parameters, device_t, Real, DIST>::type;
  using Rng = typename BaseClass::Rng;
  using Data = typename BaseClass::Data;

  WalkerWrapperSubmatrix(/*const*/ Parameters& parameters_ref, Rng& rng_ref)
      : BaseClass(parameters_ref, dca::phys::DcaData<Parameters>(parameters_ref), rng_ref, 0),
        streams_(3) {
    BaseClass::initialize(0);

  }

  // This purposefully shadows
  void doStep(const int n_steps_to_delay) {
    BaseClass::doStep(n_steps_to_delay);
  }

  using Matrix = dca::linalg::Matrix<Real, CPU>;
  using MatrixPair = std::array<Matrix, 2>;

  MatrixPair getM() {
    std::array<dca::linalg::Matrix<Real, device_t>, 2> M;

    BaseClass::computeM(M);
#ifdef DCA_HAVE_GPU
    cudaDeviceSynchronize();
#endif

    std::array<dca::linalg::Matrix<Real, CPU>, 2> M_copy{M[0], M[1]};
    return M_copy;
  }

  using BaseClass::setMFromConfig;

  const auto& getWalkerConfiguration() const {
    return BaseClass::configuration_;
  }

  Real getAcceptanceProbability() const {
    return BaseClass::acceptance_prob_;
  }

private:
  std::vector<dca::linalg::util::GpuStream> streams_;
};

}  // namespace ctint
}  // namespace solver
}  // namespace phys
}  // namespace testing

#endif  //  TEST_UNIT_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_WALKER_WALKER_WRAPPER_SUBMATRIX_HPP
