# SNHU CS 210 Project One — Chada Tech Clocks

**Student:** Victor Israel  
**Language:** C++17  
**Source files:** Exactly **one** — `main.cpp`.

This submission combines all required clock logic into `main.cpp`, keeping each task in a clearly named function. The single application starts from a validated user-entered time, displays synchronized 12-hour and 24-hour clocks side by side, and allows additions of one hour, minute or second until the user chooses Exit.

**Instructor-feedback improvement:** In an interactive console, the two clocks and menu now **redraw in-place** after each selection instead of continually scrolling and stacking duplicate clock displays. Windows uses native console cursor/clear APIs and macOS/Linux use ANSI terminal control codes. Redirected input/output continues to work with plain-text automated tests.

## Open in Visual Studio 2022

1. Extract the complete ZIP to a normal folder.
2. Open `ChadaTechClocks.sln` in Visual Studio 2022 with **Desktop development with C++** installed.
3. Choose **Debug** and **x64**.
4. Press **Ctrl+F5** to compile and run.

**If Visual Studio reports it cannot find `x64\\Debug\\ChadaTechClocks.exe`:** This means the executable is missing, usually because the project has not built successfully. Use **Build > Rebuild Solution** (Ctrl+Shift+B), then inspect **View > Output > Show output from: Build**. The output should report **1 succeeded, 0 failed** before you try Ctrl+F5. The project chooses `v143` for Visual Studio 2022 and `v145` for Visual Studio 2026, with an explicit output directory so the compiler and debugger agree on the `.exe` location. See `VISUAL_STUDIO_TROUBLESHOOTING.md` for detailed steps.

Alternatively, simply add `main.cpp` to a C++ Console App project in Visual Studio. Make sure there is only one active `main()`.

## Compile and run from a terminal

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp -o ChadaTechClocks
./ChadaTechClocks
```

For Windows PowerShell after compiling with a Windows compiler, run `./ChadaTechClocks.exe` or `.\ChadaTechClocks.exe`.

## Functions included in `main.cpp`

| Function | Responsibility |
| --- | --- |
| `twoDigitString(unsigned int n)` | Format a one-digit number with a leading zero |
| `nCharString(size_t n, char c)` | Repeat characters for clock/menu borders and spaces |
| `formatTime24(h,m,s)` | Format a 24-hour `HH:MM:SS` string |
| `formatTime12(h,m,s)` | Format a 12-hour `HH:MM:SS A M/P M` string as required by SENSE |
| `printMenu(char*[],unsigned int,unsigned char)` | Print the variable-width four-option menu |
| `getMenuChoice(unsigned int)` | Read and validate a menu choice |
| `displayClocks(h,m,s)` | Show both clocks side by side |
| `mainMenu()` | Show the menu, dispatch actions and exit on choice 4 |
| `prepareClockScreen()` (internal helper) | Clear the interactive display and return the cursor to the beginning before refreshing the clocks |
| `addOneSecond()` | Add a second, with automatic carry |
| `addOneMinute()` | Add a minute, with automatic carry |
| `addOneHour()` | Add an hour, wrapping 23 to 00 |
| `getHour`, `getMinute`, `getSecond` | Read the underlying simulated time |
| `setHour`, `setMinute`, `setSecond` | Modify the underlying simulated time |
| `main()` | Initialize the clock and launch the menu loop |

All functions use the required SENSE practice function signatures; the clock uses one shared time state so the two displays cannot drift apart. The application is a **simulation**, not a real-time clock. The menu intentionally separates items with blank lines as specified by the detailed SENSE practice instructions.

**SENSE formatting detail:** The grader expects a separating space between the meridiem letters: `07:24:09 A M` and `11:00:59 P M` (not conventional `AM` or `PM`).

**Important:** This combined `main.cpp` is for the complete Visual Studio project. Do not submit the complete file to an individual SENSE function test. Those graders provide their own helper definitions, so submitting the full program causes redefinition errors. Use the separate `CS210_SENSE_Individual_Functions.zip` package and paste only the matching function for each SENSE test.

## Test results

See `TEST_REPORT.md`. The file compiled with GNU C++17 and strict warnings-as-errors, passed 43 independent checks, and passed nine end-to-end scenarios. You can rerun the real-input scenarios with `python3 tests/test_end_to_end.py` (requires `g++`). The Visual Studio solution structure was checked, but an actual MSVC compilation was not performed in this environment.

On macOS/Linux, run `python3 tests/test_in_place_redraw.py` to verify the in-place refresh behavior in a simulated interactive terminal. On Windows, open the Visual Studio project and confirm that one clock/menu screen stays visible when you choose options 1–3.
