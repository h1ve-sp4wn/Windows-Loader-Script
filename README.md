Build the Docker Image: In the directory with your Dockerfile and tdl4_injector.cpp, run the following command to build the Docker image:

    docker build -t windows_loader_script .

Run the Docker Container: After the image has built, run the container with:

    docker run --rm --isolation=process --user=Administrator windows_loader_script

The --isolation=process flag is used for Windows containers to run in process isolation, and --user=Administrator ensures it runs with administrative privileges.

Check Output: The container will run the TDL4 injector, logging any errors or information as the script executes. If everything is successful, you should see logs regarding MBR modification and rootkit injection.

Notes for Both Environments:

Administrative Privileges: The code checks for administrative privileges (check_admin_privileges) to modify the MBR and perform other system-level tasks. Ensure you run the executable as Administrator in both cases.

Hardware Access: The script tries to modify the MBR of the system drive (\\.\PhysicalDrive0). This requires low-level hardware access, which may not be feasible in a virtualized or containerized environment (such as Docker). You may need to run this on a physical machine or a virtual machine with direct access to physical hardware.

Warning: The script is designed to inject a rootkit and modify critical parts of the system, including the MBR. This code should only be run in a controlled, isolated environment for testing purposes. Running this on a production or personal system could render the system inoperable or compromise its security.

Terminal (Windows)

Dependencies:

The code is written for a Windows environment, specifically utilizing the Windows API (windows.h) for administrative privilege checks, file system manipulation, and MBR modification.

You’ll need:

Windows SDK: For windows.h and related functions.

C Compiler (e.g., MSVC or MinGW).

Steps to Compile and Run in Terminal:

Install Visual Studio:

Download and install Visual Studio with the Desktop development with C++ workload. This will provide the necessary libraries and compilers for Windows applications.

Save the Code: Save your code as windows_loader_script.cpp.

Open Visual Studio Command Prompt:

Open the Developer Command Prompt for Visual Studio. This gives you the necessary environment variables and paths to compile with MSVC.

Compile the Program: Run the following command in the Developer Command Prompt to compile the program:

    cl /EHsc /Fe:windows_loader_script.exe windows_loader_script.cpp

This compiles the windows_loader_script.cpp file and creates an executable windows_loader_script.exe.

Run the Program: To run the compiled executable, ensure that you have administrative privileges. You can open the Command Prompt as Administrator, then run:

    windows_loader_script.exe

Expected Output:

If successful, the program will log messages indicating that it's performing operations like injecting the TDL4 rootkit into the MBR, modifying the bootloader, and executing system commands.

If unsuccessful, it will log error messages (e.g., Failed to open physical disk for MBR modification).

