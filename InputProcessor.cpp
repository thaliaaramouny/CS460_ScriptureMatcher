#include "InputProcessor.h"
#include <sstream>
#include <algorithm>
#include <cctype>

// Tokenizes the input string into lowercase words while preserving apostrophes and removing other punctuation
std::vector<std::string> InputProcessor::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input); // Create a stream from the input string
    std::string word;

    while (iss >> word) { // Extract words separated by whitespace
        // Remove punctuation except apostrophes
        word.erase(std::remove_if(word.begin(), word.end(),
            [](char c) { return std::ispunct(static_cast<unsigned char>(c)) && c != '\''; }),
            word.end());

        // Convert word to lowercase
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        if (!word.empty()) tokens.push_back(word); // Add cleaned word to tokens
    }

    return tokens; // Return the list of tokens
}

// Collapse character repetitions beyond 2 (e.g., "sooo" → "soo", "saaaad" → "saad")
std::string collapseRepeats(const std::string& word) {
    std::string result;
    char prev = '\0';
    int repeatCount = 0;
    for (char c : word) {
        if (c == prev) {
            repeatCount++;
            if (repeatCount < 2) result += c; // Allow up to 2 repeated chars
        }
        else {
            repeatCount = 0;
            result += c;
        }
        prev = c;
    }
    return result;
}

// Assigns emotional intensity scores to each word in the input, considering intensifiers, negations, case, repetition, and punctuation
std::unordered_map<std::string, double> InputProcessor::scoreIntensities(const std::string& input) {
    std::unordered_map<std::string, double> scores;

    const std::unordered_set<std::string> strongIntensifiers = { "very", "so", "super", "extremely", "really" };
    const std::unordered_set<std::string> mildIntensifiers = { "slightly", "a little", "a bit", "somewhat", "kind of" };
    const std::unordered_set<std::string> negations = { "not", "no", "never" };
    const int NEGATION_SPAN = 2;

    double boost = 1.0;
    int negationWindow = 0;
    int totalExclaimCount = std::count(input.begin(), input.end(), '!');

    std::istringstream iss(input);
    std::string rawWord;

    while (iss >> rawWord) {
        // Clean word (letters + apostrophes)
        std::string cleaned;
        std::copy_if(rawWord.begin(), rawWord.end(), std::back_inserter(cleaned),
            [](char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '\''; });

        std::string lowerWord = cleaned;
        std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        lowerWord = collapseRepeats(lowerWord); // Normalize "sooo" to "soo"


        // Check negations first (so negation applies before intensifiers)
        if (negations.count(lowerWord)) {
            negationWindow = NEGATION_SPAN;
            boost = 1.0;  // Reset boost to avoid intensifiers confusing negation
            continue;
        }

        // Check intensifiers only if not negation word
        if (strongIntensifiers.count(lowerWord)) {
            boost = 1.5; // Strong boost
            continue;
        }
        if (mildIntensifiers.count(lowerWord)) {
            boost = 1.2; // Mild boost (should be > 1 to increase, not 0.5)
            continue;
        }

        double intensity = 1.0 * boost;

        // Capital letter boost
        bool hasUpper = std::any_of(rawWord.begin(), rawWord.end(),
            [](char c) { return std::isupper(static_cast<unsigned char>(c)); });
        if (hasUpper) intensity += 0.5;

        // Detect repeated characters (3 or more in a row) and boost intensity accordingly
        int maxRepeatBoost = 0;
        int repeatCount = 1;

        for (size_t i = 1; i < rawWord.size(); ++i) {
            if (rawWord[i] == rawWord[i - 1]) {
                repeatCount++;
            }
            else {
                if (repeatCount >= 3) {
                    maxRepeatBoost = std::max(maxRepeatBoost, repeatCount - 2); // 1 for 3 repeats, 2 for 4, etc.
                }
                repeatCount = 1;
            }
        }
        // Check at end of word
        if (repeatCount >= 3) {
            maxRepeatBoost = std::max(maxRepeatBoost, repeatCount - 2);
        }

        // Add intensity boost based on max repeat block size (capped at 0.5)
        if (maxRepeatBoost > 0) {
            double boostAmount = 0.1 * maxRepeatBoost;
            if (boostAmount > 0.5) boostAmount = 0.5;
            intensity += boostAmount;
        }


        // Exclamation marks boost
        if (totalExclaimCount > 0) {
            intensity += 0.3 * totalExclaimCount;
        }

        // Apply negation window flip (negation overrides boost)
        if (negationWindow > 0) {
            intensity *= -1.0;
            negationWindow--;
        }

        scores[lowerWord] += intensity;

        boost = 1.0;  // Reset boost after each scored word
    }

    return scores;
}

// Computes tone similarity between input tokens and emotion keyword lists
std::unordered_map<std::string, double> InputProcessor::computeToneSimilarity(
    const std::vector<std::string>& tokens,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& emotionKeywords
) {
    std::unordered_map<std::string, double> similarity;
    size_t tokenCount = tokens.size();

    for (const auto& [emotion, keywords] : emotionKeywords) {
        int matchCount = 0;

        // Count how many input tokens match keywords associated with the emotion
        for (const auto& word : tokens) {
            if (keywords.count(word)) matchCount++;
        }

        if (matchCount == 0) {
            similarity[emotion] = 0.0; // No overlap with this emotion
            continue;
        }

        // Ratio of matches to total tokens (smoothed with +1 to avoid div by 0)
        double ratio = static_cast<double>(matchCount) / (tokenCount + 1);

        // Multiplier favors concentrated emotion keywords (but caps it)
        double multiplier = std::min(static_cast<double>(tokenCount) / matchCount, 3.0);

        // Final similarity score = density * intensity
        similarity[emotion] = ratio * multiplier;
    }

    return similarity; // Map from emotion → tone similarity score
}
