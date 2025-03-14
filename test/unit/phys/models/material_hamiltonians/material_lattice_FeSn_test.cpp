// Copyright (C) 2023 ETH Zurich
// Copyright (C) 2023 UT-Battelle, LLC
// All rights reserved.
//
// See LICENSE for terms of usage.
// See CITATION.md for citation guidelines, if DCA++ is used for scientific publications.
//
// Authors: Peter W. Doak (doakpw@ornl.gov)
//
// This file tests the specialization of material_lattice for FeSn based on test for NiO

#include "dca/phys/models/material_hamiltonians/material_lattice.hpp"

#include <cmath>
#include <complex>
#include <vector>

#include "gtest/gtest.h"

#include "dca/function/domains.hpp"
#include "dca/function/function.hpp"
#include "dca/phys/domains/cluster/cluster_domain.hpp"
#include "dca/phys/domains/cluster/cluster_domain_initializer.hpp"
#include "dca/phys/domains/cluster/symmetries/point_groups/no_symmetry.hpp"
#include "dca/phys/domains/cluster/cluster_domain_aliases.hpp"
#include "dca/phys/parameters/model_parameters.hpp"

// Helper structs since we can only template tests on types.
namespace dca {
namespace testing {
// dca::testing::

struct FeSnStruct {
  static constexpr phys::models::Material type = phys::models::Material::FeSn;
};

}  // namespace testing

}  // namespace dca

template <typename T>
class MaterialLatticeFeSnTest : public ::testing::Test {};

using FeSnTypes = ::testing::Types<dca::testing::FeSnStruct>;
TYPED_TEST_CASE(MaterialLatticeFeSnTest, FeSnTypes);

TYPED_TEST(MaterialLatticeFeSnTest, Initialize_H_0) {
  using namespace dca;

  using Lattice = phys::models::material_lattice<TypeParam::type, phys::domains::no_symmetry<3>>;

  using BandDmn = func::dmn<15, int>;
  using SpinDmn = func::dmn<2, int>;
  using BandSpinDmn = func::dmn_variadic<func::dmn_0<BandDmn>, func::dmn_0<SpinDmn>>;

  using KDmn = func::dmn<3, std::vector<double>>;
  const double a = Lattice::lattice_constant;
  KDmn::set_elements({{0., 0., 0.}, {0., M_PI / a, 0.}, {M_PI / a, M_PI / a, M_PI / a}});

  func::function<std::complex<double>, func::dmn_variadic<BandSpinDmn, BandSpinDmn, func::dmn_0<KDmn>>> H_0;

  phys::params::ModelParameters<phys::models::TightBindingModel<Lattice>> params;
  params.set_t_ij_file_name(DCA_SOURCE_DIR
                            "/include/dca/phys/models/material_hamiltonians/FeSn/t_ij_FeSn.txt");

  Lattice::initializeH0(params, H_0);

  // // All imaginary parts should be smaller than 10^-3.
  // for (int b1 = 0; b1 < BandDmn::dmn_size(); ++b1)
  //   for (int s1 = 0; s1 < SpinDmn::dmn_size(); ++s1)
  //     for (int b2 = 0; b2 < BandDmn::dmn_size(); ++b2)
  //       for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
  //         for (int k = 0; k < KDmn::dmn_size(); ++k)
  //           EXPECT_LE(std::abs(H_0(b1, s1, b2, s2, k).imag()), 1.e-3);

  // All matrix elements with different spin indices should be zero.
  for (int b1 = 0; b1 < BandDmn::dmn_size(); ++b1)
    for (int b2 = 0; b2 < BandDmn::dmn_size(); ++b2)
      for (int k = 0; k < KDmn::dmn_size(); ++k) {
        EXPECT_DOUBLE_EQ(0., H_0(b1, 0, b2, 1, k).real());
        EXPECT_DOUBLE_EQ(0., H_0(b1, 1, b2, 0, k).real());
      }

  // Check some nonvanishing Hamiltonian matrix elements.
  // EXPECT_DOUBLE_EQ(-1.7838558854405486, H_0(0, 0, 0, 0, 0).real());
  // EXPECT_DOUBLE_EQ(-4.452512149016615, H_0(5, 1, 5, 1, 2).real());
  // EXPECT_DOUBLE_EQ(1.428376402198317, H_0(6, 1, 5, 1, 2).real());
}

TYPED_TEST(MaterialLatticeFeSnTest, Initialize_H_interaction) {
  using namespace dca;

  using Lattice = phys::models::material_lattice<TypeParam::type, phys::domains::no_symmetry<3>>;

  using BandDmn = func::dmn<15, int>;
  using SpinDmn = func::dmn<2, int>;
  using BandSpinDmn = func::dmn_variadic<func::dmn_0<BandDmn>, func::dmn_0<SpinDmn>>;
  using NuNuDmn = func::dmn_variadic<BandSpinDmn, BandSpinDmn>;

  using CDA = dca::phys::ClusterDomainAliases<Lattice::DIMENSION>;
  using RClusterType = typename CDA::RClusterType;
  using RClusterDmn = typename CDA::RClusterDmn;

  const std::vector<std::vector<int>> DCA_cluster{{-2, 0, 0}, {0, -2, 0}, {0, 0, 2}};

  auto r_DCA = Lattice::initializeRDCABasis();
  phys::domains::cluster_domain_initializer<RClusterDmn>::execute(r_DCA.data(), DCA_cluster);

  // Get index of origin and check it.
  const int origin = RClusterType::origin_index();
  ASSERT_DOUBLE_EQ(0., RClusterDmn::get_elements()[origin][0]);
  ASSERT_DOUBLE_EQ(0., RClusterDmn::get_elements()[origin][1]);
  ASSERT_DOUBLE_EQ(0., RClusterDmn::get_elements()[origin][2]);

  func::function<double, func::dmn_variadic<BandSpinDmn, BandSpinDmn, RClusterDmn>> H_interaction;

  phys::params::ModelParameters<phys::models::TightBindingModel<Lattice>> params;
  params.set_U_ij_file_name(DCA_SOURCE_DIR
                            "/include/dca/phys/models/material_hamiltonians/FeSn/U_ij_FeSn.txt");

  Lattice::initializeHInteraction(H_interaction, params);

  // Check that the interaction is only on-site.
  for (int r = 0; r < RClusterDmn::dmn_size(); ++r)
    for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
      for (int b2 = 0; b2 < BandDmn::dmn_size(); ++b2)
        for (int s1 = 0; s1 < SpinDmn::dmn_size(); ++s1)
          for (int b1 = 0; b1 < BandDmn::dmn_size(); ++b1)
            if (r != origin)
              EXPECT_DOUBLE_EQ(0., H_interaction(b1, s1, b2, s2, r));

  // Check that there is no self-interaction (i.e. the diagonal in band and spin is zero).
  for (int s = 0; s < SpinDmn::dmn_size(); ++s)
    for (int b = 0; b < BandDmn::dmn_size(); ++b)
      EXPECT_NEAR(0., H_interaction(b, s, b, s, origin), 1E-20)
          << "Fail? Self interaction is non-zero for band:" << b << "  spin:" << s << '\n';
  ;

  // Check that H_interaction is symmetric in band and spin (H_interaction is real).
  for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
    for (int b2 = 0; b2 < BandDmn::dmn_size(); ++b2)
      for (int s1 = 0; s1 < s2; ++s1)
        for (int b1 = 0; b1 < b2; ++b1)
          EXPECT_NEAR(H_interaction(b1, s1, b2, s2, origin), H_interaction(b2, s2, b1, s1, origin), 1E-22)
              << "H_interaction is not symmetric in band and spin:" << b1 << " s1:" << s1 << " b2:" << b2
              << " s2:" << s2 << '\n';

  func::function<int, NuNuDmn> H_symmetry;
  Lattice::initializeHSymmetry(H_symmetry);

  for (int s = 0; s < SpinDmn::dmn_size(); s++)
    for (int i = 0; i < BandDmn::dmn_size(); i++)
      EXPECT_EQ(H_symmetry(i, s, i, s), i) << "i: " << i << "  s: " << s;

  // Check some values.
  // EXPECT_DOUBLE_EQ(6.83, H_interaction(0, 0, 1, 0, origin));
  // EXPECT_DOUBLE_EQ(9.14, H_interaction(0, 0, 0, 1, origin));
  // EXPECT_DOUBLE_EQ(6.49, H_interaction(2, 0, 4, 0, origin));
}
