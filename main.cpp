/*
 * Student: Victor Israel
 * Course: CS 210 - Project One
 * Project: Chada Tech Clocks
 * File: main.cpp
 *
 * Purpose: Display synchronized 12-hour and 24-hour clocks and allow the user
 *          to add one hour, minute, or second through a menu.
 *
 * All functions and the application's main() are in this single C++17 file.
 * The clock is a simulation: it changes only when the user selects an option.
 */

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

// Windows consoles use native cursor control; other terminals use ANSI codes.
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Exact function signatures from the CS 210/SENSE practice requirements.
std::string twoDigitString(unsigned int n);
std::string nCharString(std::size_t n, char c);
std::string formatTime24(unsigned int h, unsigned int m, unsigned int s);
std::string formatTime12(unsigned int h, unsigned int m, unsigned int s);
void printMenu(char* strings[], unsigned int numStrings, unsigned char width);
unsigned int getMenuChoice(unsigned int maxChoice);
void displayClocks(unsigned int h, unsigned int m, unsigned int s);
void mainMenu();
void addOneSecond();
void addOneMinute();
void addOneHour();

// The required parameter-free add functions read and update one shared clock.
unsigned int getHour();
unsigned int getMinute();
unsigned int getSecond();
void setHour(unsigned int hour);
void setMinute(unsigned int minute);
void setSecond(unsigned int second);

namespace {

struct ClockState {
    unsigned int hour = 0;
    unsigned int minute = 0;
    unsigned int second = 0;
};

ClockState currentTime;

// Clear the current console screen and return the cursor to its upper-left
// corner. Each menu selection refreshes the same screen instead of pushing
// another copy of the clocks into the terminal's scrolling history.
void prepareClockScreen() {
#ifdef _WIN32
    const HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (screen != INVALID_HANDLE_VALUE && screen != nullptr &&
        GetConsoleScreenBufferInfo(screen, &info)) {
        const COORD topLeft = {0, 0};
        const DWORD cells = static_cast<DWORD>(info.dwSize.X) *
                            static_cast<DWORD>(info.dwSize.Y);
        DWORD written = 0;

        // Native Windows APIs work in Visual Studio even when ANSI escapes
        // are unavailable in the console configuration.
        FillConsoleOutputCharacterA(screen, ' ', cells, topLeft, &written);
        FillConsoleOutputAttribute(screen, info.wAttributes, cells,
                                   topLeft, &written);
        SetConsoleCursorPosition(screen, topLeft);
    }
#else
    // In non-interactive runs, preserve plain text output for automated tests.
    if (isatty(STDOUT_FILENO)) {
        std::cout << "\x1b[2J\x1b[H";
    }
#endif
}

// Reject incomplete input such as "3x", decimals, negative values, and overflow.
bool parseNumber(const std::string& line, unsigned int minValue,
                 unsigned int maxValue, unsigned int& result) {
    std::istringstream input(line);
    long long number = -1;
    if (!(input >> number)) {
        return false;
    }
    input >> std::ws;
    if (!input.eof() || number < minValue || number > maxValue) {
        return false;
    }
    result = static_cast<unsigned int>(number);
    return true;
}

// Read the initial time safely; unlike the menu, a helpful error is displayed.
bool readStartingPart(const std::string& prompt, unsigned int maximum,
                      unsigned int& value) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return false;
        }
        if (parseNumber(line, 0, maximum, value)) {
            return true;
        }
        std::cout << "Invalid time. Enter a whole number from 0 to "
                  << maximum << ".\n";
    }
}

bool readStartingTime() {
    unsigned int hour = 0;
    unsigned int minute = 0;
    unsigned int second = 0;

    std::cout << "Welcome to Chada Tech Clocks!\n"
              << "Enter the starting time in 24-hour format.\n\n";
    if (!readStartingPart("Hour (0-23): ", 23, hour) ||
        !readStartingPart("Minute (0-59): ", 59, minute) ||
        !readStartingPart("Second (0-59): ", 59, second)) {
        return false;
    }

    setHour(hour);
    setMinute(minute);
    setSecond(second);
    std::cout << '\n';
    return true;
}

}  // namespace

// 1. Format a number with two digits (e.g., 3 -> "03", 12 -> "12").
std::string twoDigitString(unsigned int n) {
    return (n < 10 ? "0" : "") + std::to_string(n);
}

// 2. Repeat a character n times (e.g., nCharString(5, '*') -> "*****").
std::string nCharString(std::size_t n, char c) {
    // Return a named string to satisfy the SENSE style requirement.
    std::string repeated(n, c);
    return repeated;
}

// 3. Format hours, minutes, and seconds in the 24-hour standard.
std::string formatTime24(unsigned int h, unsigned int m, unsigned int s) {
    return twoDigitString(h) + ":" + twoDigitString(m) + ":" +
           twoDigitString(s);
}

// 4. Convert 0 to midnight, 12 to noon, and 13-23 to PM hours.
std::string formatTime12(unsigned int h, unsigned int m, unsigned int s) {
    const unsigned int hour12 = (h % 12 == 0) ? 12 : h % 12;
    // SENSE's expected strings use the separated letters "A M" and "P M".
    const std::string period = (h < 12) ? " A M" : " P M";
    return twoDigitString(hour12) + ":" + twoDigitString(m) + ":" +
           twoDigitString(s) + period;
}

// 5. Print the menu using the exact variable-width exercise signature.
void printMenu(char* strings[], unsigned int numStrings, unsigned char width) {
    const std::size_t lineWidth = width;
    std::cout << nCharString(lineWidth, '*') << '\n';

    for (unsigned int index = 0; index < numStrings; ++index) {
        const std::string item = "* " + std::to_string(index + 1) + " - " +
                                 strings[index];
        // Reserve the final column for '*'; align each menu row to width.
        const std::size_t spaces = (lineWidth > item.size() + 1)
                                       ? lineWidth - item.size() - 1
                                       : 0;
        std::cout << item << nCharString(spaces, ' ') << "*\n";

        // The supplied exercise requires a blank line between choices.
        if (index + 1 < numStrings) {
            std::cout << '\n';
        }
    }

    std::cout << nCharString(lineWidth, '*') << '\n';
}

// 6. Re-prompt until the user enters a choice in [1, maxChoice].
// SENSE's isolated exercise expects no output inside this function.
unsigned int getMenuChoice(unsigned int maxChoice) {
    unsigned int result = 0;
    std::string line;
    while (true) {
        if (!std::getline(std::cin, line)) {
            return 0;  // EOF is handled safely by mainMenu().
        }
        if (parseNumber(line, 1, maxChoice, result)) {
            return result;
        }
        // No error text: the exercise says the caller handles menu errors.
    }
}

// 7. Display both clocks side-by-side following the supplied layout.
void displayClocks(unsigned int h, unsigned int m, unsigned int s) {
    std::cout << nCharString(27, '*') << nCharString(3, ' ')
              << nCharString(27, '*') << '\n';
    std::cout << '*' << nCharString(6, ' ') << "12-HOUR CLOCK"
              << nCharString(6, ' ') << '*' << nCharString(3, ' ');
    std::cout << '*' << nCharString(6, ' ') << "24-HOUR CLOCK"
              << nCharString(6, ' ') << "*\n\n";
    std::cout << '*' << nCharString(6, ' ') << formatTime12(h, m, s)
              << nCharString(7, ' ') << '*' << nCharString(3, ' ');
    std::cout << '*' << nCharString(8, ' ') << formatTime24(h, m, s)
              << nCharString(9, ' ') << "*\n";
    std::cout << nCharString(27, '*') << nCharString(3, ' ')
              << nCharString(27, '*') << '\n';
}

// Provide the accessors assumed to exist in the individual SENSE exercises.
unsigned int getHour() { return currentTime.hour; }
unsigned int getMinute() { return currentTime.minute; }
unsigned int getSecond() { return currentTime.second; }
void setHour(unsigned int hour) { currentTime.hour = hour; }
void setMinute(unsigned int minute) { currentTime.minute = minute; }
void setSecond(unsigned int second) { currentTime.second = second; }

// 11. Add one hour; 23:xx:xx becomes 00:xx:xx.
void addOneHour() {
    setHour((getHour() == 23) ? 0 : getHour() + 1);
}

// 10. Add one minute, carrying into the next hour at minute 59.
void addOneMinute() {
    if (getMinute() == 59) {
        setMinute(0);
        addOneHour();
    } else {
        setMinute(getMinute() + 1);
    }
}

// 9. Add one second, carrying into the next minute at second 59.
void addOneSecond() {
    if (getSecond() == 59) {
        setSecond(0);
        addOneMinute();
    } else {
        setSecond(getSecond() + 1);
    }
}

// 8. Refresh the same screen, show the menu, and dispatch user selections.
void mainMenu() {
    char hourText[] = "Add One Hour";
    char minuteText[] = "Add One Minute";
    char secondText[] = "Add One Second";
    char exitText[] = "Exit Program";
    char* options[] = {hourText, minuteText, secondText, exitText};
    unsigned int choice = 0;

    while (true) {
        // Instructor feedback: redraw in-place after each change. Clearing
        // before painting prevents a growing stack of old clock displays.
        prepareClockScreen();
        displayClocks(getHour(), getMinute(), getSecond());
        std::cout << '\n';
        printMenu(options, 4, 26);

        // This calls getMenuChoice(4) exactly once per menu interaction.
        std::cout << "Enter your choice (1-4): ";
        choice = getMenuChoice(4);
        std::cout << '\n';
        switch (choice) {
            case 1:
                addOneHour();
                break;
            case 2:
                addOneMinute();
                break;
            case 3:
                addOneSecond();
                break;
            case 4:
                std::cout << "Thank you for using Chada Tech Clocks!\n";
                return;
            case 0:
                std::cout << "\nInput ended. Program closed.\n";
                return;
            default:
                // All other choices are rejected by getMenuChoice().
                continue;
        }
    }
}

// Keep main() short; the program's work is organized into functions above.
#ifndef CHADA_CLOCK_TESTING
int main() {
    if (!readStartingTime()) {
        std::cout << "\nInput ended. Program closed.\n";
        return 0;
    }

    mainMenu();
    return 0;
}
#endif
