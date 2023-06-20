#include <optional>
#include <vector>
#include <algorithm>


namespace dca{
    namespace phys{
        namespace params{


  struct ParamGrid{
    double start; 
    double end;
    int step;
  };

class GridGenerator1D{
    public:
        GridGenerator1D(struct ParamGrid pGrid): paramGrid_(pGrid){
             for(int i = 0; i < pGrid.step; i++){
                grid_of_U.push_back(pGrid.start + (pGrid.end-pGrid.start)*i/pGrid.step);
            }
	     it_ = grid_of_U.begin();
        };
        bool next(double& value);
        double getCurrent() const;
        //std::vector<double> getGrid_of_U() const;
        
        
    private:
  std::vector<double>::iterator it_;
        ParamGrid paramGrid_;
        double current_;
        std::vector<double> grid_of_U;
};




//check whether there exists new value. 
bool GridGenerator1D::next(double& value){
  if (it_ == grid_of_U.end())
    return false;
  value = *it_++;
  return true;
}


//gets the current value that the iterator points to in the grid, and updates the iterator
double GridGenerator1D::getCurrent() const{
  return *it_;
}


//return the vector that contains Us that were initialzied during constructor initialization
// std::vector<double> GridGenerator1D::getGrid_of_U() const{
//     return grid_of_U;
// }




        }
    }
}
