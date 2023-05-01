#define ARMA_USE_HDF5

#include <iostream>
#include <filesystem>
#include <vector>
#include <cmath>
#include <cassert>

#include <armadillo>
#include "tqdm.h"

using std::cout, std::endl, std::ifstream, std::ofstream, std::string, arma::vec;


size_t LORENZ_HALF_WIDTH = 455; // points
size_t N_WIDTHS = 30;      // widths in full frequency scan


vec calculate_power(vec& temperature, double beta) {
  double center = temperature.n_elem / 2;
  vec power(temperature.n_elem, arma::fill::zeros);
  for (size_t i = 0; i < temperature.n_elem; i++) {
    double offset = (-beta * temperature[i] + (static_cast<double>(i) - center) / static_cast<double>(LORENZ_HALF_WIDTH));
    power[i] = 1.0 / (1.0 + offset * offset);
  }
  return power;
}


vec calculate_temperature(vec& power, double x0) {
  double exponential_average = 0.0;
  double damping_factor = std::exp(-1.0 / (x0 * LORENZ_HALF_WIDTH));
  double weight = 1.0;
  vec result(power.n_elem, arma::fill::zeros);
  for (size_t i = 0; i < power.n_elem; i++) {
    exponential_average = power[i] + damping_factor * exponential_average;
    weight = 1.0 + damping_factor * weight;
    result[i] = exponential_average / weight;
  }
  return result;
}


void parse_args(int argc, char **argv, double& beta, double& x0, size_t& simulation_iterations, string& dest_dir) {

  std::stringstream beta_stream(argv[1]);
  beta_stream >> beta;
  assert(beta >= 0.0);

  std::stringstream x0_stream(argv[2]);
  x0_stream >> x0;
  assert(x0 > 0.0);

  std::stringstream iters_stream(argv[3]);
  iters_stream >> simulation_iterations;

  dest_dir = string(argv[4]);
  cout << "beta (temperature contribution to frequency offset): " << beta << endl;
  cout << "x0 (frequency scan speed): " << x0 << endl;
  cout << "simulation iterations: " << simulation_iterations << endl;
  cout << "dest_dir: " << dest_dir << endl;
}


void simulate_scan(vec& temperature, vec& power, vec& delta_abs, double beta, double x0, size_t simulation_iterations) {
  tqdm bar;
  double learning_rate = 0.01;
  double mse_error = 0.0;
  for (size_t j = 0; j < simulation_iterations; j++) {
    vec temperature_delta = calculate_temperature(power, x0) - temperature;
    temperature = learning_rate * temperature_delta + temperature;
    power = calculate_power(temperature, beta);
    mse_error += static_cast<arma::mat>(temperature_delta.t() * temperature_delta)(0,0) / temperature_delta.n_elem;
    if ((j + 1) % 10 == 0) {
      size_t error_idx = j / 10;
      delta_abs[error_idx] = std::sqrt(mse_error);
      mse_error = 0.0;
    }
    bar.progress(j + 1, simulation_iterations);
  }
  if (beta < 0.0) {
    temperature = arma::reverse(temperature);
    power = arma::reverse(power);
  }
  cout << endl;
}


int main(int argc, char **argv) {
  size_t n_points = 2 * LORENZ_HALF_WIDTH * N_WIDTHS;
  double beta = 1.0;
  double x0 = 0.05;
  size_t simulation_iterations = 100000;
  string dest_dir;
  parse_args(argc, argv, beta, x0, simulation_iterations, dest_dir);

  // saving simulation results
  std::stringstream filename;
  filename << "beta_" << argv[1] << "_x0_" << argv[2] << "_simulation_iterations_" << argv[3] << ".hdf5";
  std::stringstream hdf5_filename;
  hdf5_filename << dest_dir << "/" << filename.str();
  std::stringstream tmp_hdf5_filename;
  tmp_hdf5_filename << dest_dir << "/" << "." << filename.str();

  vec temperature(n_points, arma::fill::zeros);
  vec power = calculate_power(temperature, beta);
  vec delta_abs(simulation_iterations / 10, arma::fill::zeros);

  simulate_scan(temperature, power, delta_abs, beta, x0, simulation_iterations);

  temperature.save(arma::hdf5_name(tmp_hdf5_filename.str(), "temperature_up"), arma::hdf5_binary);
  power.save(arma::hdf5_name(tmp_hdf5_filename.str(), "power_up", arma::hdf5_opts::append), arma::hdf5_binary);
  delta_abs.save(arma::hdf5_name(tmp_hdf5_filename.str(), "delta_abs_up", arma::hdf5_opts::append), arma::hdf5_binary);


  temperature = vec(n_points, arma::fill::zeros);
  power = calculate_power(temperature, -beta);
  delta_abs = vec(simulation_iterations / 10, arma::fill::zeros);
  simulate_scan(temperature, power, delta_abs, -beta, x0, simulation_iterations);

  temperature.save(arma::hdf5_name(tmp_hdf5_filename.str(), "temperature_down", arma::hdf5_opts::append), arma::hdf5_binary);
  power.save(arma::hdf5_name(tmp_hdf5_filename.str(), "power_down", arma::hdf5_opts::append), arma::hdf5_binary);
  delta_abs.save(arma::hdf5_name(tmp_hdf5_filename.str(), "delta_abs_down", arma::hdf5_opts::append), arma::hdf5_binary);

  std::filesystem::rename(tmp_hdf5_filename.str(), hdf5_filename.str());
  cout << "temperature and power arrays saved to: " << endl << hdf5_filename.str() << endl;
  return 0;
}
