#include <iostream>
#include <limits>
#include <string>

#include <vector>
#include <sstream>

using namespace std;

namespace seatrs {
    struct seat { 
        string name, description;
        bool isReserved = false;
    };
}

namespace program {
    namespace config {
        int lengthHUD = 80;
    }

    namespace control {
        const int maxPossibleRows = 100;
        const int maxPossibleColumns = 100;
        const int minimumLengthHUD = 60;
    }
}

namespace utils {

    namespace format {

        /**
         * Repeat a given string a given number of times.
         *
         * @param num The number of times to repeat the string.
         * @param text The string to repeat.
         *
         * @returns A string consisting of the given string repeated the
         *          given number of times.
         */
        string repeatText(int num, string text) {
            string result;
            for (int i = 0; i < num; i++) {
                result += text;
            }
            return result;
        }
        
        /**
         * Repeat a given char a given number of times.
         *
         * @param num The number of times to repeat the character.
         * @param text The character to repeat.
         *
         * @returns A string consisting of the given character repeated the
         *          given number of times.
         */
        string repeatText(int num, char text) {
            return string(num, text);
        }

        vector<string> splitWords(const string input, size_t limit) {
            string word, result;
            size_t currentLength = 0;
            vector<string> results;

            size_t start = 0;
            
        }

        enum FormatPosition { LEFT, CENTER, RIGHT, CUSTOM };

        struct FormatParams {
            FormatPosition position = LEFT;
            string space = " ";
            int padding = 2;
            int firstLineIndent = 0;
            int limitLength = -1; // negative value for auto length
            int textLength = 0; 
        };

        string formatText(string text, FormatParams params) {
            int spaceLeft, spaceRight;

            if (params.textLength <= 0) {
                params.textLength = text.length();
            }

            if (params.limitLength < 0) {
                params.limitLength = program::config::lengthHUD;
            }


        }
    }

    
}