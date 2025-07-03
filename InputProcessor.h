#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/**
 * InputProcessor is responsible for interpreting raw user input.
 * It breaks down the sentence into words (tokens), analyzes emotional intensity,
 * and computes how well the tone matches various emotion categories.
 */
class InputProcessor {
public:
     static std::vector<std::string> tokenize(const std::string& input);
     static std::unordered_map<std::string, double> scoreIntensities(const std::string& input);
     static std::unordered_map<std::string, double> computeToneSimilarity(
        const std::vector<std::string>& tokens,
        const std::unordered_map<std::string, std::unordered_set<std::string>>& emotionKeywords
    );
};
