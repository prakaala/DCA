// Copyright (C) 2018 ETH Zurich
// Copyright (C) 2018 UT-Battelle, LLC
// All rights reserved.
//
// See LICENSE for terms of usage.
// See CITATION.md for citation guidelines, if DCA++ is used for scientific publications.
//
// Author: Peter Staar (taa@zurich.ibm.com)
//
// Template specialization of model parameters for the single-band Hubbard model.


template <typename Lattice>
class ModelParameters<models::TightBindingModel<Lattice>> {

public:
  ModelParameters() : t_(0.), t_prime_(0.), U_(0.), V_(0.), V_prime_(0.) {}

  template <typename Concurrency>
  int getBufferSize(const Concurrency& concurrency) const;
  template <typename Concurrency>
  void pack(const Concurrency& concurrency, char* buffer, int buffer_size, int& position) const;
  template <typename Concurrency>
  void unpack(const Concurrency& concurrency, char* buffer, int buffer_size, int& position);

  template <typename ReaderOrWriter>
  void readWrite(ReaderOrWriter& reader_or_writer);

  double get_t() const {
    return t_;
  }
  void set_t(const double t) {
    t_ = t;
  }

  double get_t_prime() const {
    return t_prime_;
  }
  void set_t_prime(const double t_prime) {
    t_prime_ = t_prime;
  }

  double get_U() const {
    return U_;
  }
  void set_U(const double U) {
    U_ = U;
  }

  double get_V() const {
    return V_;
  }
  void set_V(const double V) {
    V_ = V;
  }

  double get_V_prime() const {
    return V_prime_;
  }
  void set_V_prime(const double V_prime) {
    V_prime_ = V_prime;
  }
 

  // void generate(const struct ParamGrid inputObject) {
  //   Generator<double> generator([](){
  //     return 0.0;
  //   });
  //   for(double value: generator.makeIterator(inputObject.start, inputObject.end , inputObject.step)){
  //     set_U(value);
  //     return;
  //   }
  // }
  
  
  
  // void getUs(){
  //   double x;
  //   for(int i = 1; i< steps; i++){
  //      x = start + i*(end-start)/steps;
  //     set_U(x);
  //   }
  // }



private:
  double t_;
  double t_prime_;
  double U_;
  double V_;
  double V_prime_;
  std::optional<dca::phys::params::GridGenerator1D> u_generator_;
  std::vector<double> u_input_; // we need to retain this so it can be written back to output
};

template <typename Lattice>
template <typename Concurrency>
int ModelParameters<models::TightBindingModel<Lattice>>::getBufferSize(
    const Concurrency& concurrency) const {
  int buffer_size = 0;

  buffer_size += concurrency.get_buffer_size(t_);
  buffer_size += concurrency.get_buffer_size(t_prime_);
  buffer_size += concurrency.get_buffer_size(U_);
  buffer_size += concurrency.get_buffer_size(V_);
  buffer_size += concurrency.get_buffer_size(V_prime_);

  return buffer_size;
}

template <typename Lattice>
template <typename Concurrency>
void ModelParameters<models::TightBindingModel<Lattice>>::pack(const Concurrency& concurrency,
                                                               char* buffer, int buffer_size,
                                                               int& position) const {
  concurrency.pack(buffer, buffer_size, position, t_);
  concurrency.pack(buffer, buffer_size, position, t_prime_);
  concurrency.pack(buffer, buffer_size, position, U_);
  concurrency.pack(buffer, buffer_size, position, V_);
  concurrency.pack(buffer, buffer_size, position, V_prime_);
}

template <typename Lattice>
template <typename Concurrency>
void ModelParameters<models::TightBindingModel<Lattice>>::unpack(const Concurrency& concurrency,
                                                                 char* buffer, int buffer_size,
                                                                 int& position) {
  concurrency.unpack(buffer, buffer_size, position, t_);
  concurrency.unpack(buffer, buffer_size, position, t_prime_);
  concurrency.unpack(buffer, buffer_size, position, U_);
  concurrency.unpack(buffer, buffer_size, position, V_);
  concurrency.unpack(buffer, buffer_size, position, V_prime_);
}

template <typename Lattice>
template <typename ReaderOrWriter>
void ModelParameters<models::TightBindingModel<Lattice>>::readWrite(ReaderOrWriter& reader_or_writer) {
  try {
    reader_or_writer.open_group("single-band-Hubbard-model");
  }
  catch (...) {
    throw std::runtime_error("You must specify a single-band-Hubbard-model in input!");
  }

  try {
    reader_or_writer.execute("t", t_);
  }
  catch (const std::exception& r_e) {
  }
  try {
    reader_or_writer.execute("t-prime", t_prime_);
  }
  catch (const std::exception& r_e) {
  }
  bool fail = false;
  try {
    reader_or_writer.execute("U", u_input_);
    if(u_input_.size() == 3){ 
      dca::phys::params::ParamGrid u_grid{u_input_[0], u_input_[1], int(u_input_[2])};
      u_generator_ = std::optional<GridGenerator1D>(std::in_place, u_grid);
    }
    else if(u_input_.size() == 1)
      U_ = u_input_[0];
    else if(u_input_.size() != 0)
      fail = true;    
    //GridGenerator1D::GridGenerator1D grid(u_grid_);
            //set_U(u_grid_.start);    
  }
  catch (const std::exception& r_e) {
  }
  // This is a nice example of why this way of dealing with the input io is a mess
  // \todo fix this as there is no good way to report bad input
  if (fail)
    throw std::runtime_error("Only 1 or 3 values of U are valid inputs!");
  try {
    reader_or_writer.execute("V", V_);
  }
  catch (const std::exception& r_e) {
  }
  try {
    reader_or_writer.execute("V-prime", V_prime_);
  }
  catch (const std::exception& r_e) {
  }

  reader_or_writer.close_group();
}
