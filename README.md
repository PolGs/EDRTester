# EDR Testing Script for Windows
This script is designed to test the detection and response capabilities of endpoint detection and response (EDR) solutions. It performs a series of operations commonly used by attackers to evade detection and gain unauthorized access to a target system. The script is intended for use by security researchers and professionals in a controlled testing environment.

## Features
The script performs the following operations:

Start on Boot(reg): The script modifies the registry to add a new startup entry to ensure that the payload is executed every time the system boots up.

File Creation: The script creates a new file in a system directory to test if the EDR solution can detect the creation of new files by malicious processes.

Download Backdoor(ps): The script downloads a PowerShell script from a remote server and executes it to simulate the download and execution of a backdoor by an attacker.

System Info(ps): The script gathers system information using PowerShell commands to test the EDR solution's ability to detect suspicious activity.

Create Proc: The script creates a new process to test if the EDR solution can detect the creation of new processes by malicious processes.

Disable Windows Defender(reg): The script disables Windows Defender by modifying the registry to test if the EDR solution can detect attempts to disable antivirus software.

Process Hollowing(mem): The script uses process hollowing technique to test if the EDR solution can detect attempts to execute malicious code using legitimate processes.

## Requirements
The script is written in C and can be compiled using a C compiler for Windows, I use mingw.

## Usage
Compile the script using a C compiler for Windows.
![image](https://user-images.githubusercontent.com/19478700/218284413-ce5192f1-2e2c-4774-bc28-94cc86c4c2ad.png)
Run the compiled executable.
![image](https://user-images.githubusercontent.com/19478700/218284429-a0c522c0-a244-4050-ac22-2992028a6bd3.png)
![image](https://user-images.githubusercontent.com/19478700/218284458-bcef7b64-d2a5-4fbb-92ba-22bc4234c4f6.png)
## Disclaimer
The script is provided for educational and testing purposes only. It should only be used in a controlled testing environment and should not be used for any malicious or illegal activities. The author assumes no responsibility for any damages or consequences resulting from the use or misuse of this script.
