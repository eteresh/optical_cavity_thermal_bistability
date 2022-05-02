#define ARMA_USE_HDF5

#include <iostream>
#include <vector>
#include <cmath>

#include <armadillo>
#include "tqdm.h"

using std::cout, std::endl, std::ifstream, std::ofstream, std::string, arma::vec;


size_t LORENZ_WIDTH = 500; // points
size_t N_WIDTHS = 20;      // widths in full scan


vec smooth(vec temperature, int n_smooth_points = 300) {
  vec smooth_temperature(temperature.n_elem, arma::fill::zeros);
  double smooth_value = 0.0;
  for (size_t i = 0; i < temperature.n_elem; i++) {
    smooth_value += temperature[i];
    if (i >= n_smooth_points) {
      smooth_value -= temperature[i - n_smooth_points];
    }
    smooth_temperature[i] = smooth_value / n_smooth_points;
  }
  return smooth_temperature;
}


vec get_power(vec& temperature, double alpha) {
  double center = temperature.n_elem / 2;
  vec power(temperature.n_elem, arma::fill::zeros);
  for (size_t i = 0; i < temperature.n_elem; i++) {
    double offset = (-alpha * temperature[i] + (static_cast<double>(i) - center) / static_cast<double>(LORENZ_WIDTH));
    power[i] = 1.0 / (1.0 + offset * offset);
  }
  return power;
}


vec transform(vec& x, double beta) {
  double exponential_average = 0.0;
  double damping_factor = std::exp(-beta);
  vec result(x.n_elem, arma::fill::zeros);
  for (size_t i = 0; i < x.n_elem; i++) {
    exponential_average = x[i] + damping_factor * exponential_average;
    result[i] = beta * exponential_average;
  }
  return result;
}


void parse_args(int argc, char **argv, double& alpha, double& beta, size_t& simulation_iterations) {
  if (argc >= 2) {
    std::stringstream alpha_stream(argv[1]);
    alpha_stream >> alpha;
  }

  if (argc >= 3) {
    std::stringstream beta_stream(argv[2]);
    beta_stream >> beta;
  }

  if (argc >= 4) {
    std::stringstream iters_stream(argv[3]);
    iters_stream >> simulation_iterations;
  }
  cout << "alpha (temperature contribution): " << alpha << endl;
  cout << "beta (inverse scan speed): " << beta << endl;
  cout << "simulation iterations: " << simulation_iterations << endl;
}


int main(int argc, char **argv) {
  size_t n_points = 2 * LORENZ_WIDTH * N_WIDTHS;
  double alpha = 1.0;
  double beta = 0.05;
  size_t simulation_iterations = 100000;
  parse_args(argc, argv, alpha, beta, simulation_iterations);

  vec temperature(n_points, arma::fill::zeros);
  vec power = get_power(temperature, alpha);

  tqdm bar;
  for (size_t j = 0; j < simulation_iterations; j++) {
    temperature = transform(power, beta);
    // temperature = smooth(temperature, n_smooth_points);
    power = get_power(temperature, alpha);
    bar.progress(j + 1, simulation_iterations);
  }
  cout << endl;

  // saving simulation results
  std::stringstream hdf5_filename;
  hdf5_filename << "data/" << "alpha_" << alpha << "_beta_" << beta << "_simulation_iterations_" << simulation_iterations << ".hdf5";
  temperature.save(arma::hdf5_name(hdf5_filename.str(), "temperature"), arma::hdf5_binary);
  power.save(arma::hdf5_name(hdf5_filename.str(), "power", arma::hdf5_opts::append), arma::hdf5_binary);
  cout << "temperature and power arrays saved to: " << endl << hdf5_filename.str() << endl;
  return 0;
}
