// Geevoi A. Plaza
// CS-1105
// CS111 - Computer Programming

#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <sstream>

using namespace std;



namespace program::options {
    int maxPossibleRows = 100;
    int maxPossibleColumns = 100;
    int lengthHUD = 80;
}

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

    
    void setSize(int rows = 10, int columns = 10) {
        data::totalRows = rows;
        data::totalColumns = columns; 
        data::seats.resize(rows);
        for (auto &row : data::seats) {
            row.resize(columns);
        }
    }

    bool isValidSeat(int row, int col) {
        return row > 0 && row <= data::totalRows && col > 0 && col <= data::totalColumns;
    }
}

// Reusable utilities for the program
namespace utils {
    namespace config {
        int defaultSplitRatio[2] = { 1, 1 };
        const int minimumLengthHUD = 60;
    }

    namespace format {

        string stringRepeat(int num, string text) {
            string result;
            for (int i = 0; i < num; i++) {
                result += text;
            }
            return result;
        }

        int* splitLength(int length = program::options::lengthHUD, int ratio[2] = config::defaultSplitRatio) {

            int ratioSum = ratio[0] + ratio[1];
            int length1 = (length * ratio[0]) / ratioSum;
            int length2 = length * ratio[1];
            if (length2 % ratioSum == 0) {
                length2 /= ratioSum;
            }
            else {
                length2 = (length2 /= ratioSum) + 1;
            }

            static int lengths[2] = { length1, length2 };

            return lengths;
        }

        enum PrintPosition { LEFT, CENTER, RIGHT };

        struct PrintOptions {
            PrintPosition position = LEFT;
            int length = -1;
            string space = " ";
            int explicitTextLength = 0;
        } defaultPrintOptions;

        string buildTextFormat(const string text, PrintOptions options = defaultPrintOptions) {
            int spaceLengthLeft, spaceLengthRight;

            int textLength; 

            if (options.explicitTextLength > 0) {
                textLength = options.explicitTextLength;
            }
            else {
                textLength = text.length();
            }

            if (options.length == -1) {
                options.length = program::options::lengthHUD;
            }

            switch (options.position) {
                case LEFT: {
                    spaceLengthLeft = 0;
                    spaceLengthRight = options.length - textLength;
                    break;
                }
                case CENTER: {
                    spaceLengthLeft = (options.length - textLength) / 2;
                    if ((options.length - textLength) % 2 == 0) {
                        spaceLengthRight = spaceLengthLeft;
                    }
                    else {
                        spaceLengthRight = spaceLengthLeft + 1;
                    }
                    break;
                }
                case RIGHT: {
                    spaceLengthLeft = options.length - textLength;
                    spaceLengthRight = 0;
                    break;
                }
            }

            return (stringRepeat(spaceLengthLeft, options.space) + text + stringRepeat(spaceLengthRight, options.space));
        }

        string print(const string text, PrintOptions options = defaultPrintOptions) {
            cout << buildTextFormat(text, options);
            return "";
        }
        vector<string> splitWords(const string& input, size_t limit) {
            string word, result;
            size_t currentLength = 0;
            vector<string> results;

            size_t start = 0;
            // Collect leading spaces for the first line
            while (start < input.length() && input[start] == ' ') {
                result += input[start];
                start++;
            }

            currentLength = result.length();

            // Process the rest of the input word by word
            for (size_t i = start; i < input.length(); ) {
                // Skip extra spaces
                while (i < input.length() && input[i] == ' ') i++;
                size_t wordStart = i;
                while (i < input.length() && input[i] != ' ') i++;
                size_t wordLength = i - wordStart;

                // Check if the current word fits in the line
                if (currentLength + wordLength + (currentLength > 0 && result[currentLength - 1] != ' ' ? 1 : 0) > limit) {
                    results.push_back(result);
                    result.clear();
                    currentLength = 0;
                }

                // Append the word to the current line
                if (!result.empty() && result[currentLength - 1] != ' ') {
                    result += ' ';
                    currentLength++;
                }
                result += input.substr(wordStart, wordLength);
                currentLength += wordLength;
            }

            // Add the last line if not empty
            if (!result.empty()) {
                results.push_back(result);
            }

            return results;
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

        bool getInput(const string prompt, char& var) {
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

        string buildAsInput(const string prompt, string value = "") {
            return " >> " + prompt + value + "\n";
        }
        
    }

}

namespace program {

    namespace components {

        using namespace utils;

        string buildHeader(char lineChar = '=', int length = program::options::lengthHUD, bool newLine = true) {
            return (newLine ? string(length, lineChar) + '\n' : string(length, lineChar));
        }

        string displayHeader(char lineChar = '=', int length = program::options::lengthHUD, bool newLine = true) {
            cout << buildHeader(lineChar, length, newLine);
            return "";
        }

        string buildNameArt(int length = program::options::lengthHUD) {

            string lines[] = {
                "░██████╗░░█████╗░██████╗░░░░░░░░░░██████╗██████╗░░██████╗",
                "██╔════╝░██╔══██╗██╔══██╗░░░░░░░░██╔════╝██╔══██╗██╔════╝",
                "██║░░██╗░███████║██████╔╝░█████╗░╚█████╗░██████╔╝░╚████╗░",
                "██║░░╚██╗██╔══██║██╔═══╝░░╚════╝░░╚═══██╗██╔══██╗░╚═══██╗",
                "╚██████╔╝██║░░██║██║░░░░░░░░░░░░░██████╔╝██║░░██║██████╔╝",
                "░╚═════╝░╚═╝░░╚═╝╚═╝░░░░░░░░░░░░░╚═════╝░╚═╝░░╚═╝╚═════╝░" 
            };

            string output;


            for (int i = 0; i < sizeof(lines)/sizeof(*lines); i++) {
                output += (format::buildTextFormat(lines[i], {format::CENTER, length, "░", 57}) + "\n");
            }

            return output;
        }

        string buildTitle() {

            format::PrintOptions formatCenter = {format::CENTER, options::lengthHUD - 20};
            format::PrintOptions formatRight = {format::RIGHT, 19};

            string output = buildNameArt()
                + buildHeader()
                + format::buildTextFormat("Geevoi A. Plaza's", formatCenter) + "|" + format::buildTextFormat("CS-1105", formatRight) + "\n"
                + format::buildTextFormat("Seat Reservation System", formatCenter) + "|" + format::buildTextFormat("CS 111", formatRight) + "\n"
                + buildHeader();

            return output;
        }

        string buildHUD() {
            int* lengthRatio = format::splitLength();
            string output = buildTitle()
                + format::buildTextFormat(
                    (
                        to_string(seatrs::data::totalRows) 
                            + " Rows x " 
                            + to_string(seatrs::data::totalColumns) 
                            + " Cols"
                    ), 
                    {format::LEFT, lengthRatio[0]}
                )
                + format::buildTextFormat(
                    (
                        to_string(seatrs::data::totalOccupiedSeats) 
                            + "/" 
                            + to_string(seatrs::data::totalRows * seatrs::data::totalColumns) 
                            + " seats occupied"
                    ), 
                    {format::RIGHT, lengthRatio[1]}
                ) + "\n"
                + buildHeader('-');
            
            return output;
        }

        struct InstructionParams {
            vector<string> instructions;
            string errorMessage;
            bool error = false;
            bool persistNewLine = true;
            format::PrintPosition position = format::CENTER;
        } instructionParams;

        string buildInstructions(InstructionParams params) {
            string output;

            for (auto &instruction : params.instructions) {
                output += (format::buildTextFormat(instruction, {params.position}) + "\n");
            }

            if (params.error) {
                output +=(format::buildTextFormat("## " + params.errorMessage + " ##", {params.position}) + "\n");
            } else if (params.persistNewLine) {
                output += "\n";
            }

            return output;
        }


        string buildOptions(vector<string> options, int offset = 0) {
            string output = "";
            int num;
            int optionsSize = options.size();

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

                output += (format::buildTextFormat(("        [" + to_string(num) + "] " + options[i] + " ")) + "\n");
            }
            return output;
        }
    }
}

namespace display {

    void clear() {
        #if defined(__LINUX__) || defined(__APPLE__) || defined(__gnu_linux__) || defined(__linux__)
            cout << "\x1b[2J\x1b[H";
        #else
            cout << string(100, '\n');
        #endif
        return;
    }

    namespace screenTemplate {

        using namespace program;
        using namespace utils;

        struct HandleInputParams {
            vector<string> instructions;
            vector<string> options;
            int maxInputValue;
            int minInputValue;
            string prevInput;
            vector<int> returnInvokers;
            string inputPrompt = "Enter your choice: ";
            string errorMessage;
            string errorMessageInvalidType = "Invalid input! Please enter an integer";
            string errorMessageOutOfRange = "Invalid input! Please enter a valid option.";
            bool persistNewLine = false;
            int optionsOffset = 0;
        };

        struct HandleInput {
            int value = 0;
            bool error = false;
            string inputText;
        } defaultHandleInput;

        HandleInput handleInput(HandleInputParams params, HandleInput result = defaultHandleInput) {
            if ((params.options.size() != 0) && (params.maxInputValue == 0) && (params.minInputValue == 0)) {
                params.maxInputValue = params.options.size() - 1 + params.optionsOffset;
                params.minInputValue = params.optionsOffset;
            }

            // do {
            clear();

            cout << components::buildHUD() 
                << components::buildInstructions({
                    params.instructions,
                    params.errorMessage,
                    result.error,
                    params.persistNewLine
                });

            if (params.options.size() > 0) {
                cout << components::buildOptions(params.options , params.optionsOffset);
            }

            cout << components::buildHeader('-');

            if (!params.prevInput.empty()) {
                cout << params.prevInput;
            }

            bool fail = input::getInput(params.inputPrompt, result.value);

            

            if (fail) {
                result.error = true;
                params.errorMessage = params.errorMessageInvalidType;
                return handleInput(params, result);
            } else {
                for (auto &invokerValue : params.returnInvokers) {
                    if (result.error = result.value == invokerValue) {
                        break;
                    }
                }
                if (!result.error) {
                    if (result.error = result.value < params.minInputValue || result.value > params.maxInputValue) {
                        params.errorMessage = params.errorMessageOutOfRange;
                        return handleInput(params, result);
                    }
                }
            }
            //     if (!result.error) {
            //         for (auto &invokerValue : params.returnInvokers) {
            //             if (result.error = (result.value == invokerValue)) {
            //                 result.inputText =
            //                     params.prevInput.empty() 
            //                         ? input::buildAsInput(params.inputPrompt, to_string(result.value)) 
            //                         : params.prevInput + "\n" + input::buildAsInput(params.inputPrompt, to_string(result.value));
            //                 return result;
            //             }
            //         }
            //         if (result.error = (result.value < params.minInputValue || result.value > params.maxInputValue)) {
            //             errorMesage = params.errorMessageOutOfRange;
            //             continue;
            //         }
            //     }
            //     errorMesage = params.errorMessageInvalidType;
            // } while (result.error);

            result.inputText =
                params.prevInput.empty() 
                    ? input::buildAsInput(params.inputPrompt, to_string(result.value)) 
                    : params.prevInput + "\n" + input::buildAsInput(params.inputPrompt, to_string(result.value));

            return result;
        }

        struct HandleStringInputParams {
            vector<string> instructions;
            vector<string> options;
            vector<string> returnInvokers;
            string inputPrompt = "Enter your choice: ";
            string errorMessage = "The value cannot be empty!";
            string prevInput;
            int optionsOffset = 0;
        };

        struct HandleStringInput {
            string value = "";
            bool error = false;
            string inputText = "";
        } defaultStringInput;

        HandleStringInput handleStringInput(HandleStringInputParams params, HandleStringInput result = defaultStringInput) {
            
            clear();

            cout << components::buildHUD() 
                << components::buildInstructions({
                    params.instructions,
                    params.errorMessage,
                    result.error,
                    false
                });

            if (params.options.size() > 0) {
                cout << components::buildOptions(params.options , params.optionsOffset);
            }

            cout << components::buildHeader('-');

            if (!params.prevInput.empty()) {
                cout << params.prevInput;
            }

            bool empty = input::getInput(params.inputPrompt, result.value);

            for (auto &invokerValue : params.returnInvokers) {
                if (result.error = result.value == invokerValue) {
                    break;
                }
            }

            if (result.error = ((!result.error) && empty)) {
                return handleStringInput(params, result);
            }
            
            result.inputText =
                params.prevInput.empty() 
                    ? input::buildAsInput(params.inputPrompt, result.value) 
                    : params.prevInput + "\n" + input::buildAsInput(params.inputPrompt, result.value);

            return result;
        }


        struct PostScreenParams {
            vector<string> title;
            vector<string> paragraphs;
            string prompt;
            vector<string> returnInvokers = {"0"};
        };

        int postScreen(PostScreenParams params) {
            clear();
            string value;
            size_t limit = program::options::lengthHUD - 10;

            components::InstructionParams titleParams;
            titleParams.instructions = params.title;
            titleParams.position = format::CENTER;
            titleParams.persistNewLine = false;
            titleParams.error = false;

            cout << components::buildHUD() 
                << components::buildInstructions(titleParams);

            for (auto &paragraph : params.paragraphs) {
                
                vector<string> lines = utils::format::splitWords(paragraph, limit);
                for (auto &line : lines) {
                    cout << utils::format::buildTextFormat("    " + line, {utils::format::LEFT, program::options::lengthHUD-2}) + "\n";
                }
            }

            cout << components::buildHeader('-');

            input::getInput(params.prompt, value);

            for (auto &invokerValue : params.returnInvokers) {
                if (value == invokerValue) {
                    return -1;
                }
            }

            return 1;
        }

        struct RowColumnParams {
            vector<string> instructions;
            string errorMessageInvalidType = "Invalid input! Please enter an integer";
            string errorMessageRowOutOfRange = "Number of rows must be between 1 and " + to_string(seatrs::data::totalRows) + ".";
            string errorMessageColumnOutOfRange = "Number of columns must be between 1 and " + to_string(seatrs::data::totalColumns) + ".";
            string inputPromptRow = "Enter Row number: ";
            string inputPromptColumn = "Enter Column number: ";
            int maxInputValueRow = seatrs::data::totalRows;
            int maxInputValueColumn = seatrs::data::totalColumns;
        };

        struct RowColumn {
            int row;
            int column;
            bool error;
        };

        RowColumn getRowColumn(RowColumnParams params) {
            RowColumn result;
            result.row = 0;
            result.column = 0;
            result.error = false;

            screenTemplate::HandleInputParams sharedParams, paramsRow, paramsColumn;

            sharedParams.instructions = params.instructions;
            sharedParams.returnInvokers = {0};
            sharedParams.errorMessageInvalidType = params.errorMessageInvalidType;
            sharedParams.persistNewLine = true;
            sharedParams.optionsOffset = 0;
            sharedParams.minInputValue = 1;

            paramsRow = paramsColumn = sharedParams;
            
            paramsRow.errorMessageOutOfRange = params.errorMessageRowOutOfRange;
            paramsRow.inputPrompt = params.inputPromptRow;
            paramsRow.maxInputValue = params.maxInputValueRow;
            
            paramsColumn.errorMessageOutOfRange = params.errorMessageColumnOutOfRange;
            paramsColumn.inputPrompt = params.inputPromptColumn;
            paramsColumn.maxInputValue = params.maxInputValueColumn;

            screenTemplate::HandleInput resultRow, resultColumn;

            do {
                resultRow = screenTemplate::handleInput(paramsRow);
                
                if (result.error = resultRow.error) {
                    break;
                }

                paramsColumn.prevInput = resultRow.inputText;

                resultColumn = screenTemplate::handleInput(paramsColumn);

                result.error = resultColumn.error;

            } while (result.error);

            if (!result.error) {
                result.row = resultRow.value;
                result.column = resultColumn.value;
            }

            return result;
        }
    }

    namespace screen {

        /** Return Value:
         *      1   -> Success, return to Main Menu
         *      -1  -> Return to Last Screen
         *      0   -> Finish
         * */ 


        using namespace display;

        int optionsSetDimensions() {
            screenTemplate::RowColumnParams params;
            params.instructions = {
                "[Options -> Edit Seat Layout Dimensions]",
                "Enter the number of rows and columns of the seat layout. - [0] Return"
            };

            params.errorMessageRowOutOfRange = "Number of rows must be " + to_string(program::options::maxPossibleRows) + " or less.";
            params.errorMessageColumnOutOfRange = "Number of columns must be " + to_string(program::options::maxPossibleColumns) + " or less.";

            params.inputPromptRow = "Enter the number of rows: ";
            params.inputPromptColumn = "Enter the number of columns: ";

            params.maxInputValueRow = program::options::maxPossibleRows;
            params.maxInputValueColumn = program::options::maxPossibleColumns;

            screenTemplate::RowColumn result = screenTemplate::getRowColumn(params);

            if (result.error) {
                return -1;
            }

            seatrs::setSize(result.row, result.column);
            return 1;
        }

        int options() {
            int status;

            screenTemplate::HandleInputParams params;
            params.instructions = {"[Options]", "Choose an option."};
            params.options = {
                "Return to Main Menu",
                "Edit Seat Layout Dimensions",
                "Exit",
            };

            do {
                screenTemplate::HandleInput result = screenTemplate::handleInput(params);

                switch (result.value) {
                    case 1: {
                        status = 1;
                        break;
                    }
                    case 2: {
                        status = optionsSetDimensions();
                        break;
                    }
                    case 0: {
                        status = 0;
                        break;
                    }
                }
            } while (status == -1);

            return status;
        }
        
        int createReservation() {
            int status = 0;

            screenTemplate::RowColumnParams params;
            params.instructions = {"[Create Seat Reservation]", "Enter the row and column of the seat. -> [0] Back"};
                
            screenTemplate::RowColumn result;

            screenTemplate::PostScreenParams postParams;
            postParams.title = {
                params.instructions[0],
                "",
                " -> [0] Create Reservation",
                " -> [Enter] Return to Main Menu",
                ""
            };
            

            screenTemplate::HandleStringInputParams sharedParams, nameParams, descriptionParams;
            sharedParams.instructions = {
                params.instructions[0],
                "Enter the the reservation information for the seat."
            };
            sharedParams.returnInvokers = {"0"};
            nameParams = descriptionParams = sharedParams;
            
            nameParams.inputPrompt = "Enter Name: ";
            descriptionParams.inputPrompt = "Enter Description: ";

            screenTemplate::HandleStringInput resultName, resultDescription;

            do {
                result = screenTemplate::getRowColumn(params);

                if (result.error) {
                    status = 1;
                    return status;
                }

                int rowIndex = result.row - 1;
                int columnIndex = result.column - 1;

                if (seatrs::isValidSeat(rowIndex, columnIndex)) {
                    if (seatrs::data::seats[rowIndex][columnIndex].isReserved) {
                        screenTemplate::PostScreenParams newPostParams = postParams;
                        newPostParams.paragraphs = {" ## The seat [" + to_string(rowIndex + 1) + "][" + to_string(columnIndex + 1) + "] is already reserved! Please pick another seat."};
                        status = screenTemplate::postScreen(newPostParams);
                        continue;
                    }
                } else {
                    screenTemplate::PostScreenParams newPostParams = postParams;
                    newPostParams.paragraphs = {" ## The seat [" + to_string(rowIndex + 1) + "][" + to_string(columnIndex + 1) + "] is invalid! Please pick another seat within the range of the seat layout ."};
                    status = screenTemplate::postScreen(postParams);
                    continue;
                }

                bool error = false;

                do {
                    resultName = screenTemplate::handleStringInput(nameParams);
                    
                    if (error = resultName.error) {
                        break;
                    }

                    descriptionParams.prevInput = resultName.inputText;

                    resultDescription = screenTemplate::handleStringInput(descriptionParams);

                    error = resultDescription.error;

                } while (error);

                if (error) {
                    status = -1;
                } else {
                    status = 1;
                    seatrs::Seat &seat = seatrs::data::seats[result.row - 1][result.column - 1];
                    seat.name = resultName.value;
                    seat.description = resultDescription.value;
                    seat.isReserved = true;

                    screenTemplate::PostScreenParams successPostParams = postParams;
                    postParams.paragraphs.clear();
                    successPostParams.title = {
                        postParams.title[0], 
                        "Reservation created successfully for " + resultName.value + " at seat [" + to_string(result.row) + "][" + to_string(result.column) + "].",
                        "",
                        " -> [0] Create Another Reservation",
                        " -> [Enter] Return to Main Menu"
                    };
                    status = screenTemplate::postScreen(successPostParams);
                }

            } while (status == -1);

            return status;
        }

        int readReservation() {
            int status = 0;

            return status;
        }

        void mainMenu() {
            int status = 1;

            screenTemplate::HandleInputParams params;
            params.instructions = {"[Main Menu]", "Choose an option."};
            params.options = {
                "Display Seat Layout",
                "Create Seat Reservation",
                "Read/Display Seat Reservation",
                "Update Seat Reservation",
                "Delete/Cancel Seat Reservation",
                "Options (-> Exit)"
            };

            do {
                screenTemplate::HandleInput result = screenTemplate::handleInput(params);

                switch (result.value) {
                    case 2: {
                        status = createReservation();
                        break;
                    }
                    case 0: {
                        status = options();
                        break;
                    }
                    
                }
            } while (status != 0);
        }
        
    }
}


int main() {
    seatrs::setSize();
    display::screen::mainMenu();
    return 0;
}