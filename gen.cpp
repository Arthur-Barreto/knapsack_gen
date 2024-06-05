#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

const int num_items = 40;
const int knapsack_capacity = 100;
const int solutions_per_pop = 16;
const int num_generations = 1000;

vector<int> item_number(num_items);
vector<int> weight(num_items);
vector<int> value(num_items);
vector<vector<int>> initial_population(solutions_per_pop, vector<int>(num_items));

// Function to generate random numbers
int random_int(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// Function to print items
void print_items() {
    cout << "Possíveis itens:\nItem\tPeso\tValor\n";
    for (int i = 0; i < num_items; i++) {
        cout << item_number[i] << "\t" << weight[i] << "\t" << value[i] << "\n";
    }
    cout << "Capacidade da mochila: " << knapsack_capacity << endl;
}

// Function to initialize items and population
void initialize() {
    iota(item_number.begin(), item_number.end(), 1);
    generate(weight.begin(), weight.end(), []() { return random_int(1, 15); });
    generate(value.begin(), value.end(), []() { return random_int(1, 100); });
    for (auto &individual : initial_population) {
        generate(individual.begin(), individual.end(), []() { return random_int(0, 1); });
    }
    print_items();
    cout << "Tamanho da população = (" << solutions_per_pop << ", " << num_items << ")\n";
}

// Fitness function
vector<int> fitness(const vector<vector<int>> &population) {
    vector<int> fitness(population.size(), 0);
    for (size_t i = 0; i < population.size(); i++) {
        int S1 = inner_product(population[i].begin(), population[i].end(), value.begin(), 0);
        int S2 = inner_product(population[i].begin(), population[i].end(), weight.begin(), 0);
        if (S2 <= knapsack_capacity) {
            fitness[i] = S1;
        } else {
            fitness[i] = 0;
        }
    }
    return fitness;
}

// Selection function
vector<vector<int>> selection(const vector<int> &fitness, int num_parents, const vector<vector<int>> &population) {
    vector<vector<int>> parents(num_parents, vector<int>(population[0].size()));
    vector<int> fitness_copy = fitness;
    for (int i = 0; i < num_parents; i++) {
        auto max_fitness_iter = max_element(fitness_copy.begin(), fitness_copy.end());
        int max_fitness_idx = distance(fitness_copy.begin(), max_fitness_iter);
        parents[i] = population[max_fitness_idx];
        fitness_copy[max_fitness_idx] = -9999;
    }
    return parents;
}

// Crossover function
vector<vector<int>> crossover(const vector<vector<int>> &parents, int num_offsprings) {
    vector<vector<int>> offsprings(num_offsprings, vector<int>(parents[0].size()));
    int crossover_point = parents[0].size() / 2;
    double crossover_rate = 0.8;
    int i = 0, cnt_offsprings = 0;
    while (cnt_offsprings < num_offsprings) {
        int parent1_index = i % parents.size();
        int parent2_index = (i + 1) % parents.size();
        double x = static_cast<double>(rand()) / RAND_MAX;
        if (x > crossover_rate) {
            i++;
            continue;
        }
        copy(parents[parent1_index].begin(), parents[parent1_index].begin() + crossover_point, offsprings[cnt_offsprings].begin());
        copy(parents[parent2_index].begin() + crossover_point, parents[parent2_index].end(), offsprings[cnt_offsprings].begin() + crossover_point);
        cnt_offsprings++;
        i++;
    }
    return offsprings;
}

// Mutation function
vector<vector<int>> mutation(vector<vector<int>> offsprings) {
    double mutation_rate = 0.15;
    for (auto &offspring : offsprings) {
        double random_value = static_cast<double>(rand()) / RAND_MAX;
        if (random_value > mutation_rate) {
            continue;
        }
        int int_random_value = random_int(0, offsprings[0].size() - 1);
        offspring[int_random_value] = (offspring[int_random_value] == 0) ? 1 : 0;
    }
    return offsprings;
}

// Optimization function
pair<vector<int>, vector<vector<int>>> optimize(vector<vector<int>> population) {
    vector<int> parameters;
    vector<vector<int>> fitness_history;
    int num_parents = solutions_per_pop / 2;
    int num_offsprings = solutions_per_pop - num_parents;

    for (int i = 0; i < num_generations; i++) {
        vector<int> fitness_ = fitness(population);
        fitness_history.push_back(fitness_);
        vector<vector<int>> parents = selection(fitness_, num_parents, population);
        vector<vector<int>> offsprings = crossover(parents, num_offsprings);
        vector<vector<int>> mutants = mutation(offsprings);
        copy(parents.begin(), parents.end(), population.begin());
        copy(mutants.begin(), mutants.end(), population.begin() + parents.size());
    }

    cout << "Última geração: \n";
    for (const auto &individual : population) {
        for (int gene : individual) {
            cout << gene << " ";
        }
        cout << "\n";
    }

    vector<int> fitness_last_gen = fitness(population);
    cout << "Fitness da última geração: \n";
    for (int fit : fitness_last_gen) {
        cout << fit << " ";
    }
    cout << "\n";

    auto max_fitness_iter = max_element(fitness_last_gen.begin(), fitness_last_gen.end());
    int max_fitness_idx = distance(fitness_last_gen.begin(), max_fitness_iter);
    parameters = population[max_fitness_idx];

    return {parameters, fitness_history};
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    initialize();

    auto [parameters, fitness_history] = optimize(initial_population);

    cout << "Indivíduo da última geração com maior fitness: \n";
    for (int gene : parameters) {
        cout << gene << " ";
    }
    cout << "\n";

    cout << "\nQuais itens irão na mochila:\n";
    int total_weight = 0;
    for (int i = 0; i < num_items; i++) {
        if (parameters[i] != 0) {
            cout << item_number[i] << "\t";
            total_weight += weight[i];
        }
    }
    cout << "\nPeso total da mochila = " << total_weight << endl;

    return 0;
}
