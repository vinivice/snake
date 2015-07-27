#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <fstream>

typedef std::vector<float> Neuron;
typedef std::vector<Neuron> Layer;
typedef std::vector<Layer> Neural_network;

class Individual{
        public:
            Neural_network genome;
            float fitness;
            static float max_fitness;
            
            Individual():fitness(0){};
            Individual(Neural_network g):genome(g), fitness(0){};
};

bool sort_idv_fitness(const Individual &lhs, const Individual &rhs);

typedef std::vector<Individual> Population;

void cross_over(const Individual *dad, const Individual *mom, Individual *child1, Individual *child2);

void mutation(Individual *idv);

void rand_individual(Individual *idv, const int input_size);

void print_genome(Individual *idv);

void generation_0(Population *pop, int input_size, int save_n);

void next_generation(Population *pop, int input_size, int save_n);

float calc_input_f(std::vector<float> f, Neuron n);

float sigmoid_function_f(std::vector<float> f, Neuron n);

float alt_sigmoid_function_f(std::vector<float> f, Neuron n);

void save_individual(std::string file_name, Individual *idv);

void load_individual(std::string file_name, Individual *idv);

void save_population(std::string pop_name, Population *pop, float max_fitness, int generation, int save_n);

void load_population(std::string pop_name, Population *pop, float *max_fitness, int *generation, int *save_n);

#endif
