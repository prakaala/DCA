#include "dca/phys/parameters/model_parameters.hpp"
#include "gtest/gtest.h"
#include "dca/phys/parameters/GridGenerator1D.hpp"

TEST(GridGenerator1DTest, standard grid){
    
    ParamGrid pGrid{0,10,1000};
    GridGenerator1D gg1d(pGrid);
    double val;
    for(int i =0; i< 1000, i++){
        if(gg1d.next(val)){
            EXPECT_EQ(val, i*0.001);
        }else{
            FAIL();
        }
    }
    EXPECT_FALSE(gg1d.next(val));

}