/**
@file   lms8cli.cpp
@brief  CLI tool for configuring LMS8001 boards via .ini files
*/

#include "LMS8001.h"
#include "lmsComms.h"
#include "INI.h"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

void printUsage()
{
    cout << "LMS8 CLI - LMS8001 Board Configuration Tool\n\n";
    cout << "Usage:\n";
    cout << "  lms8cli load <config.ini>    Load configuration from .ini to board\n";
    cout << "  lms8cli save <config.ini>    Save board configuration to .ini file\n";
    cout << "  lms8cli info                 Show connected device information\n";
    cout << "  lms8cli list                 List available devices\n";
}

bool connectAndConfigure(LMScomms& comms, LMS8001& lms)
{
    // Refresh device list and connect
    int devCount = comms.RefreshDeviceList();
    if (devCount == 0)
    {
        cerr << "ERROR: No LMS8001 devices found\n";
        return false;
    }

    vector<string> devices = comms.GetDeviceList();
    cout << "Found " << devCount << " device(s):\n";
    for (size_t i = 0; i < devices.size(); ++i)
        cout << "  [" << i << "] " << devices[i] << "\n";

    // Open first device
    if (comms.Open(0) != IConnection::SUCCESS)
    {
        cerr << "ERROR: Failed to open device\n";
        return false;
    }
    cout << "Connected to " << devices[0] << "\n";
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    string cmd = argv[1];

    // Commands that don't need board connection
    if (cmd == "list")
    {
        LMScomms comms;
        int devCount = comms.RefreshDeviceList();
        cout << "Found " << devCount << " device(s):\n";
        for (auto& name : comms.GetDeviceList())
            cout << "  " << name << "\n";
        return 0;
    }

    if (cmd == "help" || cmd == "--help" || cmd == "-h")
    {
        printUsage();
        return 0;
    }

    // Commands that need board connection
    LMScomms comms;
    LMS8001 lms(&comms);

    if (cmd == "load")
    {
        if (argc < 3) { cerr << "ERROR: Missing .ini filename\n"; return 1; }

        if (!connectAndConfigure(comms, lms)) return 1;

        liblms8_status status = lms.LoadConfig(argv[2]);
        if (status == LIBLMS8_SUCCESS)
            cout << "SUCCESS: Configuration loaded from " << argv[2] << "\n";
        else if (status == LIBLMS8_FILE_NOT_FOUND)
            cerr << "ERROR: File not found: " << argv[2] << "\n";
        else
            cerr << "ERROR: Failed to load configuration\n";

        comms.Close();
        return (status == LIBLMS8_SUCCESS) ? 0 : 1;
    }
    else if (cmd == "save")
    {
        if (argc < 3) { cerr << "ERROR: Missing .ini filename\n"; return 1; }

        if (!connectAndConfigure(comms, lms)) return 1;

        // Ensure we have current data from chip
        lms.DownloadAll();

        liblms8_status status = lms.SaveConfig(argv[2]);
        if (status == LIBLMS8_SUCCESS)
            cout << "SUCCESS: Configuration saved to " << argv[2] << "\n";
        else
            cerr << "ERROR: Failed to save configuration\n";

        comms.Close();
        return (status == LIBLMS8_SUCCESS) ? 0 : 1;
    }
    else if (cmd == "info")
    {
        if (!connectAndConfigure(comms, lms)) return 1;

        LMSinfo info = comms.GetInfo();
        cout << "Device: " << (int)info.device << "\n";
        cout << "Firmware: " << info.firmware << "\n";
        cout << "Hardware: " << info.hardware << "\n";
        cout << "Protocol: " << info.protocol << "\n";

        comms.Close();
        return 0;
    }
    else
    {
        cerr << "ERROR: Unknown command: " << cmd << "\n";
        printUsage();
        return 1;
    }

    return 0;
}
