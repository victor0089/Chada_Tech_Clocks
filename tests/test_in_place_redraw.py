"""Verify that the clock redraws instead of stacking panels on a real TTY.

Usage: python3 tests/test_in_place_redraw.py
This pseudo-terminal test runs on macOS/Linux; Windows uses native Win32
console clearing and should be verified in Visual Studio's console window.
"""

from pathlib import Path
import os
import subprocess
import tempfile


def main():
    if os.name == "nt":
        print("SKIP: This pseudo-terminal test runs on macOS/Linux only.")
        return

    import errno
    import pty
    import select
    import termios
    import time

    source = Path(__file__).resolve().parents[1] / "main.cpp"
    with tempfile.TemporaryDirectory(prefix="chada-redraw-") as directory:
        program = Path(directory) / "ChadaTechClocks"
        subprocess.run(
            ["g++", "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
             str(source), "-o", str(program)],
            check=True, timeout=15,
        )

        master, slave = pty.openpty()
        try:
            settings = termios.tcgetattr(slave)
            settings[3] &= ~termios.ECHO
            termios.tcsetattr(slave, termios.TCSANOW, settings)
            process = subprocess.Popen(
                [str(program)], stdin=slave, stdout=slave, stderr=slave
            )
            os.close(slave)
            slave = -1

            # The starting time crosses midnight when the user adds a second.
            os.write(master, b"23\n59\n59\n3\n4\n")
            output = bytearray()
            deadline = time.monotonic() + 5
            while time.monotonic() < deadline:
                ready, _, _ = select.select([master], [], [], 0.2)
                if ready:
                    try:
                        chunk = os.read(master, 4096)
                    except OSError as error:
                        if error.errno == errno.EIO:
                            break
                        raise
                    if not chunk:
                        break
                    output.extend(chunk)
                elif process.poll() is not None:
                    break

            if process.wait(timeout=2) != 0:
                raise AssertionError("The clock program exited unsuccessfully")

            # A terminal processes each clear/home sequence as one fresh frame.
            frames = output.decode("utf-8").split("\x1b[2J\x1b[H")
            if len(frames) != 3:
                raise AssertionError("Expected 2 in-place clock/menu frames")
            if "23:59:59" not in frames[1] or "11:59:59 P M" not in frames[1]:
                raise AssertionError("First clock frame was incorrect")
            if "00:00:00" not in frames[2] or "12:00:00 A M" not in frames[2]:
                raise AssertionError("Updated clock frame was incorrect")
            if frames[2].count("12-HOUR CLOCK") != 1 or frames[2].count("24-HOUR CLOCK") != 1:
                raise AssertionError("Old clock panels were stacked on screen")
            if "Thank you" not in frames[2]:
                raise AssertionError("Exit confirmation is missing")
            print("PASS: A real terminal receives 2 fresh frames with no stacked clocks.")
        finally:
            os.close(master)
            if slave != -1:
                os.close(slave)


if __name__ == "__main__":
    main()
