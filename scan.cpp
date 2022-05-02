#define ARMA_USE_HDF5

#include <iostream>
#include <vector>
#include <cmath>

#include <armadillo>
#include "tqdm.h"

using std::cout, std::endl, std::ifstream, std::ofstream, std::string, arma::vec;


size_t LORENZ_WIDTH = 450; // points
size_t N_WIDTHS = 10;      // widths in full scan


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


vec get_power(vec& temperature, double beta) {
  double center = temperature.n_elem / 2;
  vec power(temperature.n_elem, arma::fill::zeros);
  for (size_t i = 0; i < temperature.n_elem; i++) {
    double offset = (-beta * temperature[i] + (static_cast<double>(i) - center) / static_cast<double>(LORENZ_WIDTH));
    power[i] = 1.0 / (1.0 + offset * offset);
  }
  return power;
}


vec transform(vec& x, double x0) {
  double exponential_average = 0.0;
  double damping_factor = std::exp(-1.0 / x0);
  vec result(x.n_elem, arma::fill::zeros);
  for (size_t i = 0; i < x.n_elem; i++) {
    exponential_average = x[i] + damping_factor * exponential_average;
    result[i] = exponential_average / x0;
  }
  return result;
}


void parse_args(int argc, char **argv, double& beta, double& x0, size_t& simulation_iterations) {
  if (argc >= 2) {
    std::stringstream beta_stream(argv[1]);
    beta_stream >> beta;
  }

  if (argc >= 3) {
    std::stringstream x0_stream(argv[2]);
    x0_stream >> x0;
  }

  if (argc >= 4) {
    std::stringstream iters_stream(argv[3]);
    iters_stream >> simulation_iterations;
  }
  cout << "beta (temperature contribution): " << beta << endl;
  cout << "x0 (scan speed): " << x0 << endl;
  cout << "simulation iterations: " << simulation_iterations << endl;
}


int main(int argc, char **argv) {
  size_t n_points = 2 * LORENZ_WIDTH * N_WIDTHS;
  double beta = 1.0;
  double x0 = 20.0;
  size_t simulation_iterations = 100000;
  parse_args(argc, argv, beta, x0, simulation_iterations);

  vec temperature(n_points, arma::fill::zeros);
  vec power = get_power(temperature, beta);

  tqdm bar;
  for (size_t j = 0; j < simulation_iterations; j++) {
    temperature = transform(power, x0);
    // temperature = smooth(temperature, n_smooth_points);
    power = get_power(temperature, beta);
    bar.progress(j + 1, simulation_iterations);
  }
  if (beta < 0.0) {
    temperature = arma::reverse(temperature);
    power = arma::reverse(power);
  }
  cout << endl;

  // saving simulation results
  std::stringstream hdf5_filename;
  hdf5_filename << "data/" << "beta_" << beta << "_x0_" << x0 << "_simulation_iterations_" << simulation_iterations << ".hdf5";
  temperature.save(arma::hdf5_name(hdf5_filename.str(), "temperature"), arma::hdf5_binary);
  power.save(arma::hdf5_name(hdf5_filename.str(), "power", arma::hdf5_opts::append), arma::hdf5_binary);
  cout << "temperature and power arrays saved to: " << endl << hdf5_filename.str() << endl;
  return 0;
}
