// Geevoi A. Plaza
// CS-1105
// CS111 - Computer Programming
// LABORATORY ACTIVITY 3

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct SrsSeat {
    string name;
    string description;
    bool isAvailable = true;
};

namespace options {
    bool visualizeSeats = true;
    int maxPossibleRows = 100;
    int maxPossibleColumns = 100;
    int lengthHUD = 80;
}

// begin UTILS

namespace srsData {
    int totalOccupiedSeats = 0;
    vector<vector<SrsSeat>> seats;

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

namespace utils {
    int defaultSplitRatio[2] = { 1, 1 };
    string defaultEmptyArray[1];
    const int minimumLengthHUD = 60;
}

// end UTILS

// begin BETTER PRINT
namespace format {
    enum PrintPosition { LEFT, CENTER, RIGHT };

    string stringRepeat(int num, string text) {
        for (int i = 0; i < num; i++) {
            cout << text;
        }
        return "";
    }

    int* splitLength(int length = options::lengthHUD, int ratio[2] = utils::defaultSplitRatio) {

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

    string print(const string text, PrintPosition pos = LEFT, int length = -1, string space = " ", int explicitTextLength = 0) {
        int spaceLengthLeft, spaceLengthRight;

        int textLength;


        if (explicitTextLength > 0) {
            textLength = explicitTextLength;
        }
        else {
            textLength = text.size();
        }

        if (length < 0) {
            length = options::lengthHUD;
        }

        switch (pos) {
        case LEFT: {
            spaceLengthLeft = 0;
            spaceLengthRight = length - textLength;
            break;
        };
        case CENTER: {
            spaceLengthLeft = (length - textLength) / 2;
            if ((length - textLength) % 2 == 0) {
                spaceLengthRight = spaceLengthLeft;
            }
            else {
                spaceLengthRight = spaceLengthLeft + 1;
            }
            break;
        };
        case RIGHT: {
            spaceLengthLeft = length - textLength;
            spaceLengthRight = 0;
            break;
        };
        }

        cout << stringRepeat(spaceLengthLeft, space) << text << stringRepeat(spaceLengthRight, space);

        // Return empty string to work with stream
        return "";
    }
}
// end BETTER PRINT

// begin HUD MANAGER

namespace display {
    
    void clear() {
        #if defined(__LINUX__) || defined(__APPLE__) || defined(__gnu_linux__) || defined(__linux__)
            cout << "\x1b[2J\x1b[H";
        #else
            cout << string(100, '\n');
        #endif
        return;
    }

    string displayNameArt(int length = options::lengthHUD) {

        string lines[] ={
            "░██████╗░░█████╗░██████╗░░░░░░░░░░██████╗██████╗░░██████╗",
            "██╔════╝░██╔══██╗██╔══██╗░░░░░░░░██╔════╝██╔══██╗██╔════╝",
            "██║░░██╗░███████║██████╔╝░█████╗░╚█████╗░██████╔╝░╚████╗░",
            "██║░░╚██╗██╔══██║██╔═══╝░░╚════╝░░╚═══██╗██╔══██╗░╚═══██╗",
            "╚██████╔╝██║░░██║██║░░░░░░░░░░░░░██████╔╝██║░░██║██████╔╝",
            "░╚═════╝░╚═╝░░╚═╝╚═╝░░░░░░░░░░░░░╚═════╝░╚═╝░░╚═╝╚═════╝░" 
        };

        for (int i = 0; i < sizeof(lines)/sizeof(*lines); i++) {
            cout << format::print(lines[i], format::CENTER, length, "░", 57) << endl;
        }

        return "";
    }

    string buildHeader(char lineChar = '=', int length = options::lengthHUD, bool newLine = true) {
        return (newLine ? string(length, lineChar) + '\n' : string(length, lineChar));
    }

    void displayTitle() {
        using namespace options;

        cout << displayNameArt()
            << buildHeader()
            << format::print("Geevoi A. Plaza's", format::CENTER, lengthHUD - 20) << "|" << format::print("CS-1105", format::RIGHT, 19) << endl
            << format::print("Seat Reservation System", format::CENTER, lengthHUD - 20) << "|" << format::print("CS 111", format::RIGHT, 19) << endl
            << buildHeader();
    }

    void displayHUD() {

        display::displayTitle();

        int* lengthRatio = format::splitLength();


        cout << format::print((to_string(srsData::totalRows) + " Rows x " + to_string(srsData::totalColumns) + " Cols"), format::LEFT, lengthRatio[0])
            << format::print((to_string(srsData::totalOccupiedSeats) + "/" + to_string(srsData::totalRows * srsData::totalColumns) + " seats occupied"), format::RIGHT, lengthRatio[1]) << endl
            << buildHeader('-');
    }

    void displayInstructions(vector<string> instructions, string errorMessage = "", bool fail = false, bool newLine = true) {
        
        for (auto instruction : instructions) {
            cout << format::print(instruction, format::CENTER) << endl;
        }

        if (fail) {
            cout << format::print("## " + errorMessage + " ##", format::CENTER) << endl;
        } else if (newLine) {
            cout << endl;
        }
    }

    void displayOptions(vector<string> options, int startOffset = 1) {
        int num;
        int optionsSize = options.size();

        for (int i = (startOffset == 0); i < optionsSize; i++) {
            if (startOffset == 0 && i == optionsSize - 1) {
                num = 0;
            } else {
                num = startOffset + i;
            };

            cout << format::print(("        [" + to_string(num) + "] " + options[i] + " ")) << endl;
        }
    }
}

// end HUD MANAGER


// begin BETTER INPUT

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
// end BETTER INPUT

namespace dimension {

    struct DimensionInfo {
        int dimensionLimit;
        string inputPrompt;
        string errorMessageWhenExceeded;
    };

    struct Info {
        vector<string> instructions;
        string errorMessageDefault = "Invalid input! Please enter an integer";
        bool error = false;
        string prevInput;
    } info;
    
    struct DimensionData {
        Info info;
        DimensionInfo dimensionInfo;
        struct Values {
            int dimensionResult = 0;
        } values;
    };

    void getDimension(DimensionData& data) {
        string errorMessage;
        data.values.dimensionResult = 0;
        data.info.error = false;

        do {
            display::clear();
            display::displayHUD();

            display::displayInstructions(data.info.instructions, errorMessage, data.info.error);
            cout << display::buildHeader('-');

            if (!data.info.prevInput.empty()) {
                cout << data.info.prevInput;
            }
            data.info.error = input::getInput(data.dimensionInfo.inputPrompt, data.values.dimensionResult);
            if (!data.info.error) {
                if (data.info.error = data.values.dimensionResult > data.dimensionInfo.dimensionLimit) {
                    errorMessage = data.dimensionInfo.errorMessageWhenExceeded;
                    continue;
                } else if (data.info.error = data.values.dimensionResult <= 0) {
                    data.info.prevInput.clear();
                    return;
                }
            }
            errorMessage = data.info.errorMessageDefault;
        } while (data.info.error);

        data.info.prevInput = 
            data.info.prevInput.empty() 
                ? input::buildAsInput(data.dimensionInfo.inputPrompt, to_string(data.values.dimensionResult)) 
                : data.info.prevInput + "\n" + input::buildAsInput(
                    data.dimensionInfo.inputPrompt, to_string(data.values.dimensionResult)
                );
        return;
    }

    struct RowColumnData {
        Info info;
        DimensionInfo rowInfo = {
            srsData::totalRows,
            "Enter row number: ",
            "Row number must be between 1 and " + to_string(srsData::totalRows) + "."
        }; 
        DimensionInfo columnInfo = {
            srsData::totalColumns,
            "Enter column number: ",
            "Column number must be between 1 and " + to_string(srsData::totalColumns) + "."
        };
        struct Values {
            int row = 0;
            int column = 0;
        } values;
    };

    void getRowColumn(RowColumnData& data) {
        DimensionData dimensionData;
        data.values.row = 0;
        data.values.column = 0;
;
        dimensionData.info = data.info;

        do {
            dimensionData.dimensionInfo = data.rowInfo;

            getDimension(dimensionData);
            data.values.row = dimensionData.values.dimensionResult;
            if (data.info.error = dimensionData.info.error) {
                return;
            }

            dimensionData.dimensionInfo = data.columnInfo;

            getDimension(dimensionData);
            data.values.column = dimensionData.values.dimensionResult;
            
        } while (dimensionData.info.error);

        data.info.error = dimensionData.info.error;
    }
}

namespace screen {
    void mainMenu() {
        vector<string> options = {
            "Options (& Exit)"
        };
        vector<string> instructions = {"Main Menu"};
        string defaultErrorMessage = "Invalid input! Please enter an integer";
        string errorMessage;
        bool error = false;
        int choice;

        do {
            display::clear();
            display::displayHUD();

            display::displayInstructions(instructions, errorMessage, error, false);
            // cout << format::print(display::buildHeader('-', options::lengthHUD/2, false), format::CENTER) << endl;
            display::displayOptions(options, 0);
            cout << display::buildHeader('-');
            
            error = input::getInput("Enter your choice: ", choice);
            if (!error) {
                if (error = choice < 0 || choice >= options.size()) {
                    errorMessage = "Please enter a valid option.";
                    continue;
                } 
            }
            errorMessage = defaultErrorMessage;
        } while (error);
    }

    void setDimensions() {
        dimension::RowColumnData data = {
            {
                {
                    "Options -> Edit Seat Layout Dimensions",
                    "Enter the number of rows and columns of the seat layout. - [0] Back"
                }, // instructions
                "Invalid input! Please enter an integer" // error message
            }, // info
            {
                options::maxPossibleRows, // dimension limit
                "Enter the number of rows: ", // input prompt
                "Number of rows must be " + to_string(options::maxPossibleRows) + " or less." // error message when exceeded
            }, // row
            {
                options::maxPossibleColumns, // dimension limit
                "Enter the number of columns: ", // input prompt
                "Number of columns must be " + to_string(options::maxPossibleColumns) + " or less." // error message when exceeded
            } // column
        };

        dimension::getRowColumn(data);

        if (!data.info.error) {
            srsData::setSize(data.values.row, data.values.column);
        }
    }
}

int main() {
    srsData::setSize();
    while (true) {
        display::clear();
        screen::setDimensions();
    }
}