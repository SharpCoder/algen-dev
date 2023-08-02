#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>

#include "../src/algen.cpp"

using namespace algen;
using namespace std;

/*********************************************/
/** Structs needed for this genetic algorith */
/*********************************************/

// This represents the input for your problem. In our case,
// the input is a randomized string of 13 characters.
struct InputData {
    char seed[13];
};

// This represents one possible solution to your problem. In
// our case this is an array of character shifts.
struct Solution {
    int deltas[13];
};

// This represents the final output of a given solution. In
// our case, given an array of shifts and an input data, we
// will generate a final 13 character string as the output.
struct OutputData {
    char final[13];
};

// This is an optional set of feature flags that can be passed
// in to each function. Use it to configure your algorithm.
struct FeatureFlags {};

/*******************************************/
/* Helpers                                 */
/*******************************************/
char target[13] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
int randomCharacter() { return (rand() % 128) - (rand() % 128); }
float randomFloat() { return rand() / float(RAND_MAX); }

/*******************************************/
/* Implementation of classes               */
/*******************************************/

// The Analyzer class is used to score a given solution
class HelloAnalyzer : public Analyzer<OutputData, Solution, FeatureFlags> {
    // Score a given solution by calculating how far off each letter
    // is from the target and using the inverse difference as
    // the score.
    float score(shared_ptr<OutputData> attempt, const Parameters<FeatureFlags>& params) {
        float score = 0.0;
        for (int i = 0; i < 13; i++) {
            int difference = abs(attempt->final[i] - target[i]);
            if (difference > 128) {
                difference = 128;
            }

            score += (128.0 - (float)difference);
        }

        return score;
    }

    // This method is invoked after each generation. Check if
    // the provided output matches the winning condition.
    // If this method returns true, the runner will short-cirucit.
    bool checkSolution(float score, shared_ptr<Solution> solution, const shared_ptr<OutputData> attempt) {
        return score == 13.0 * 128.0;
    }
};

// The Algorithm class is used to perform the actual genetic algorithm
// logic. It's separate from the analyzer so you can create multiple
// different ways to solve the problem and mix/match later.
class HelloAlgorithm : public Algorithm<InputData, OutputData, Solution, FeatureFlags> {
   public:
    // This method will take a solution, the input, the parameters, and evaluate
    // what a final output would look like given those things.
    shared_ptr<OutputData> generateOutput(const shared_ptr<Solution> solution, const InputData& input,
                                          const Parameters<FeatureFlags>& params) {
        shared_ptr<OutputData> result(new OutputData());

        for (int i = 0; i < 13; i++) {
            result->final[i] = (char)(input.seed[i] + solution->deltas[i]);
        }

        return result;
    }

    // This method will create a brand new, empty state. It should be randomized
    // values which are used to seed the initial population.
    shared_ptr<Solution> generateRandomSolution(const InputData& input, const Parameters<FeatureFlags>& params) {
        auto solution = make_shared<Solution>();

        for (int i = 0; i < 13; i++) {
            solution->deltas[i] = randomCharacter();
        }

        return solution;
    }

    // This method will take two solutions and try to create a better one using parts of each.
    // There are many ways to implement this method, but in general, you should strive to
    // implement crossover and mutation (at a minimum). Whatever that looks likef or your
    // specific problem.
    shared_ptr<Solution> combineNodes(const shared_ptr<Solution> left, const shared_ptr<Solution> right,
                                      const Parameters<FeatureFlags>& params) {
        shared_ptr<Solution> result(new Solution());

        for (int i = 0; i < 13; i++) {
            if (randomFloat() < params.crossoverFactor) {
                result->deltas[i] = right->deltas[i];
            } else {
                result->deltas[i] = left->deltas[i];
            }
        }

        for (int i = 0; i < 13; i++) {
            // Mutation logic
            if (randomFloat() < params.mutationFactor) result->deltas[i] = randomCharacter();

            // Boundary logic
            if (result->deltas[i] < -128) {
                result->deltas[i] = -128;
            } else if (result->deltas[i] > 128) {
                result->deltas[i] = 128;
            }
        }

        return result;
    }
};

int64_t now() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

int main() {
    InputData input;
    int64_t begin_time = now();

    // Randomize the input to alpha characters
    for (int i = 0; i < 13; i++) {
        input.seed[i] = 'a' + rand() % 26;
    }

    // Setup the parameters for this experiment.
    Parameters<FeatureFlags> params = {.generations = 1000,
                                       .population = 5000,
                                       .elitismFactor = 0.05,
                                       .crossoverFactor = 0.25,
                                       .mutationFactor = 0.025,
                                       .tournamentSize = 7,
                                       .featureFlags = {}};

    // Choose our analyzer and algorithm
    HelloAlgorithm algorithm;
    HelloAnalyzer analyzer;

    // Execute the program.
    shared_ptr<Solution> winner = runAlgorithm(params, input, algorithm, analyzer);
    int64_t end_time = now();

    // Output the best solution we've found
    cout << "Winning output " << algorithm.generateOutput(winner, input, params)->final << endl;
    cout << "Took " << (end_time - begin_time) << "ms " << endl;
    return 0;
}