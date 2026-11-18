"""Run nine real-input scenarios against the single-file Chada Tech clock.

Usage: python3 tests/test_end_to_end.py
Requires a C++17 compiler named g++ on PATH.
"""

from pathlib import Path
import subprocess
import tempfile


SOURCE = Path(__file__).resolve().parents[1] / "main.cpp"


def check_case(binary, name, inputs, required, displays, bad_times=0):
    run = subprocess.run(
        [str(binary)], input=inputs, text=True, capture_output=True, timeout=5
    )
    output = run.stdout
    if run.returncode or run.stderr:
        raise AssertionError(f"{name}: process failed: {run.stderr}")
    for fragment in required:
        if fragment not in output:
            raise AssertionError(f"{name}: expected {fragment!r} in:\n{output}")
    actual_displays = output.count("12-HOUR CLOCK")
    if actual_displays != displays or output.count("24-HOUR CLOCK") != displays:
        raise AssertionError(f"{name}: wrong number of clock refreshes")
    if output.count("Invalid time.") != bad_times:
        raise AssertionError(f"{name}: wrong initial input error count")
    print(f"PASS: {name}")


def main():
    scenarios = [
        ("add second", "15\n22\n1\n3\n4\n", ["15:22:02", "03:22:02 P M"], 2, 0),
        ("midnight second rollover", "23\n59\n59\n3\n4\n",
         ["00:00:00", "12:00:00 A M"], 2, 0),
        ("noon second rollover", "11\n59\n59\n3\n4\n",
         ["12:00:00", "12:00:00 P M"], 2, 0),
        ("midnight minute rollover", "23\n59\n30\n2\n4\n",
         ["00:00:30", "12:00:30 A M"], 2, 0),
        ("midnight hour rollover", "23\n30\n10\n1\n4\n",
         ["00:30:10", "12:30:10 A M"], 2, 0),
        ("invalid values and menu input", "24\n5\n60\n6\nabc\n7\nbad\n0\n5\n2.5\n3\n4\n",
         ["05:06:08", "05:06:08 A M", "Thank you"], 2, 3),
        ("full menu sequence", "5\n10\n20\n1\n2\n2\n3\n3\n3\n4\n",
         ["06:12:23", "06:12:23 A M"], 7, 0),
        ("exit without adjustment", "5\n10\n20\n4\n",
         ["05:10:20", "Thank you"], 1, 0),
        ("end of input", "5\n10\n20\n", ["05:10:20", "Input ended"], 1, 0),
    ]

    with tempfile.TemporaryDirectory(prefix="chada-clock-e2e-") as directory:
        executable = Path(directory) / "ChadaTechClocks"
        subprocess.run(
            ["g++", "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
             str(SOURCE), "-o", str(executable)],
            check=True, timeout=15,
        )
        for name, inputs, required, displays, bad_times in scenarios:
            check_case(executable, name, inputs, required, displays, bad_times)

    print("SUCCESS: 9/9 complete-program scenarios passed.")


if __name__ == "__main__":
    main()
