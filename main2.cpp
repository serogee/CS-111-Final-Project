#include <iostream>
#include <limits>
#include <string>

#include <vector>
#include <sstream>

using namespace std;

namespace seatrs {
    struct Seat { 
        string name, description;
        bool isReserved = false;
    };

    namespace data {
        int totalOccupiedSeats = 0;
        vector<vector<Seat>> seats;

        int totalRows = 10;
        int totalColumns = 10;
    }
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

        /**
         * Split a given string into words, ensuring that no word exceeds the
         * given limit.
         *
         * @param input The string to split into words.
         * @param limit The maximum number of characters allowed in each word.
         *
         * @returns A vector of strings, each containing a line from the
         *          original input string. No line exceeds the given limit.
         */
        vector<string> splitWords(const string& input, size_t limit) {
            stringstream stream(input); // Stream to process the input line by line
            string line, word, result;
            vector<string> results;
            char space = ' ';

            while (getline(stream, line)) {
                result.clear();

                int currentLength = 0;
                int i = 0;

                // Preserve leading spaces (indent) for each line
                while (i < line.length() && line[i] == space) i++;
                
                result += string(i, space);
                currentLength = result.length();

                while (i < line.length()) {
                    int spaceStart = i;
                    while (i < line.length() && line[i] == space) i++;
                    int wordStart = i;
                    while (i < line.length() && line[i] != space) i++;
                    int wholeLength = i - spaceStart;
                    int wordLength = i - wordStart;

                    if ((currentLength + (currentLength > 0 ? wholeLength : wordLength) > limit) && !(wordLength > limit)) {
                        results.push_back(result);
                        result.clear();
                        currentLength = 0;
                    }

                    int length = (currentLength > 0 ? wholeLength : wordLength);

                    result += line.substr((currentLength > 0 ? spaceStart : wordStart), length);
                    currentLength += length;
                }

                // Add the last part of the processed line
                results.push_back(result);
                
            }

            return results;
        }

        enum FormatAlign { 
            LEFT, 
            CENTER, 
            RIGHT 
        };

        struct FormatParams {
            FormatAlign align = LEFT;
            int limitLength = -1;       // negative means use program::config::lengthHUD
            int padding = 0;
            string space = " ";
            int explicitTextLength = 0; // to handle text where length != no. characters
                                        //   like \t which is 1 character but 4 spaces,
                                        //   specifically for the nameArt to work
        };

        /**
         * Format a given string into a block of text with a specified length, 
         * with optional padding and centering. The text is split into words and 
         * each word is placed on a new line if it exceeds the specified length. 
         * The words are then centered and padded with a given space character.
         * 
         * @param text The text to format.
         * @param params The parameters for formatting the text.
         * 
         * @returns A string consisting of the formatted text.
         */
        string formatText(const string& text, FormatParams params) {
            int spaceLeft, spaceRight;
            int textLength;
            string spaceLeftText, spaceRightText;

            string result;

            vector<string> lines = (params.explicitTextLength > 0) ?
                vector<string> {text} :
                splitWords(text, params.limitLength - (params.padding * 2));

            for (int i = 0; i < lines.size(); i++) {
                string line = lines[i];

                textLength = (params.explicitTextLength > 0) ? 
                    (params.explicitTextLength) : 
                    (text.length());

                if (params.limitLength < 0) {
                    params.limitLength = program::config::lengthHUD;
                }
                
                int spaceTotalLength = params.limitLength - textLength - (params.padding * 2);

                // Determine in what alignment each line should be formatted in
                switch (params.align) {
                    case LEFT:
                        spaceLeft = 0;
                        spaceRight = spaceTotalLength;
                        break;
                    case CENTER:
                        spaceLeft = (params.limitLength - textLength) / 2;
                        if ((params.limitLength - textLength) % 2 == 0) {
                            spaceRight = spaceLeft;
                        }
                        else {
                            spaceRight = spaceLeft + 1;
                        }
                        break;
                    case RIGHT:
                        spaceLeft = spaceTotalLength;
                        spaceRight = 0;
                        break;
                }

                spaceLeftText = repeatText(spaceLeft, params.space);
                spaceRightText = repeatText(spaceRight, params.space);

                result += (repeatText(params.padding, params.space) + spaceLeftText + line + spaceRightText + repeatText(params.padding, params.space) + (i < lines.size() - 1 ? "\n" : ""));
            }

            return result;
        }


        /**
         * Format a given vector of strings into a block of text with a specified length, 
         * with optional padding and centering. The text is split into words and each word is placed on a new line if it exceeds the specified length. 
         * The words are then centered and padded with a given space character.
         * 
         * @param text The text to format, given as a vector of strings.
         * @param params The parameters for formatting the text.
         * 
         * @returns A string consisting of the formatted text.
         */
        string formatText(const vector<string> text, FormatParams params) {
            string result;
            for (auto line : text) {
                result += (formatText(line, params) + "\n");
            }
            return result;
        }
    }

    namespace input {
        void cinHandleFail(bool& fail) {
            fail = cin.fail();

            if (fail) {
                cin.clear();
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        bool getInput(const string prompt, int& var) {
            bool fail;

            cout << " >> " << prompt;
            cin >> var;

            cinHandleFail(fail);
            return fail;
        }

        bool getInput(const string prompt, string& var) {
            cout << " >> " << prompt;
            getline(cin, var);
            return var.empty();
        }
    }
    
}

namespace display {
    namespace components {
        using namespace utils;

        string buildHeader(char lineChar = '=', int length = program::config::lengthHUD, bool newLine = true) {
            return (newLine ? string(length, lineChar) + '\n' : string(length, lineChar));
        }

        string buildHUD() {
            int length = program::config::lengthHUD;

            vector<string> nameArtLines = {
                "░██████╗░░█████╗░██████╗░░░░░░░░░░██████╗██████╗░░██████╗",
                "██╔════╝░██╔══██╗██╔══██╗░░░░░░░░██╔════╝██╔══██╗██╔════╝",
                "██║░░██╗░███████║██████╔╝░█████╗░╚█████╗░██████╔╝░╚████╗░",
                "██║░░╚██╗██╔══██║██╔═══╝░░╚════╝░░╚═══██╗██╔══██╗░╚═══██╗",
                "╚██████╔╝██║░░██║██║░░░░░░░░░░░░░██████╔╝██║░░██║██████╔╝",
                "░╚═════╝░╚═╝░░╚═╝╚═╝░░░░░░░░░░░░░╚═════╝░╚═╝░░╚═╝╚═════╝░" 
            };

            format::FormatParams nameArtFormat;

            nameArtFormat.align = format::CENTER;
            nameArtFormat.limitLength = length;
            nameArtFormat.space = "░";
            nameArtFormat.explicitTextLength = 57;

            string nameArt = format::formatText(nameArtLines, nameArtFormat) + '\n';

            format::FormatParams titleNameFormat = {format::CENTER, length - 20};
            format::FormatParams titleInfoFormat = {format::RIGHT, 19};

            string title = buildHeader() 
                + format::formatText("Geevoi A. Plaza's", titleNameFormat) + "|" + format::formatText("CS-1105", titleInfoFormat) + "\n"
                + format::formatText("Seat Reservation System", titleNameFormat) + "|" + format::formatText("CS 111", titleInfoFormat) + "\n"
                + buildHeader();

            format::FormatParams hudDimensionsFormat = {format::LEFT, length/2};
            format::FormatParams hudOccupiedFormat = {format::RIGHT, (length % 2 == 0 ? length/2 : (length/2) + 1)};

            string output = nameArt + title
                + format::formatText(
                    (to_string(seatrs::data::totalRows) + " Rows x " + to_string(seatrs::data::totalColumns) + " Cols"),
                    hudDimensionsFormat
                ) 
                + format::formatText(
                    (to_string(seatrs::data::totalOccupiedSeats) + "/" + to_string(seatrs::data::totalRows * seatrs::data::totalColumns) + " seats occupied"),
                    hudOccupiedFormat
                ) + '\n'
                + buildHeader('-');

            return output;
        }
    }
}

int main() {
    string str = "░";
    cout << str.length() << endl << display::components::buildHUD() << endl;
    return 0;
}