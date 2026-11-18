# Verified Test Results — CS 210 Project One

**Source submitted:** `main.cpp` (one C++ file)  
**Compiler:** GNU g++ using `-std=c++17 -Wall -Wextra -Wpedantic -Werror`  
**Compilation:** Successful, with no errors or warnings.

## Function-level tests

**43 of 43 checks passed**, covering:

- `twoDigitString()` padding and `nCharString()` character repetition.
- 12-hour and 24-hour formatting, including noon and midnight.
- Adding hours, minutes, and seconds with correct carry and wraparound.
- Side-by-side clock headings and time display.
- Both supported menu widths (26 and 57 characters) and menu spacing.
- Rejection of invalid input, acceptance of valid input, and graceful EOF handling.
- Main menu sequence `1, 2, 2, 3, 3, 3, 4` (one hour, two minutes, three seconds).
- Interactive console refreshes the same clock/menu screen in-place after selections, following the instructor's improvement feedback.

## Complete application scenarios

**9 of 9 scenarios passed:**

1. Add one second to a normal time (15:22:01 → 15:22:02).
2. Second rollover across midnight (23:59:59 → 00:00:00).
3. Second rollover across noon (11:59:59 → 12:00:00 P M).
4. Minute rollover across midnight (23:59:30 → 00:00:30).
5. Hour rollover across midnight (23:30:10 → 00:30:10).
6. Invalid starting values and menu selections rejected and retried.
7. Required menu sequence `1, 2, 2, 3, 3, 3, 4` applies exactly the requested adjustments.
8. Selecting option 4 exits without another clock update.
9. End-of-file input exits cleanly.

The number of clock and menu render calls was checked in each end-to-end scenario to ensure the program refreshes both panels together after each valid adjustment. The interactive-terminal test separately checks that screen-clear/cursor-home sequences prevent old panels from stacking; redirected automated tests intentionally preserve plain-text output.

## Instructor-feedback improvement: in-place display

**Interactive pseudo-terminal test passed (1 of 1).** The test ran the actual application with terminal output and verified that two successive frames use screen-clear/cursor-home control, the second frame shows `00:00:00` and `12:00:00 A M` after crossing midnight, and the final visible frame contains only one 12-hour heading and one 24-hour heading. This addresses the feedback about repeated, scrolling clock copies.

On Windows, the program uses Win32 console functions to clear and return to the beginning of the same display; on macOS/Linux, it uses ANSI terminal codes. A native Visual Studio/MSVC run was not available in this test environment.

**Note:** GNU g++ was available for compilation. MSVC/Visual Studio was not available for a native Windows build, so Windows compilation is not independently claimed.

## Visual Studio startup error compatibility check

After the user reported that Visual Studio could not locate `x64\\Debug\\ChadaTechClocks.exe`, the `.vcxproj` was updated to select `v143` for Visual Studio 2022 and `v145` for Visual Studio 2026, and to write its executable to the exact `$(ProjectDir)$(Platform)\\$(Configuration)\\` output folder expected by the debugger. The revised project XML parsed successfully. All 43 code checks, 9 application scenarios, and the interactive redraw test still passed afterward.

The missing EXE message can also result from a local build failure or an uninstalled C++ workload. The detailed user-facing diagnosis and recovery steps are in `VISUAL_STUDIO_TROUBLESHOOTING.md`. The user's specific cause cannot be confirmed without Visual Studio's **Build** output.

## SENSE grader compatibility

The revised 12-hour formatter produces `A M` and `P M` with the exact spacing demonstrated by SENSE's `formatTime12` failed test output. The isolated get-menu-choice exercise also uses an unsigned numeric input and only one conditional, and the isolated main-menu exercise declares its choice before the loop.

This complete Visual Studio project compiles successfully on its own. It must **not** be pasted into the individual SENSE function exercises: SENSE supplies helper function definitions, and combining them with this whole program produces duplicate-definition compiler errors. Those exercises require individual function submissions from the separate SENSE-specific package. Local simulation of all 11 isolated exercise tests passed, but official SENSE grading must be performed within each exercise.
