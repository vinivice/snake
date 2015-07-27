#include "neural_network.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <fstream>

bool sort_idv_fitness(const Individual &lhs, const Individual &rhs)
{
    return lhs.fitness > rhs.fitness;
}

void cross_over(const Individual *dad, const Individual *mom, Individual *child1, Individual *child2)
{
    int ml, mn, mw, i;
    ml = rand()%dad->genome.size(); //choose middle layer
    mn = rand()%dad->genome[ml].size(); //choose middle neuron
    mw = rand()%dad->genome[ml][mn].size();  //choose middle weight

    for(i = 0; i < ml; i++)
    {
        child1->genome[i] = dad->genome[i];
        child2->genome[i] = mom->genome[i];
    }

    for(i = 0; i < mn; i++)
    {
        child1->genome[ml][i] = dad->genome[ml][i];
        child2->genome[ml][i] = mom->genome[ml][i];
    }

    for(i = 0; i < mw; i++)
    {
        child1->genome[ml][mn][i] = dad->genome[ml][mn][i];
        child2->genome[ml][mn][i] = mom->genome[ml][mn][i];
    }

    for(i = mw; i < dad->genome[ml][mn].size(); i++)
    {
        child2->genome[ml][mn][i] = dad->genome[ml][mn][i];
        child1->genome[ml][mn][i] = mom->genome[ml][mn][i];
    }

    for(i = (mn+1); i < dad->genome[ml].size(); i++)
    {
        child2->genome[ml][i] = dad->genome[ml][i];
        child1->genome[ml][i] = mom->genome[ml][i];
    }

    for(i = (ml+1); i < dad->genome.size(); i++)
    {
        child2->genome[i] = dad->genome[i];
        child1->genome[i] = mom->genome[i];
    }
}

void mutation(Individual *idv)
{
    int ml, mn, mw, i, j = 0;
    while(j < 5)
    {
        ml = rand()%idv->genome.size(); //choose middle layer
        mn = rand()%idv->genome[ml].size(); //choose middle neuron
        mw = rand()%idv->genome[ml][mn].size();  //choose middle weight
        i = (rand()%20001);
        idv->genome[ml][mn][mw] = (i - 10000)/10000.0;
        j = (rand()%100);
    }
}

void rand_individual(Individual *idv, const int input_size)
{
    //input == game grid
    //output == 4 neurons giving the direction

    idv->genome.clear();
    idv->genome.resize(1);

    idv->genome[0].resize(4);
    //idv->genome[1].resize(4);

    int i, j;
    
    for(i = 0; i < 4; i++)
    {
        idv->genome[0][i].clear();
        for(j = 0; j < (input_size+1); j++)
        {
            idv->genome[0][i].push_back((rand()%20001 - 10000)/10000.0);
        }
    }

    /*for(i = 0; i < 4; i++)
    {
        idv->genome[1][i].clear();
        for(j = 0; j < 16; j++)
        {
            idv->genome[1][i].push_back((rand()%20001 - 10000)/10000.0);
        }
    }*/

}

void print_genome(Individual *idv)
{
    int i, j, k;

    std::cout.precision(4); 
    std::cout << std::fixed;
    
    for(i = 0; i < idv->genome.size(); i++)
    {
        for(j = 0; j < idv->genome[i].size(); j++)
        {
            for(k = 0; k < idv->genome[i][j].size(); k++)
            {
                std::cout << idv->genome[i][j][k] << " - ";
            }
        }
    }
    
    std::cout << std::endl << std::endl;
}

void generation_0(Population *pop, int input_size, int save_n)
{
    int i;
    
    pop->resize(save_n*(2*save_n + 1));

    for(i = 0; i < pop->size(); i++)
    {
        rand_individual(&(*pop)[i], input_size);
    }
    
}

void next_generation(Population *pop, int input_size, int save_n)
{
    std::sort(pop->begin(), pop->end(), sort_idv_fitness);

    int i, j, aux = save_n;
    
    for(i = 0; i < save_n; i++)
    {
        for(j = i+1; j < save_n; j++)
        {
            cross_over(&(*pop)[i], &(*pop)[j], &(*pop)[aux], &(*pop)[aux + 1]);
            (*pop)[aux].fitness = (*pop)[aux + 1].fitness = 0;
            aux++;
            aux++;
        }
    }

    for(i = 0; i < save_n; i++)
    {
        (*pop)[aux] = (*pop)[i];
        mutation(&(*pop)[aux]);
        (*pop)[aux].fitness = 0;
        aux++;
    }
    
    for(i = 0; i < save_n; i++)
    {
        for(j = i+1; j < save_n; j++)
        {
            cross_over(&(*pop)[i], &(*pop)[j], &(*pop)[aux], &(*pop)[aux+1]);
            mutation(&(*pop)[aux]);
            mutation(&(*pop)[aux+1]);
            (*pop)[aux].fitness = (*pop)[aux + 1].fitness = 0;
            aux++;
            aux++;
        }
    }

    for(i = 0; i < save_n; i++)
    {
        rand_individual(&(*pop)[aux + i], input_size);
        (*pop)[aux + i].fitness = 0;
    }
}

float calc_input_f(std::vector<float> f, Neuron n)
{
    //Calculate sigmoid function input
    float input=0;
    int i;

    for(i = 0; i < (n.size() - 1); i++)
    {
        input += (f[i])*n[i];
    }

    input += n[n.size() - 1];

    return input;

}

float sigmoid_function_f(std::vector<float> f, Neuron n)
{
    return (1.0/(1+exp(-calc_input_f(f, n))));
}

float alt_sigmoid_function_f(std::vector<float> f, Neuron n)
{
    return (2*(1.0/(1+exp(-calc_input_f(f, n))) - 0.5));
}


void save_individual(std::string file_name, Individual *idv)
{
    std::ofstream save_file;
    save_file.open(file_name.c_str(), std::ofstream::out | std::ofstream::binary);
    int i, j, k, auxi;
    float auxf;
    
    for(i = 0; i < idv->genome.size(); i++)
    {
        save_file.write("L", 1);
        for(j = 0; j < idv->genome[i].size(); j++)
        {
            save_file.write("N", 1);
            auxi = idv->genome[i][j].size();
            save_file.write((char*)(&auxi), 4);
            for(k = 0; k < idv->genome[i][j].size(); k++)
            {
                save_file.write((char*)(&(idv->genome[i][j][k])), 4);
            }
        }
    }
    save_file.write("F", 1);
    save_file.write((char*)(&(idv->fitness)), 4);
    save_file.write("E", 1);
    save_file.close();
}

void load_individual(std::string file_name, Individual *idv)
{
    idv->genome.clear();

    std::ifstream load_file;
    load_file.open(file_name.c_str(), std::ifstream::in | std::ifstream::binary);

    char *auxc = new char[1];
    int auxi, i;
    float auxf;
    auxc[0] = 'A';
    
    if(load_file.is_open())
    {	
    	while(auxc[0] != 'E')
    	{
        	load_file.read(auxc, 1);
        	if(auxc[0] == 'L')
        	{
        	    idv->genome.push_back(Layer());
        	}
        	else if(auxc[0] == 'N')
        	{
        	    (idv->genome.back()).push_back(Neuron());
        	    load_file.read((char*)(&auxi), 4);
        	    for(i = 0; i < auxi; i++)
        	    {
        	        load_file.read((char*)(&auxf), 4);
        	        ((idv->genome.back()).back()).push_back(auxf);
        	    }
        	}
            else if(auxc[0] == 'F')
            {
                load_file.read((char*)(&auxf), 4);
                idv->fitness = auxf;
            }
    	}
    }
    load_file.close();
}

void save_population(std::string pop_name, Population *pop, float max_fitness, int generation, int save_n)
{
    int i;
    std::string auxs;

    std::ofstream save_file;
    auxs = pop_name;
    auxs += "_info.vvc";
    save_file.open(auxs.c_str(), std::ofstream::out | std::ofstream::binary);
    save_file.write((char*)&generation, 4);
    save_file.write((char*)&max_fitness, 4);
    save_file.write((char*)&save_n, 4);
    save_file.close();


    for(i = 0; i < pop->size(); i++)
    {
        auxs = pop_name;
        auxs += '[';
        auxs += std::to_string(i);
        auxs += "].vvc";
        save_individual(auxs,&(*pop)[i]);        
    }
}

    void load_population(std::string pop_name, Population *pop, float *max_fitness, int *generation, int *save_n)
{
    int i;
    std::string auxs;

    std::ifstream load_file;
    auxs = pop_name;
    auxs += "_info.vvc";
    load_file.open(auxs.c_str(), std::ifstream::in | std::ifstream::binary);
    load_file.read((char*)generation, 4);
    load_file.read((char*)max_fitness, 4);
    load_file.read((char*)save_n, 4);
    load_file.close();

    pop->resize((*save_n)*(2*(*save_n)+1));

    for(i = 0; i < pop->size(); i++)
    {
        auxs = pop_name;
        auxs += '[';
        auxs += std::to_string(i);
        auxs += "].vvc";
        load_individual(auxs,&(*pop)[i]);        
    }
}
