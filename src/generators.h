#ifndef GENERATORS_H
#define GENERATORS_H

#include <string>
#include <vector>

#include <QString>
#include <QVector>

namespace Utility
{
    namespace Generators
    {

        namespace WordVectors
        {
            void addWords(const std::vector<std::string> & input, std::vector<std::string> & output);

            const std::vector<std::string> prepositions {
                "a", "the", "and", "as", "but",
                "how", "or", "this", "with", "will",
                "not", "for", "of"
            };

            const std::vector<std::string> pronouns {
                "i", "us", "me", "you", "they", "them", "anyone", "somebody"
            };

            const std::vector<std::string> nouns {
                "home", "pc", "work", "body", "hell", "boulder", "rook"
            };

            const std::vector<std::string> time {
                "now", "next", "simetimes", "tomorrow", "week"
            };

            const std::vector<std::string> answers = {
                "okay", "damn", "yes", "no", "well", "nope", "yeah", "whatever"
            };

            const std::vector<std::string> dolboyob = {
                "aboba", "pivo", "amogus", "sasibob",
                "biba", "boba", "bapa", "andrew", "pidr",
                "ass", "asshole", "fuck", "shit"
            };
        }

        QString generateText(int countMin, int countMax, const std::vector<std::string> & initWords);

        QString generateString(int sizeMin, int sizeMax, bool hasSpaces);

        QString generateAboba(int countMin, int countMax);

        QString generatePath(bool withSpaces = false);

        QVector<QString> generateDepends(QVector<QString> & dependTargets, const QString & target, int targetCount, const int randomInterval);
    }
}
#endif // GENERATORS_H
