#include "textmatcher.hpp"

#include <algorithm>
#include <functional>
#include <numeric>

#include <QString>
#include <QStringList>

using aide::core::TextMatcher;

double TextMatcher::score(const QString& text, const QString& word)
{
    if (text.isEmpty() || !text.contains(word, Qt::CaseInsensitive)) {
        return 0.0;
    }

    return static_cast<double>(word.length()) /
           static_cast<double>(text.length());
}

double TextMatcher::score(const QStringList& texts, const QStringList& words)
{
    const auto bestScoreForWord = [&texts](const QString& word) {
        return std::transform_reduce(
            texts.begin(), texts.end(), 0.0,
            [](double lhs, double rhs) { return std::max(lhs, rhs); },
            [&word](const QString& text) {
                return TextMatcher::score(text, word);
            });
    };

    return std::transform_reduce(words.begin(), words.end(), 0.0, std::plus<>(),
                                 bestScoreForWord);
}
