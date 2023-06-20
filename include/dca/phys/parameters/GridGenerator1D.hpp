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
        };
        bool next(double& value) const;
        double getCurrent() const;
        //std::vector<double> getGrid_of_U() const;
        
        
    private:
        ParamGrid paramGrid_;
        double current_;
        std::vector<double> grid_of_U;
};




//check whether there exists new value. 
bool GridGenerator1D::next(double& value) const{
    auto it = std::find(grid_of_U.begin(), grid_of_U.end(), value);

    if(it != grid_of_U.end() && std::next(it) != grid_of_U.end()){
        return true;
    }

    return false;

}


//gets the current value that the iterator points to in the grid, and updates the iterator
double GridGenerator1D::getCurrent() const{
   auto it = grid_of_U.begin();   
     //what happens after array is ended?some garbabe value resides within the pointer
    if(it != grid_of_U.end()){
       return *it;
    }else{
        return *grid_of_U.end();
    }
    
}


//return the vector that contains Us that were initialzied during constructor initialization
// std::vector<double> GridGenerator1D::getGrid_of_U() const{
//     return grid_of_U;
// }




        }
    }
}
