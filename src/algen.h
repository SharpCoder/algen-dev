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
    FeatureFlags featureFLags;
};

template <class InputData, class OutputData, class Solution, class FeatureFlags>
class Algorithm {
   public:
    virtual OutputData generateOutput(Node<Solution> node, InputData input, Parameters<FeatureFlags> params);
    virtual Node<Solution> allocateNode(InputData input, Parameters<FeatureFlags> params);
    virtual Node<Solution> combineNodes(Node<Solution> left, Node<Solution> right, Parameters<FeatureFlags> params);
};

template <class OutputData, class FeatureFlags>
class Analyzer {
   public:
    virtual float score(OutputData attempt, Parameters<FeatureFlags> params);
};

}  // namespace algen

#endif