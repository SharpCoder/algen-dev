#ifndef __ALGEN_RUNNER_H_
#define __ALGEN_RUNNER_H_

namespace algen {

    template <class Solution>
    struct Node {
        float score;
        Solution solution;
    };

    template <class Solution>
    struct Result {
        float score;
        Node<Solution> output;
    };

    template <class FeatureFlags>
    struct Parameters {
        unsigned int generations;
        unsigned int population;
        float elitismFactor;
        float crossoverFactor;
        float mutationFactor;
        unsigned int tournamentSize;
        FeatureFlags featureFlags;
    };

    template <class InputData, class OutputData, class Solution, class FeatureFlags>
    class Algorithm {
       public:
        virtual OutputData generateOutput(const Node<Solution>* node, const InputData* input,
                                          const Parameters<FeatureFlags>* params);
        virtual Node<Solution> allocateNode(const InputData* input, const Parameters<FeatureFlags>* params);
        virtual Node<Solution> combineNodes(const Node<Solution>* left, const Node<Solution>* right,
                                            const Parameters<FeatureFlags>* params);
    };

    template <class OutputData, class FeatureFlags>
    class Analyzer {
       public:
        virtual float score(OutputData attempt, const Parameters<FeatureFlags>* params);
    };

}  // namespace algen

#endif