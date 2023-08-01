
#include "algen.h"

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <cstdlib>
#include <iostream>

/**
 * @brief The main namespace for the algen Genetic Algorithm Runner package.
 *
 */
namespace algen {

    namespace internal {

        template <class T>
        bool sort(Node<T>& a, Node<T>& b) {
            return a.score > b.score;
        }

        template <class Solution, class FeatureFlags>
        const Node<Solution>* tournamentSelection(const Node<Solution>* nodes, unsigned int nodeLength,
                                                  const Parameters<FeatureFlags>* params) {
            const Node<Solution>* bestNode;
            float bestScore = 0.0;

            for (int i = 0; i < params->tournamentSize; i++) {
                int idx = rand() % nodeLength;
                if (nodes[idx].score > bestScore) {
                    bestScore = nodes[idx].score;
                    bestNode = &nodes[idx];
                }
            }

            return bestNode;
        }

    }  // namespace internal

    /**
     * This method is the main entrypoint to the genetic algorithm runner
     *
     * Give elements of an experiment this method will setup your genetic algoirhtm with
     * an initial population, evaluate each solution, score each solution, and then
     * generate a new population using tournament selection and invoking the crossover
     * method you've defined.
     *
     * After executing for the defined amount of time, the best solution identified
     * will be returned.
     *
     * @tparam InputData The shape of information provided to each solution as an input.
     * @tparam OutputData The shape of information representing a final output. This is what
                          gets scored by the analyzer.
     * @tparam Solution The shape of information representing a solution. This is what is used
                        to generate the final output.
     * @tparam FeatureFlags An optional struct defining feature flags that are passed throughout
                            the system. Use this to experiment with different crossover, mutation,
                            and scoring methods.
     */
    template <class InputData, class OutputData, class Solution, class FeatureFlags>
    Node<Solution> runAlgorithm(const Parameters<FeatureFlags>* params, const InputData* input,
                                Algorithm<InputData, OutputData, Solution, FeatureFlags>* algorithm,
                                Analyzer<OutputData, Solution, FeatureFlags>* analyzer) {
        // Define generational parameters
        int poplen = params->population;
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

        for (int generation = 0; generation < params->generations; generation++) {
            int nextPopIter = 0;

            // Evaluate all the solutions
            for (int r = 0; r < poplen; r++) {
                outputs[r] = algorithm->generateOutput(&population[r], input, params);
                population[r].score = analyzer->score(outputs[r], params);

                if (population[r].score > bestScore) {
                    bestScore = population[r].score;
                    bestSolution = population[r];
                    bestOutput = outputs[r];
                }
            }

            // Check for the winning condition.
            if (analyzer->checkSolution(bestScore, bestSolution.solution, bestOutput)) {
                break;
            }

            // Create the next population'
            std::sort(population, population + (sizeof(population) / sizeof(population[0])),
                      algen::internal::sort<Solution>);

            // Take the creame of the crop, in both directions. And we multiply by 0.5 because
            // each iteration takes 2 nodes
            for (; nextPopIter < poplen * params->elitismFactor; nextPopIter += 2) {
                int bottomIdx = poplen - nextPopIter - 1;
                int topIdx = nextPopIter;
                nextPopulation[nextPopIter] = population[bottomIdx];
                nextPopulation[nextPopIter + 1] = population[topIdx];
            }

            for (; nextPopIter < poplen; nextPopIter++) {
                const Node<Solution>* left = internal::tournamentSelection(population, poplen, params);
                const Node<Solution>* right = internal::tournamentSelection(population, poplen, params);
                nextPopulation[nextPopIter] = algorithm->combineNodes(left, right, params);
            }

            // Promote nextPopulation into real population
            for (int r = 0; r < poplen; r++) {
                population[r] = nextPopulation[r];
            }
        }

        return bestSolution;
    }

}  // namespace algen
