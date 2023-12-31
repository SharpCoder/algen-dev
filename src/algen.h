#ifndef __ALGEN_RUNNER_H_
#define __ALGEN_RUNNER_H_

#include <memory>

using namespace std;

namespace algen {

    template <class Solution>
    struct Node {
        float score;
        shared_ptr<Solution> solution;
    };

    template <class Solution>
    struct Result {
        float score;
        Node<Solution> output;
    };

    // This is a set of common genetic algorithm parameters that
    // are often used for testing purposes.
    template <class FeatureFlags>
    struct Parameters {
        // The total number of generations to produce in the test
        unsigned int generations;
        // The total number of solutions to create per generation
        unsigned int population;
        // A number between 0 - 1 which indicates the percentage of
        // results that will be retained because they are the best
        // (or worst) solutions in the population.
        float elitismFactor;
        // A number between 0 - 1 which indicates the probability
        // of crossover, favoring one solution over the other.
        float crossoverFactor;
        // A number between 0 - 1 which indicates the probability
        // of mutation ocurring for a specific bit.
        float mutationFactor;
        // The total number of solutions that will be included
        // in the tournament selection event.
        unsigned int tournamentSize;
        // An optaionl set of feature flags needed for your
        // convenience.
        FeatureFlags featureFlags;
    };

    // An algorithm is a structure that represents the problem you are trying
    // to solve. It has methods for generating a random solution and evaluating
    // the solution in order to produce an output. Furthermore, it should know
    // how to recombine two solutions to produce the next generation.
    template <class InputData, class OutputData, class Solution, class FeatureFlags>
    class Algorithm {
       public:
        // A method which can take a test case and a Solution (effectively, the chromosome of the
        // genetic algorithm) and return an output.
        //
        // An example of this might be:
        // Given the input string "ELWWO"
        // Given the Solution [3,-7,-11,-11,0]
        // Apply the Solution (character shifts) to the original input
        //
        // Output -> "HELLO"
        //
        // This will later be scored with the analyzer.
        virtual shared_ptr<OutputData> generateOutput(const shared_ptr<Solution> solution, const InputData& input,
                                                      const Parameters<FeatureFlags>& params) = 0;

        // This method should generate a randomized Solution.
        virtual shared_ptr<Solution> generateRandomSolution(const InputData& input,
                                                            const Parameters<FeatureFlags>& params) = 0;

        // Given two Node<Solution>, generate an offsprint using whatever
        // genetic algorithm techniques you like. At a minimum, it should
        // include:
        //
        // - Crossover
        // - Mutation
        virtual shared_ptr<Solution> combineNodes(const shared_ptr<Solution> left, const shared_ptr<Solution> right,
                                                  const Parameters<FeatureFlags>& params) = 0;
    };

    template <class OutputData, class Solution, class FeatureFlags>
    class Analyzer {
       public:
        // Given a final output, this method will score it in a deterministic fashion
        // to determine how well it did.
        virtual float score(shared_ptr<OutputData> attempt, const Parameters<FeatureFlags>& params) = 0;

        // Given the best score, solution, and final output, determine if the desired solution
        // has been found.
        //
        // If this method returns true, the simulation will terminate.
        virtual bool checkSolution(float score, shared_ptr<Solution> solution,
                                   const shared_ptr<OutputData> attempt) = 0;
    };

}  // namespace algen

#endif