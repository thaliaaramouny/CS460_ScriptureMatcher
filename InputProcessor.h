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
    /**
     * Tokenizes the user's input by:
     *  - Removing punctuation (except apostrophes)
     *  - Lowercasing all words
     *  - Splitting the sentence into individual words
     * This allows for consistent keyword matching regardless of input format.
     */
    static std::vector<std::string> tokenize(const std::string& input);

    /**
     * Scores emotional intensity for each word using:
     *  - Detected intensifiers (e.g., "very", "slightly")
     *  - Repeated letters (e.g., "saaad")
     *  - Capital letters
     *  - Exclamation marks
     *  - Negations ("not", "never") to invert meaning
     * Returns a map from word → intensity score.
     * These scores directly affect graph traversal costs.
     */
    static std::unordered_map<std::string, double> scoreIntensities(const std::string& input);

    /**
     * Computes how much the input tone overlaps with each emotion category.
     * Compares token words to emotion-specific keyword sets and calculates a soft match score.
     * This adds a secondary boost to emotion detection beyond just raw keywords.
     *
     * Returns a map: emotion name → tone similarity score.
     */
    static std::unordered_map<std::string, double> computeToneSimilarity(
        const std::vector<std::string>& tokens,
        const std::unordered_map<std::string, std::unordered_set<std::string>>& emotionKeywords
    );
};
