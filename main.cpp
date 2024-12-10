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
        Seat** seats;

        int totalRows = 10;
        int totalColumns = 10;
    }

    /**
     * Sets the size of the seat layout to the given number of rows and columns.
     * If the new size is larger than the old size, the old data is copied into the
     * new array. If the new size is smaller than the old size, the extra data is
     * lost.
     * 
     * @param rows The number of rows in the new seat layout.
     * @param columns The number of columns in the new seat layout.
     */
    void setSize(int rows = 10, int columns = 10) {

        // Create a 2d array with the specified size
        Seat** newSeats = new Seat*[rows];
        for (int iRow = 0; iRow < rows; iRow++) {
            newSeats[iRow] = new Seat[columns];
        }

        if (data::seats != nullptr) {

            // Copy as much of the old array into the new array
            for (
                int iRow = 0; 
                (
                    data::totalRows < rows 
                        ? iRow < data::totalRows 
                        : iRow < rows
                ); 
                iRow++
            ) {
                for (int iColumn = 0; (data::totalColumns < columns ? iColumn < data::totalColumns : iColumn < columns); iColumn++) {
                    newSeats[iRow][iColumn] = data::seats[iRow][iColumn]; 
                }
            }

            // Delete the old 2d array to free up memory
            for (int iRow = 0; iRow < data::totalRows; iRow++) {
                delete[] data::seats[iRow];
            }
            delete[] data::seats;
        }

        // Update the seats pointer and dimensions
        data::seats = newSeats;
        data::totalRows = rows;
        data::totalColumns = columns;
    }

    /**
     * Checks if a given seat position is valid.
     * 
     * @param irow The row of the seat
     * @param icol The column of the seat
     * 
     * @returns true if the seat is valid, false otherwise
     */
    bool isValidSeat(int irow, int icol) {
        return (irow >= 0) && (irow < data::totalRows) && 
        (icol >= 0) && (icol < data::totalColumns);
    }
}

namespace program {
    namespace config {
        int lengthHUD = 80;
    }

    namespace control {
        const int minLengthHUD = 60;
        const int maxLengthHUD = 100;
        const int maxPossibleRows = maxLengthHUD;
        const int maxPossibleColumns = maxLengthHUD;
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
                                        //   specifically for the nameArt to work.
        } optionsFormat = {LEFT, -1, 8};

        /**
         * Format a given string into a block of text with a specified length, 
         * with optional padding and centering. The text is split into words and 
         * each word is placed on a new line if it exceeds the specified length. 
         * The words are then formatted and padded with a given space character.
         * 
         * @param text The text to format.
         * @param params The parameters for formatting the text.
         * 
         * @returns A string consisting of the formatted text.
         */
        string formatText(const string& text, FormatParams params) {
            int spaceLeft, spaceRight;
            int lineLength;
            string spaceLeftText, spaceRightText;
            string result;

            vector<string> lines = 
                (params.explicitTextLength > 0)
                    ? vector<string> {text} // the text length is explicitly defined,
                                            //   expected to only be in one line.  
                    : splitWords(text, params.limitLength - (params.padding * 2));  // get the lines after splitting the words 
                                                                                    //   based on the limit length for each line.

            for (int i = 0; i < lines.size(); i++) {
                string line = lines[i];

                lineLength = 
                    (params.explicitTextLength > 0) 
                        ? (params.explicitTextLength)
                        : (line.length());

                if (params.limitLength < 0) {
                    params.limitLength = program::config::lengthHUD;
                }
                
                // Determine how much space each line should have, excluding padding and line length
                int spaceTotalLength = params.limitLength - lineLength - (params.padding * 2);

                // Determine in what alignment each line should be formatted in
                switch (params.align) {
                    case LEFT:
                        spaceLeft = 0;
                        spaceRight = spaceTotalLength;
                        break;
                    case CENTER:
                        spaceLeft = ((params.limitLength - lineLength) / 2) - params.padding;
                        if ((params.limitLength - lineLength) % 2 == 0) {
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
         * with optional padding and centering. The words are then formatted and padded
         * with a given space character.
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

        /**
         * Format a given input prompt and value as a string. Used to
         * save a user's input for continuous prompts.
         * 
         * @param prompt The input prompt to format.
         * @param value The value entered by the user, if any.
         * @param indentString The optional indent string to prepend to the formatted string.
         * 
         * @returns A string consisting of the formatted input prompt and value.
         */
        string formatAsInput(const string& prompt, const string& value = "", const string& indentString = " >> ") {
            return (indentString + prompt + value + '\n');
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
            // Learned from: https://stackoverflow.com/a/60890250

            // Check if cin is in a failure state
            fail = cin.fail();

            if (fail) {
                cin.clear();
            }

            // Clearing the input buffer regardless of fail allows to clean excess input
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
         * @returns A boolean indicating whether the input was invalid (empty).
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
        
        /**
         * Clears the console screen and moves the cursor to the top left position.
         * 
         * This function will work on both Windows and Unix-like systems.
         */
        void clear() {
            #if defined(__LINUX__) || defined(__APPLE__) || defined(__gnu_linux__) || defined(__linux__)
                cout << "\x1b[2J\x1b[H";
                // credit: https://stackoverflow.com/a/6487534
            #else
                cout << string(100, '\n');
                // Windows is not a Unix-like system, thus we use a different, less preferable method
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

        /**
         * Builds a header string consisting of a repeated character.
         *
         * @param lineChar The character to repeat for the header line.
         * @param length The total length of the header line.
         * @param newLine Whether to append a newline character at the end of the header.
         *
         * @returns A string consisting of the repeated character forming a header.
         */
        string buildHeader(char lineChar = '=', int length = program::config::lengthHUD, bool newLine = true) {
            return (newLine ? string(length, lineChar) + '\n' : string(length, lineChar));
        }

        /**
         * Builds the entire HUD string, consisting of the name art, title, and information about the
         * layout dimensions and the number of occupied seats.
         *
         * @returns A string representing the entire HUD.
         */
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

        /**
         * Handles integer input with validation and error handling, recursively prompting the user until valid input is received.
         *
         * @param params A structure containing parameters for input handling.
         * @param result An optional parameter if a previous result is available.
         *
         * @returns A HandleIntInput structure containing the validated input value.
         */
        HandleIntInput handleInput(
            const HandleIntInputParams params, 
            HandleIntInput result = defaultIntInput
        ) {
            // clear screen
            screen::clear();

            format::FormatParams titleFormat;
            titleFormat.align = format::CENTER;
            titleFormat.padding = 2;

            cout << components::buildHUD() 
                << (format::formatText(params.titleText, titleFormat)) << endl
                << (result.error ? format::formatText("## " + result.errorMessage + " ##", titleFormat) + '\n' : "")
                << (params.bodyText.empty() ? "" : params.bodyText + '\n')
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

        /**
         * Handles string input from the user, displaying a formatted screen with instructions
         * and error messages. Recursively prompts the user for input if validation fails.
         *
         * @param params A structure containing parameters for input handling.
         * @param result An optional parameter if a previous result is available.
         * @returns A HandleIntInput structure containing the validated input value.
         */
        HandleStringInput handleInput(const HandleStringInputParams params, HandleStringInput result = defaultStringInput) {
            // clear screen
            screen::clear();

            format::FormatParams titleFormat;
            titleFormat.align = format::CENTER;
            titleFormat.padding = 2;

            cout << components::buildHUD() 
                << (format::formatText(params.titleText, titleFormat)) << endl
                << (result.error ? format::formatText("## " + result.errorMessage + " ##", titleFormat) + '\n' : "")
                << (params.bodyText.empty() ? "" : params.bodyText + '\n')
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

        /**
         * Displays a formatted screen with instructions and error messages, and 
         * prompts the user for input.
         *
         * @param params A structure containing parameters for input handling.
         *
         * @returns An integer indicating the status of the function.
         *          SUCCESS if the user entered a valid input.
         *          RETURN if the user chose to abort.
         */
        int postScreen(const PostScreenParams params) {
            // clear screen
            screen::clear();
            string value;

            format::FormatParams titleFormat;
            titleFormat.align = format::CENTER;
            titleFormat.padding = 2;

            cout << components::buildHUD() 
                << (format::formatText(params.titleText, titleFormat)) << endl
                << (params.errorMessage.empty() ? "" : format::formatText("## " + 
                    params.errorMessage + " ##", titleFormat) + '\n')
                << (params.bodyText.empty() ? "" : params.bodyText + '\n')
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


        /**
         * Prompts the user to enter row and column numbers, and returns their values 
         * in a RowColumn structure.
         *
         * @param params A structure containing parameters for input handling.
         *
         * @returns A RowColumn structure, with the "error" field set to true if the 
         * user entered invalid data or aborted , and the "row" and "column" fields 
         * set to the entered values if the input was valid.
         */
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

        /**
         * Get the name and description from user input.
         *
         * @param params The parameters for the function.
         *
         * @param result The result of the function. If the result.error is true, the user aborted.
         * Otherwise, the result.name and result.description contain the name and description entered by the
         * user.
         *
         * @return The result of the function.
         */
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

        int optionsSetHUDLength() {
            templates::HandleIntInputParams setHUDLengthParams;

            setHUDLengthParams.titleText =
                "[Edit HUD Length]\n"
                "Enter new HUD length.";
            
            setHUDLengthParams.bodyText = format::formatText(
                "[0] Return to Main Menu\n", 
                format::optionsFormat
            );

            setHUDLengthParams.errorMessageOutOfRange = "Invalid input! Please enter an integer between " + to_string(program::control::minLengthHUD) + " and " + to_string(program::control::maxLengthHUD) + ".";
            setHUDLengthParams.inputPrompt = "Enter new HUD length: ";
            setHUDLengthParams.minValue = program::control::minLengthHUD;
            setHUDLengthParams.maxValue = program::control::maxLengthHUD;

            templates::HandleIntInput result = templates::handleInput(setHUDLengthParams);

            if (result.error) {
                return RETURN;
            }

            program::config::lengthHUD = result.value;

            return SUCCESS;
        }

        int optionsSetDimensions() {
            templates::RowColumnParams rowColumnParams;

            rowColumnParams.titleText = 
                "[Edit Seat Layout Dimensions]\n" 
                "Enter new dimensions.";
            
            rowColumnParams.errorMessageRowOutOfRange = "Invalid input! Number of row must be between 1 and " + to_string(program::control::maxPossibleRows) + ".";
            rowColumnParams.errorMessageColumnOutOfRange = "Invalid input! Number of column must be between 1 and " + to_string(program::control::maxPossibleColumns) + ".";

            rowColumnParams.inputPromptRow = "Enter the number of rows: ";
            rowColumnParams.inputPromptColumn = "Enter the number of columns: ";

            rowColumnParams.maxInputValueRow = program::control::maxPossibleRows;
            rowColumnParams.maxInputValueColumn = program::control::maxPossibleColumns;

            templates::RowColumn result = getRowColumn(rowColumnParams);

            if (result.error) {
                return RETURN;
            } 
            
            seatrs::setSize(result.row, result.column);
            return SUCCESS;
        }

        int optionsMenu() {
            int status;
            templates::HandleIntInputParams choiceParams;

            choiceParams.titleText = 
                "[Settings]\n" 
                "Choose an option.";
            
            choiceParams.bodyText = format::formatText(
                "[1] Exit\n"
                "[2] Edit Seat Layout Dimensions\n"
                "[3] Edit HUD Length\n"
                "[0] Return to Main Menu\n", 
                format::optionsFormat
            );
            choiceParams.minValue = 0;
            choiceParams.maxValue = 3;

            do {
                templates::HandleIntInput result = templates::handleInput(choiceParams);

                switch (result.value) {
                    case 1: {
                        status = FINISH;
                        break;
                    }
                    case 2: {
                        status = optionsSetDimensions();
                        break;
                    }
                    case 3: {
                        status = optionsSetHUDLength();
                        break;
                    }
                    case 0: {
                        status = SUCCESS;
                        break;
                    }
                }
            } while (status == RETURN);

            return status;
        }

        int showSeatLayout() {
            templates::PostScreenParams seatLayoutParams;

            seatLayoutParams.titleText = "[Show Seat Layout]";
            string bodytextInstructions = format::formatText(
                "[Enter] Return to Main Menu\n", 
                format::optionsFormat
            );

            string bodyText;
            string separator;

            if ((seatrs::data::totalColumns*2) + 1 <= program::config::lengthHUD) {
                separator = " ";
            } else if (seatrs::data::totalColumns + 1 > program::config::lengthHUD) {
                seatLayoutParams.bodyText = bodytextInstructions;
                seatLayoutParams.errorMessage = "The columns of the seat layout exceeds the HUD length!";
                templates::postScreen(seatLayoutParams);
                return RETURN;
            }

            bodyText = " ";

            int count = 1;

            for (int icol = 0; icol < seatrs::data::totalColumns; icol++) {
                bodyText += (
                    separator.empty() 
                        ? to_string(count)
                        : separator + to_string(count)
                );
                count++;
                if (count % 10 == 0) count = 0;
            }
            bodyText += '\n';

            count = 1;
            for (int irow = 0; irow < seatrs::data::totalRows; irow++) {
                bodyText += to_string(count);
                for (int icol = 0; icol < seatrs::data::totalColumns; icol++) {
                    bodyText += (separator + (
                        seatrs::data::seats[irow][icol].isReserved
                            ? "X"
                            : "O"
                    ));
                }
                bodyText += '\n';
                count++;
                if (count % 10 == 0) count = 0;
            }

            seatLayoutParams.bodyText = '\n' + format::formatText(bodyText, {format::CENTER}) + "\n\n" + bodytextInstructions;
            seatLayoutParams.titleText += "\nNot Occupied O | X Occupied";

            templates::postScreen(seatLayoutParams);

            return SUCCESS;
        }

        int createReservation() {
            int status;

            string bodyText = format::formatText(
                "[0] Return to Main Menu\n", 
                format::optionsFormat
            );

            templates::RowColumnParams rcParams;
            rcParams.titleText = 
                "[Create Seat Reservation]\n"
                "Enter the row and column of the seat.";
            rcParams.bodyText = bodyText;
            templates::RowColumn rcResult;
            
            templates::NameDescriptionParams ndParams;
            ndParams.titleText = 
                "[Create Seat Reservation]\n"
                "Enter the Name and Description for the reservation.";
            ndParams.bodyText = bodyText;
            templates::NameDescription ndResult;
            
            templates::PostScreenParams postParams;
            postParams.bodyText = format::formatText(
                "[0] Create another Seat Reservation\n"
                "[Enter] Return to Main Menu\n", 
                format::optionsFormat
            );
            
            do {
                postParams.titleText = "[Create Seat Reservation]";

                rcResult = templates::getRowColumn(rcParams);

                if (rcResult.error) {
                    status = SUCCESS;
                    break;
                }

                int irow = rcResult.row - 1;
                int icolumn = rcResult.column - 1;

                if (seatrs::isValidSeat(irow, icolumn)) {
                    if (seatrs::data::seats[irow][icolumn].isReserved) {
                        postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] is already reserved.";
                        status = templates::postScreen(postParams);
                        continue;
                    }
                } else {
                    postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] does not exist.";
                    status = templates::postScreen(postParams);
                    continue;
                }

                ndResult = templates::getNameDescription(ndParams);

                if (ndResult.error) {
                    status == RETURN;
                } else {
                    seatrs::Seat &seat = seatrs::data::seats[irow][icolumn];
                    seat.name = ndResult.name;
                    seat.description = ndResult.description;
                    seat.isReserved = true;

                    postParams.titleText = 
                        "[Create Seat Reservation]\n"
                        "Reservation created successfully.";
                    postParams.errorMessage.clear();
                    status = templates::postScreen(postParams);
                }

            } while (status == RETURN);

            return status;
        }

        int readReservation() {
            int status;

            templates::RowColumnParams rcParams;
            rcParams.titleText = 
                "[View/Read Seat Reservation]\n"
                "Enter the row and column of the seat reservation to read.";
            rcParams.bodyText = format::formatText(
                "[0] Return to Main Menu\n", 
                format::optionsFormat
            );
            templates::RowColumn rcResult;

            format::FormatParams detailsFormat;
            detailsFormat.align = format::LEFT;
            detailsFormat.padding = 2;
            
            templates::PostScreenParams postParams;
            postParams.bodyText = format::formatText(
                "[0] Read/View another Seat Reservation\n"
                "[Enter] Return to Main Menu\n", 
                format::optionsFormat
            );
            
            do {
                postParams.titleText = "[Read/View Seat Reservation]";

                rcResult = templates::getRowColumn(rcParams);

                if (rcResult.error) {
                    status = SUCCESS;
                    break;
                } else {
                    int irow = rcResult.row - 1;
                    int icolumn = rcResult.column - 1;

                    if (seatrs::isValidSeat(irow, icolumn)) {
                        if (!seatrs::data::seats[irow][icolumn].isReserved) {
                            postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] is not reserved.";
                            status = templates::postScreen(postParams);
                            continue;
                        }
                    } else {
                        postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] does not exist.";
                        status = templates::postScreen(postParams);
                        continue;
                    }

                    seatrs::Seat &seat = seatrs::data::seats[irow][icolumn];

                    postParams.bodyText = format::formatText(
                        (
                            "\n"
                            "This seat is reserved by:\n"
                            " >> Name: " + seat.name + "\n"
                            " >> Description: " + seat.description
                        ),
                        detailsFormat
                    ) + "\n\n" + postParams.bodyText;
                    postParams.titleText = 
                        "[View/Read Seat Reservation]\n"
                        "Seat Reservation Details:";
                    postParams.errorMessage.clear();
                    status = templates::postScreen(postParams);
                }
                

            } while (status == RETURN);

            return status;
        }

        int updateReservation() {
            int status;

            string bodyText = format::formatText(
                "[0] Return to Main Menu\n", 
                format::optionsFormat
            );

            templates::RowColumnParams rcParams;
            rcParams.titleText = 
                "[Update Seat Reservation]\n"
                "Enter the row and column of the seat.";
            rcParams.bodyText = bodyText;
            templates::RowColumn rcResult;
            
            templates::NameDescriptionParams ndParams;
            ndParams.titleText = 
                "[Update Seat Reservation]\n"
                "Enter the Name and Description for the reservation.";
            ndParams.bodyText = bodyText;
            templates::NameDescription ndResult;
            
            templates::PostScreenParams postParams;
            postParams.bodyText = format::formatText(
                "[0] Update another Seat Reservation\n"
                "[Enter] Return to Main Menu\n", 
                format::optionsFormat
            );
            
            do {
                postParams.titleText = "[Update Seat Reservation]";

                rcResult = templates::getRowColumn(rcParams);

                if (rcResult.error) {
                    status = SUCCESS;
                    break;
                }

                int irow = rcResult.row - 1;
                int icolumn = rcResult.column - 1;

                if (seatrs::isValidSeat(irow, icolumn)) {
                    if (!seatrs::data::seats[irow][icolumn].isReserved) {
                        postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] is not reserved.";
                        status = templates::postScreen(postParams);
                        continue;
                    }
                } else {
                    postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] does not exist.";
                    status = templates::postScreen(postParams);
                    continue;
                }

                ndResult = templates::getNameDescription(ndParams);

                if (ndResult.error) {
                    status == RETURN;
                } else {
                    seatrs::Seat &seat = seatrs::data::seats[irow][icolumn];
                    seat.name = ndResult.name;
                    seat.description = ndResult.description;
                    seat.isReserved = true;

                    postParams.titleText = 
                        "[Update Seat Reservation]\n"
                        "Reservation updated successfully.";
                    postParams.errorMessage.clear();
                    status = templates::postScreen(postParams);
                }

            } while (status == RETURN);

            return status;
        }

        int deleteReservation() {
            int status;

            templates::RowColumnParams rcParams;
            rcParams.titleText = 
                "[Delete Seat Reservation]\n"
                "Enter the row and column of the seat reservation to cancel.";
            rcParams.bodyText = format::formatText(
                "[0] Return to Main Menu\n", 
                format::optionsFormat
            );
            templates::RowColumn rcResult;
            
            templates::PostScreenParams postParams;
            postParams.bodyText = format::formatText(
                "[0] Delete another Seat Reservation\n"
                "[Enter] Return to Main Menu\n", 
                format::optionsFormat
            );
            
            do {
                postParams.titleText = "[Update Seat Reservation]";

                rcResult = templates::getRowColumn(rcParams);

                if (!rcResult.error) {
                    int irow = rcResult.row - 1;
                    int icolumn = rcResult.column - 1;

                    if (seatrs::isValidSeat(irow, icolumn)) {
                        if (!seatrs::data::seats[irow][icolumn].isReserved) {
                            postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] is not reserved.";
                            status = templates::postScreen(postParams);
                            continue;
                        }
                    } else {
                        postParams.errorMessage = "The seat [" + to_string(irow + 1) + ", " + to_string(icolumn + 1) + "] does not exist.";
                        status = templates::postScreen(postParams);
                        continue;
                    }
                    
                    seatrs::Seat &seat = seatrs::data::seats[irow][icolumn];
                    seat.description.clear();
                    seat.name.clear();
                    seat.isReserved = false;

                    postParams.titleText = 
                        "[Delete Seat Reservation]\n"
                        "Reservation deleted successfully.";
                    postParams.errorMessage.clear();
                    status = templates::postScreen(postParams);
                }

                status = SUCCESS;

            } while (status == RETURN);

            return status;
        }

        int mainMenu() {
            int status;
            templates::HandleIntInputParams choiceParams;

            choiceParams.titleText = 
                "[Main Menu]\n" 
                "Choose an option.";
            
            choiceParams.bodyText = format::formatText(
                "[1] Display Seat Layout\n"
                "[2] Create Seat Reservation\n"
                "[3] Read/Display Seat Reservation\n"
                "[4] Update Seat Reservation\n"
                "[5] Delete/Cancel Seat Reservation\n"
                "[0] Settings (-> Exit)\n", 
                format::optionsFormat
            );
            choiceParams.minValue = 0;
            choiceParams.maxValue = 5;

            do {
                templates::HandleIntInput result = templates::handleInput(choiceParams);

                switch (result.value) {
                    case 1: {
                        status = showSeatLayout();
                        break;
                    }
                    case 2: {
                        status = createReservation();
                        break;
                    }
                    case 3: {
                        status = readReservation();
                        break;
                    }
                    case 4: {
                        status = updateReservation();
                        break;
                    }
                    case 5: {
                        status = deleteReservation();
                        break;
                    }
                    case 0: {
                        status = optionsMenu();
                        break;
                    }
                    
                }
            } while (status != 0);
        
            return 0;
        }
    }
}


int main() {
    seatrs::setSize();
    return display::screen::mainMenu();
}