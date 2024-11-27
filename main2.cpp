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

            vector<string> lines = 
                (params.explicitTextLength > 0)
                    ? vector<string> {text} 
                    : splitWords(text, params.limitLength - (params.padding * 2));

            for (int i = 0; i < lines.size(); i++) {
                string line = lines[i];

                textLength = 
                    (params.explicitTextLength > 0) 
                        ? (params.explicitTextLength)
                        : (text.length());

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

        string formatAsInput(const string& prompt, const string& value = "", const string& indentString = " >> ") {
            return (indentString + prompt + value);
        }
    }

    namespace input {
        /**
         * Handles the failure state of standard input stream (cin).
         * 
         * This function checks if the cin stream is in a failure state and
         * resets it if necessary. It also clears the input buffer to prevent
         * any incorrect data from remaining in the stream.
         * 
         * @param fail A reference to a boolean variable that is set to true
         *             if cin is in a failure state, false otherwise.
         */
        void cinHandleFail(bool& fail) {
            fail = cin.fail();

            if (fail) {
                cin.clear();
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        /**
         * Prompts the user for input and reads an integer into the provided variable.
         * 
         * @param prompt The message displayed to the user before input.
         * @param var A reference to an int variable where the input will be stored.
         * 
         * @returns A boolean indicating whether the input is invalid.
         */
        bool getInput(const string prompt, int& var) {
            bool fail;

            cout << " >> " << prompt;
            cin >> var;

            cinHandleFail(fail);
            return fail;
        }

        /**
         * Prompts the user for input and reads a line of text into the provided variable.
         * 
         * @param prompt The message displayed to the user before input.
         * @param var A reference to a string variable where the input will be stored.
         * 
         * @returns A boolean indicating whether the input was empty.
         */
        bool getInput(const string prompt, string& var) {
            cout << " >> " << prompt;
            getline(cin, var);
            return var.empty();
        }
    }
    
}

namespace display {

    namespace screen {
        void clear() {
            #if defined(__LINUX__) || defined(__APPLE__) || defined(__gnu_linux__) || defined(__linux__)
                cout << "\x1b[2J\x1b[H";
            #else
                cout << string(100, '\n');
            #endif
            return;
        }

        enum Status {
            SUCCESS = 1,
            RETURN = -1,
            FINISH = 0
        };
    }

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

        struct Options {
            string value;
            int maxOption, minOption;
        };


        Options buildOptions(vector<string> options, int offset = 0) {
            int num;
            int optionsSize = options.size();
            
            Options option;
            option.maxOption = optionsSize - 1 + offset;
            option.minOption = offset;

            format::FormatParams formatOptions;
            formatOptions.padding = 7;
            
            for (int i = 0; i < optionsSize; i++) {
                if (offset == 0) {
                    if (i == optionsSize - 1) {
                        num = 0;
                    } else { 
                        num = i + 1;
                    };
                } else {
                    num = offset + i;
                };

                option.value += (format::formatText(" [" + to_string(num) + "] " + options[i], formatOptions) + "\n");
            }
            return option;
        } 
    }

    namespace templates {
        using namespace utils;

        struct HandleIntInputParams {
            string titleText;
            string bodyText;
            string inputPrompt = "Enter your choice: ";
            string errorMessageInvalidType = "Invalid input! Please enter an integer.";
            string errorMessageOutOfRange = "Invalid input! Please enter a valid option.";
            string prevInputText;
            vector<int> abortInvokers = {0};
            int minValue = 0, maxValue = 0;
        };

        struct HandleIntInput {
            int value = 0;
            bool error = false;
            string errorMessage;
            string inputText;
        } defaultIntInput;

        HandleIntInput handleInput(const HandleIntInputParams params, HandleIntInput result = defaultIntInput) {
            // clear screen
            screen::clear();

            format::FormatParams titleFormat;
            titleFormat.align = format::CENTER;
            titleFormat.padding = 2;

            cout << components::buildHUD() 
                << (params.titleText) << endl
                << (params.bodyText.empty() ? "" : params.bodyText + '\n')
                << (result.error ? format::formatText("## " + result.errorMessage + " ##", titleFormat) + '\n' : "")
                << components::buildHeader('-');

            if (!params.prevInputText.empty()) {
                cout << params.prevInputText;
            }

            bool invalid = input::getInput(params.inputPrompt, result.value);

            if (invalid) {
                result.error = true;
                result.errorMessage = params.errorMessageInvalidType;
                return handleInput(params, result);
            } else {
                for (auto &invokerValue : params.abortInvokers) {
                    if (result.error = result.value == invokerValue) {
                        break;
                    }
                    if (!result.error) {
                        if (result.error = result.value < params.minValue || result.value > params.maxValue) {
                            result.errorMessage = params.errorMessageOutOfRange;
                            return handleInput(params, result);
                        }
                    }
                }
            }

            if (!result.error) {
                result.errorMessage.clear();
            }
            result.inputText = 
                params.prevInputText.empty()
                    ? format::formatAsInput(params.inputPrompt, to_string(result.value))
                    : params.prevInputText + "\n" + format::formatAsInput(params.inputPrompt, to_string(result.value));

            return result;
        }

        struct HandleStringInputParams {
            string titleText;
            string bodyText;
            string inputPrompt = "Enter input: ";
            string errorMessageEmpty = "Invalid input! Please enter a non-empty string.";
            string prevInputText;
            vector<string> abortInvokers;
        };

        struct HandleStringInput {
            string value;
            bool error = false;
            string errorMessage;
            string inputText;
        } defaultStringInput;

        HandleStringInput handleInput(const HandleStringInputParams params, HandleStringInput result = defaultStringInput) {
            // clear screen
            screen::clear();

            format::FormatParams titleFormat;
            titleFormat.align = format::CENTER;
            titleFormat.padding = 2;

            cout << components::buildHUD() 
                << (format::formatText(params.titleText, titleFormat)) << endl
                << (params.bodyText.empty() ? "" : params.bodyText + '\n')
                << (result.error ? format::formatText("## " + result.errorMessage + " ##", titleFormat) + '\n' : "")
                << components::buildHeader('-');

            if (!params.prevInputText.empty()) {
                cout << params.prevInputText;
            }

            bool empty = input::getInput(params.inputPrompt, result.value);

            for (auto &invokerValue : params.abortInvokers) {
                if (result.error = result.value == invokerValue) {
                    break;
                }
            }

            if (result.error = ((!result.error) && empty && (!params.errorMessageEmpty.empty()))) {
                result.errorMessage = params.errorMessageEmpty;
                return handleInput(params, result);
            }

            result.inputText = 
                params.prevInputText.empty()
                    ? format::formatAsInput(params.inputPrompt, result.value)
                    : params.prevInputText + "\n" + format::formatAsInput(params.inputPrompt, result.value);

            return result;
        }

        struct PostScreenParams {
            string titleText;
            string bodyText;
            string errorMessage;
            string prompt = "Enter input: ";
            vector<string> abortInvokers = {"0"};
        };

        int postScreen(const PostScreenParams params) {
            // clear screen
            screen::clear();
            string value;

            format::FormatParams titleFormat;
            titleFormat.align = format::CENTER;
            titleFormat.padding = 2;

            cout << components::buildHUD() 
                << (format::formatText(params.titleText, titleFormat)) << endl
                << (params.bodyText.empty() ? "" : params.bodyText + '\n')
                << (params.errorMessage.empty() ? "" : format::formatText("## " + params.errorMessage + " ##", titleFormat) + '\n')
                << components::buildHeader('-');

            input::getInput(params.prompt, value);

            for (auto &invokerValue : params.abortInvokers) {
                if (value == invokerValue) {
                    return screen::Status::RETURN;
                }
            }

            return screen::Status::SUCCESS;
        }

        struct RowColumnParams {
            string titleText;
            string bodyText;
            string inputPromptRow = "Enter Row number: ";
            string inputPromptColumn = "Enter Column number: ";
            string errorMessageInvalidType = "Invalid input! Please enter an integer.";
            string errorMessageRowOutOfRange = "Invalid input! Number of row must be between 1 and " + to_string(seatrs::data::totalRows) + ".";
            string errorMessageColumnOutOfRange = "Invalid input! Number of column must be between 1 and " + to_string(seatrs::data::totalColumns) + ".";
            int maxInputValueRow = seatrs::data::totalRows;
            int maxInputValueColumn = seatrs::data::totalColumns;
        };

        struct RowColumn {
            int row;
            int column;
            bool error = false;
        } defaultRowColumn;


        RowColumn getRowColumn(const RowColumnParams params, RowColumn result = defaultRowColumn) {
            templates::HandleIntInputParams sharedParams, paramsRow, paramsColumn;
            
            sharedParams.titleText = params.titleText;
            sharedParams.bodyText = params.bodyText;
            sharedParams.errorMessageInvalidType = params.errorMessageInvalidType;
            sharedParams.minValue = 1;
            
            paramsRow = paramsColumn = sharedParams;

            paramsRow.errorMessageOutOfRange = params.errorMessageRowOutOfRange;
            paramsRow.inputPrompt = params.inputPromptRow;
            paramsRow.maxValue = params.maxInputValueRow;

            paramsColumn.errorMessageOutOfRange = params.errorMessageColumnOutOfRange;
            paramsColumn.inputPrompt = params.inputPromptColumn;
            paramsColumn.maxValue = params.maxInputValueColumn;

            templates::HandleIntInput resultRow, resultColumn;

            do {
                resultRow = templates::handleInput(paramsRow);

                if (result.error = resultRow.error) {
                    break;
                }
                
                paramsColumn.prevInputText = resultRow.inputText;
                
                resultColumn = templates::handleInput(paramsColumn);

                result.error = resultColumn.error;

            } while (result.error);

            if (!result.error) {
                result.row = resultRow.value;
                result.column = resultColumn.value;
            }

            return result;
        }

        struct NameDescriptionParams {
            string titleText;
            string bodyText;
            string inputPromptName = "Enter Name: ";
            string inputPromptDescription = "Enter Description: ";
            string errorMessageEmptyName = "Invalid input! Please enter a non-emtpy string.";
            string errorMessageEmptyDescription;
        };

        struct NameDescription {
            string name;
            string description;
            bool error = false;
        } defaultNameDescription;

        NameDescription getNameDescription(const NameDescriptionParams params, NameDescription result = defaultNameDescription) {
            templates::HandleStringInputParams sharedParams, paramsName, paramsDescription;

            sharedParams.titleText = params.titleText;
            sharedParams.bodyText = params.bodyText;

            paramsName = paramsDescription = sharedParams;

            paramsName.inputPrompt = params.inputPromptName;
            paramsName.errorMessageEmpty = params.errorMessageEmptyName;

            paramsDescription.inputPrompt = params.inputPromptDescription;
            paramsDescription.errorMessageEmpty = params.errorMessageEmptyDescription;

            templates::HandleStringInput resultName, resultDescription;

            do {
                resultName = templates::handleInput(paramsName);

                if (result.error = resultName.error) {
                    break;
                }

                paramsDescription.prevInputText = resultName.inputText;

                resultDescription = templates::handleInput(paramsDescription);

                result.error = resultDescription.error;
            } while (result.error);

            if (!result.error) {
                result.name = resultName.value;
                result.description = resultDescription.value;
            }

            return result;
        }
    }

    namespace screen {
        using namespace utils;

        int mainMenu() {
            int status;
            templates::HandleIntInputParams choiceParams;

            choiceParams.titleText = 
                "[Main Menu]\n" 
                "Choose an option.";
            
            components::Options options = components::buildOptions({
                "Display Seat Layout",
                "Create Seat Reservation",
                "Read/Display Seat Reservation",
                "Update Seat Reservation",
                "Delete/Cancel Seat Reservation",
                "Options (-> Exit)"
            });

            choiceParams.bodyText = options.value;
            choiceParams.minValue = options.minOption;
            choiceParams.maxValue = options.maxOption;

            do {
                templates::HandleIntInput result = templates::handleInput(choiceParams);

                switch (result.value) {
                    case 2: {
                        status = 1;
                        break;
                    }
                    case 0: {
                        status = 0;
                        break;
                    }
                    
                }
            } while (status != 0);
        
            return 0;
        }
    }
}

int main() {
    return display::screen::mainMenu();
}