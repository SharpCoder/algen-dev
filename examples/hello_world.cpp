#include <iostream>

#include "../src/algen.cpp"

using namespace algen;

/*********************************************/
/** Structs needed for this genetic algorith */
/*********************************************/

struct InputData {
    char seed[13];
};

struct Solution {
    int deltas[13];
};

struct OutputData {
    char final[13];
};

struct FeatureFlags {};

/*******************************************/
/* Implementation of classes               */
/*******************************************/
class HelloAnalyzer : public Analyzer<OutputData, FeatureFlags> {
    float score(OutputData attempt, Parameters<FeatureFlags> params) { return 1.0; }
};

class HelloAlgorithm : public Algorithm<InputData, OutputData, Solution, FeatureFlags> {
   public:
    OutputData generateOutput(Node<Solution> node, InputData input, Parameters<FeatureFlags> params) {
        OutputData* result = new OutputData();
        return *result;
    }

    Node<Solution> allocateNode(InputData input, Parameters<FeatureFlags> params) {
        Node<Solution>* node = new Node<Solution>();
        return *node;
    }

    Node<Solution> combineNodes(Node<Solution> left, Node<Solution> right, Parameters<FeatureFlags> params) {
        Node<Solution>* node = new Node<Solution>();
        return *node;
    }
};

int main() {
    InputData input;
    Parameters<FeatureFlags> params = {.generations = 100,
                                       .population = 1000,
                                       .elitismFactor = 0.01,
                                       .crossoverFactor = 0.5,
                                       .mutationFactor = 0.001,
                                       .tournamentSize = 10,
                                       .featureFLags = {}};

    HelloAlgorithm algorithm;
    HelloAnalyzer analyzer;

    runAlgorithm<InputData, OutputData, Solution, FeatureFlags>(params, input, &algorithm, &analyzer);

    return 0;
}