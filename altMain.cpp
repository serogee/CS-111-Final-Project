// Geevoi A. Plaza
// CS-1105
// CS111 - Computer Programming

#include <iostream>
#include <limits>
#include <string>
#include <vector>

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

        void setSize(int rows = 10, int columns = 10) {
            totalRows = rows;
            totalColumns = columns; 
            seats.resize(rows);
            for (auto &row : seats) {
                row.resize(columns);
            }
        }
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
            for (int i = 0; i < num; i++) {
                cout << text;
            }
            return "";
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
                    spaceLengthLeft = (options.length - textLength) / 2;
                    spaceLengthRight = options.length - spaceLengthLeft - textLength;
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

            return stringRepeat(spaceLengthLeft, options.space) + text + stringRepeat(spaceLengthRight, options.space);
        }

        string print(const string text, PrintOptions options = defaultPrintOptions) {
            cout << buildTextFormat(text, options);
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
            return false;
        }

        string buildAsInput(const string prompt, string value = "") {
            return " >> " + prompt + " " + value + "\n";
        }
        
    }

}

namespace program {

    namespace options {
        int maxPossibleRows = 100;
        int maxPossibleColumns = 100;
        int lengthHUD = 80;
    }

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
                output += (format::print(lines[i], {format::CENTER, length, "░", 57}) + "\n");
            }

            return output;
        }

        string buildTitle() {

            format::PrintOptions formatCenter = {format::CENTER, options::lengthHUD - 20};
            format::PrintOptions formatRight = {format::RIGHT, 19};

            string output = buildNameArt()
                + buildHeader()
                + format::print("Geevoi A. Plaza's", formatCenter) + "|" + format::print("CS-1105", formatRight) + "\n"
                + format::print("Seat Reservation System", formatCenter) + "|" + format::print("CS 111", formatRight) + "\n"
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
            bool newLine = true;
        } instructionParams;

        string buildInstructions(InstructionParams params) {
            string output;

            for (auto &instruction : params.instructions) {
                output += (format::print(instruction, {format::CENTER}) + "\n");
            }

            if (params.error) {
                output +=(format::print("## " + params.errorMessage + " ##", {format::CENTER}) + "\n");
            } else if (params.newLine) {
                output += "\n";
            }

            return output;
        }

        string buildOptions(vector<string> options, int offset = 0) {
            string output = "";
            int num;
            int optionsSize = options.size();

            for (int i = (offset == 0); i < optionsSize; i++) {
                if (offset == 0 && i == optionsSize - 1) {
                    num = 0;
                } else {
                    num = offset + i;
                };

                output += (format::print(("        [" + to_string(num) + "] " + options[i] + " ")) + "\n");
            }
            return output;
        }
}

namespace display {
    namespace screenTemplate {

    }
}