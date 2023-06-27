#include <cstdlib>
#include <iostream>
#include <limits>

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
    float score(OutputData attempt, Parameters<FeatureFlags> params) {
        float score = 0.0;
        char target[13] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

        for (int i = 0; i < 13; i++) {
            if (attempt.final[i] == target[i]) {
                score += 1.0;
            }
        }

        return score;
    }
};

class HelloAlgorithm : public Algorithm<InputData, OutputData, Solution, FeatureFlags> {
   public:
    OutputData generateOutput(Node<Solution> node, InputData input, Parameters<FeatureFlags> params) {
        OutputData* result = new OutputData();
        for (int i = 0; i < 13; i++) {
            result->final[i] = (char)(input.seed[i] + node.solution.deltas[i]);
        }
        return *result;
    }

    Node<Solution> allocateNode(InputData input, Parameters<FeatureFlags> params) {
        Node<Solution>* node = new Node<Solution>();
        node->score = std::numeric_limits<float>::min();
        for (int i = 0; i < 13; i++) {
            node->solution.deltas[i] = (short)((rand() % 256) - 128);
        }

        return *node;
    }

    Node<Solution> combineNodes(Node<Solution> left, Node<Solution> right, Parameters<FeatureFlags> params) {
        Node<Solution>* node = new Node<Solution>();
        for (int i = 0; i < 13; i++) {
            if (rand() % 2 == 0) {
                node->solution.deltas[i] = left.solution.deltas[i];
            } else {
                node->solution.deltas[i] = right.solution.deltas[i];
            }

            if (rand() % 500 == 0) {
                node->solution.deltas[i] = (char)((rand() % 256) - 128);
            }
        }
        return *node;
    }
};

int main() {
    InputData input;
    Parameters<FeatureFlags> params = {.generations = 100,
                                       .population = 10000,
                                       .elitismFactor = 0.01,
                                       .crossoverFactor = 0.5,
                                       .mutationFactor = 0.001,
                                       .tournamentSize = 10,
                                       .featureFLags = {}};

    HelloAlgorithm algorithm;
    HelloAnalyzer analyzer;

    Node<Solution> winner =
        runAlgorithm<InputData, OutputData, Solution, FeatureFlags>(params, input, &algorithm, &analyzer);

    std::cout << "Winning output " << algorithm.generateOutput(winner, input, params).final << std::endl;
    return 0;
}