# Visual Studio: "Unable to start program ... ChadaTechClocks.exe"

The message *"The system cannot find the file specified"* means Visual Studio
tried to run an executable that does not exist at its expected location. The
most important step is to determine **why the build did not produce the EXE**.

## Try these steps, in order

1. Download the latest `CS210_SENSE_Complete_Single_CPP_Project.zip` and **extract
   the complete archive** into a new, writable folder (for example,
   `Documents\\ChadaTechClocks`). Do not work inside the compressed ZIP.
2. In Visual Studio, use **File > Open > Project/Solution**, then open
   `CS210_Project_One_Single_File\\ChadaTechClocks.sln` from the extracted folder.
3. In Solution Explorer, right-click **ChadaTechClocks** and choose
   **Set as Startup Project**.
4. Confirm the toolbar reads **Debug** and **x64**.
5. Select **Build > Rebuild Solution**. Open **View > Output**, then select
   **Show output from: Build**. Look at the first error (not the launch dialog).
6. The build needs to finish with **1 succeeded, 0 failed**. Only then select
   **Debug > Start Without Debugging** or press **Ctrl+F5**.
7. After a successful build, you should find the program here:
   `CS210_Project_One_Single_File\\x64\\Debug\\ChadaTechClocks.exe`.

## If the build fails

- **MSB8020 / missing v143 or v145 tools:** The project automatically targets
  `v143` when opened with Visual Studio 2022 and `v145` with Visual Studio 2026.
  Open **Visual Studio Installer > Modify**, install the
  **Desktop development with C++** workload, and make sure the matching MSVC
  x64/x86 build tools and Windows SDK are selected. Then rebuild.
- **C1083 / cannot open include file `windows.h`:** Install the Windows SDK
  component from the same Visual Studio Installer workload, and rebuild.
- **LNK errors:** Open **View > Output > Build**, copy the **first link error**,
  and resolve it before attempting to run; there is no valid EXE until linking
  succeeds.
- **Build succeeds, but EXE is absent:** In File Explorer, inspect the folder
  shown in step 7. Check whether Windows Security quarantined the program;
  review protection history before changing any antivirus settings.
- **Visual Studio asks to run the last successful build after errors:** Choose
  **No** and fix the build first. The previous executable may not exist.

The source is a single `main.cpp` file. If you prefer, you can also create a
fresh **C++ Console App** in Visual Studio, replace its auto-created source
with this `main.cpp`, and run **Build > Build Solution**.

The project has been compiled and behavior-tested on a GNU C++17 compiler;
native Visual Studio/MSVC compilation requires an installed Windows toolchain
and was not independently verified in this environment.
