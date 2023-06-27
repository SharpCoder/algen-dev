
#include "algen.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

#ifndef MAX_THREADS
#define MAX_THREADS 4
#endif

namespace algen {

    namespace internal {

        template <class T>
        bool sort(Node<T>& a, Node<T>& b) {
            return a.score > b.score;
        }

        template <class Solution, class FeatureFlags>
        Node<Solution> tournamentSelection(Node<Solution>* nodes, unsigned int nodeLength,
                                           Parameters<FeatureFlags> params) {
            Node<Solution> bestNode;
            float bestScore = 0.0;

            for (int i = 0; i < params.tournamentSize; i++) {
                int idx = rand() % nodeLength;
                Node<Solution> node = nodes[idx];

                if (node.score > bestScore) {
                    bestScore = node.score;
                    bestNode = node;
                }
            }

            return bestNode;
        }

        template <class InputData, class OutputData, class Solution, class FeatureFlags>
        void evaluateSolution(int begin, int end, OutputData* outputs, Node<Solution>* population, InputData input,
                              Parameters<FeatureFlags> params,
                              Algorithm<InputData, OutputData, Solution, FeatureFlags>* algorithm,
                              Analyzer<OutputData, FeatureFlags>* analyzer) {
            for (int r = begin; r < end; r++) {
                outputs[r] = algorithm->generateOutput(population[r], input, params);
                population[r].score = analyzer->score(outputs[r], params);
            }
        }

        template <class InputData, class OutputData, class Solution, class FeatureFlags>
        void recombine(int begin, int end, Node<Solution>* population, Node<Solution>* nextPopulation, int poplen,
                       Parameters<FeatureFlags> params,
                       Algorithm<InputData, OutputData, Solution, FeatureFlags>* algorithm) {
            for (int r = begin; r < end; r++) {
                Node<Solution> left = internal::tournamentSelection(population, poplen, params);
                Node<Solution> right = internal::tournamentSelection(population, poplen, params);
                nextPopulation[r++] = algorithm->combineNodes(left, right, params);
            }
        }

    }  // namespace internal

    template <class InputData, class OutputData, class Solution, class FeatureFlags>
    Node<Solution> runAlgorithm(Parameters<FeatureFlags> params, InputData input,
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
        std::vector<std::thread> threads;

        for (int i = 0; i < poplen; i++) {
            population[i] = algorithm->allocateNode(input, params);
        }

        for (int generation = 0; generation < params.generations; generation++) {
            int nextPopIter = 0;
            std::cout << "Starting generation " << generation << std::endl;

            // Evaluate all the solutions
            const int RANGE_PER_THREAD = poplen / MAX_THREADS;
            for (int tid = 0; tid < MAX_THREADS; tid++) {
                int start = tid * RANGE_PER_THREAD;
                int end = start + RANGE_PER_THREAD;
                threads.emplace_back(
                    std::thread(internal::evaluateSolution<InputData, OutputData, Solution, FeatureFlags>, start, end,
                                &*outputs, &*population, input, params, &*algorithm, &*analyzer));
            }

            for (auto& th : threads) th.join();
            threads.clear();

            for (int r = 0; r < poplen; r++) {
                if (population[r].score > bestScore) {
                    bestScore = population[r].score;
                    bestSolution = population[r];
                    bestOutput = outputs[r];
                }
            }

            // Create the next population'
            std::sort(population, population + (sizeof(population) / sizeof(population[0])),
                      algen::internal::sort<Solution>);

            // Take the creame of the crop, in both directions. And we multiply by 0.5 because
            // each iteration takes 2 nodes
            for (; nextPopIter < poplen * params.elitismFactor; nextPopIter += 2) {
                int bottomIdx = poplen - nextPopIter - 1;
                int topIdx = nextPopIter;
                nextPopulation[nextPopIter] = population[bottomIdx];
                nextPopulation[nextPopIter + 1] = population[topIdx];
            }

            const int RECOMBINATIONS_PER_THREAD = (poplen - nextPopIter) / MAX_THREADS;
            for (int tid = 0; tid < MAX_THREADS; tid++) {
                int start = nextPopIter + tid * RECOMBINATIONS_PER_THREAD;
                int end = start + RECOMBINATIONS_PER_THREAD;

                threads.emplace_back(std::thread(internal::recombine<InputData, OutputData, Solution, FeatureFlags>,
                                                 start, end, &*population, &*nextPopulation, poplen, params,
                                                 &*algorithm));
            }

            for (auto& th : threads) th.join();
            threads.clear();

            // for (int r = 0; r < poplen - nextPopIter; r++) {
            //     Node<Solution> left = internal::tournamentSelection(population, poplen, params);
            //     Node<Solution> right = internal::tournamentSelection(population, poplen, params);
            //     nextPopulation[nextPopIter++] = algorithm->combineNodes(left, right, params);
            // }

            // Promote nextPopulation into real population
            for (int r = 0; r < poplen; r++) {
                population[r] = nextPopulation[r];
            }
        }

        std::cout << "Simulation terminated" << std::endl;
        return bestSolution;
    }

}  // namespace algen
