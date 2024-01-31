#ifndef UL_GENERATORS_H
#define UL_GENERATORS_H

#include <string>
#include <vector>

#ifdef QT_CORE_LIB
#include <QString>
#include <QVector>
#endif // QT_CORE_LIB

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

        // Std-used
        std::string generateText(int countMin, int countMax, const std::vector<std::string> & initWords);
        std::string generateString(int sizeMin, int sizeMax, bool hasSpaces);
        std::string generateAboba(int countMin, int countMax);
        std::string generatePath(bool withSpaces = false);
        std::vector<std::string> generateDepends(std::vector<std::string> & dependTargets, const std::string & target, int targetCount, const int randomInterval);

        // Qt-used
#ifdef QT_CORE_LIB
        QString generateTextQ(int countMin, int countMax, const std::vector<std::string> & initWords);
        QString generateStringQ(int sizeMin, int sizeMax, bool hasSpaces);
        QString generateAbobaQ(int countMin, int countMax);
        QString generatePathQ(bool withSpaces = false);
        QVector<QString> generateDependsQ(QVector<QString> & dependTargets, const QString & target, int targetCount, const int randomInterval);
#endif // QT_CORE_LIB
    }
}
#endif // UL_GENERATORS_H
