#include <iostream>

#include "../src/algen.cpp"

class InputData {};
class OutputData {};
class Solution {};
class FeatureFlags {};

int main() {
    InputData input;
    algen::Parameters<FeatureFlags> params = {.generations = 100,
                                              .population = 1000,
                                              .elitismFactor = 0.01,
                                              .crossoverFactor = 0.5,
                                              .mutationFactor = 0.001,
                                              .tournamentSize = 10,
                                              .featureFLags = {}};

    algen::runAlgorithm<InputData, OutputData, Solution, FeatureFlags>(params, input);

    return 0;
}