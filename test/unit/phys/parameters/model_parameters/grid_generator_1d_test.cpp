//#include "dca/phys/parameters/model_parameters.hpp"

#include <iostream>
#include "gtest/gtest.h"
#include "dca/phys/parameters/GridGenerator1D.hpp"


using ParamGrid = dca::phys::params::ParamGrid;
using GridGenerator1D = dca::phys::params::GridGenerator1D;
TEST(GridGenerator1DTest, standard_grid){
    
    ParamGrid pGrid{0,1,1000};
    GridGenerator1D gg1d(pGrid);
    double val;
    for(int i =0; i< 1000; i++){
        if(gg1d.next(val)){
            EXPECT_NEAR(val, i*0.001, 0.001);
        }else{
	  std::cout << "val: " << val << " expect: "  << i*0.001 << " \n";
        }
    }
    EXPECT_FALSE(gg1d.next(val));

}
