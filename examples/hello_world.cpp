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

char target[13] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};

/*******************************************/
/* Implementation of classes               */
/*******************************************/
class HelloAnalyzer : public Analyzer<OutputData, FeatureFlags> {
    float score(OutputData attempt, Parameters<FeatureFlags> params) {
        float score = 0.0;
        for (int i = 0; i < 13; i++) {
            int difference = abs(attempt.final[i] - target[i]);
            if (difference > 128) {
                difference = 128;
            }

            score += (128.0 - (float)difference);
        }

        return score;
    }
};

int randomCharacter() { return (rand() % 128) - (rand() % 128); }
void printNode(Node<Solution> node) {
    std::cout << "node " << node.score << " [";
    for (int i = 0; i < 13; i++) {
        std::cout << node.solution.deltas[i] << " ";
    }
    std::cout << "]" << std::endl;
}

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
            node->solution.deltas[i] = randomCharacter();
        }

        return *node;
    }

    Node<Solution> combineNodes(Node<Solution> left, Node<Solution> right, Parameters<FeatureFlags> params) {
        Node<Solution>* node = new Node<Solution>();

        // Inspect the nodes
        bool chop = rand() % 13;

        for (int i = 0; i < chop; i++) {
            node->solution.deltas[i] = left.solution.deltas[i];
        }

        for (int i = chop; i < 13; i++) {
            node->solution.deltas[i] = right.solution.deltas[i];
        }

        for (int i = 0; i < 13; i++) {
            // Mutation logic
            if (rand() % 800 == 0) node->solution.deltas[i] = randomCharacter();
            if (rand() % 800 == 0) node->solution.deltas[i] = randomCharacter();

            // Boundary logic
            if (node->solution.deltas[i] < -128) {
                node->solution.deltas[i] = -128;
            } else if (node->solution.deltas[i] > 128) {
                node->solution.deltas[i] = 128;
            }
        }

        return *node;
    }
};

int main() {
    InputData input;
    Parameters<FeatureFlags> params = {.generations = 300,
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