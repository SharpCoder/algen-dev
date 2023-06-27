
#include "algen.h"

#include <algorithm>
#include <iostream>

namespace algen {

template <class T>
bool sort(Node<T>& a, Node<T>& b) {
    return a.score > b.score;
}

template <class InputData, class OutputData, class Solution, class FeatureFlags>
void runAlgorithm(Parameters<FeatureFlags> params, InputData input,
                  Algorithm<InputData, OutputData, Solution, FeatureFlags>* algorithm,
                  Analyzer<OutputData, FeatureFlags>* analyzer) {
    // Define generational parameters
    int poplen = params.population;
    Node<Solution> population[poplen];
    Node<Solution> nextPopulation[poplen];
    OutputData outputs[poplen];

    // Best solutions
    float bestScore = 0.0;
    Node<Solution> bestSolution;
    OutputData bestOutput;

    for (int i = 0; i < poplen; i++) {
        population[i] = algorithm->allocateNode(input, params);
    }

    for (int generation = 0; generation < params.generations; generation++) {
        std::cout << "Starting generation " << generation << std::endl;

        // Evaluate all the solutions
        for (int r = 0; r < poplen; r++) {
            outputs[r] = algorithm->generateOutput(population[r], input, params);
            population[r].score = analyzer->score(outputs[r], params);

            if (population[r].score > bestScore) {
                bestScore = population[r].score;
                bestSolution = population[r];
                bestOutput = outputs[r];
            }
        }

        // Create the next population'
        std::sort(population, population + (sizeof(population) / sizeof(population[0])), algen::sort<Solution>);

        // Take the creame of the crop, in both directions. And we multiply by 0.5 because
        // each iteration takes 2 nodes
        for (int r = 0; r < poplen * params.elitismFactor; r += 2) {
            int bottomIdx = poplen - r - 1;
            int topIdx = r;
            nextPopulation[r] = population[bottomIdx];
            nextPopulation[r + 1] = population[topIdx];
        }
    }

    std::cout << "Hello, super fun world!\n";
    std::cout << population[30].score;
}

}  // namespace algen
