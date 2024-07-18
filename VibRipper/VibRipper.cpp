/* ------------------------------------------------ */
/* Project: VibRipper                               */
/* File: VibRipper.cpp                              */
/* Description: Program entry point                 */
/* ------------------------------------------------ */
/* Author: K. NeSmith                               */
/* GitHub: resistiv                                 */
/* ------------------------------------------------ */

#include <iostream>
#include "Repacker.h"
#include "Unpacker.h"
#include "VibRipper.h"

/* Program entry point. */
int main(int argc, char** argv)
{
    // Make a good first impression :)
    std::cout << std::endl << PROGRAM << " v" << VERSION << " by " << AUTHOR << std::endl;

    // Are we clueless?
    if (argc == 1)
        return WriteUsage();

    // Process arguments
    switch (*argv[1])
    {
    // Help
    case 'h':
        return WriteUsage() | WriteHelp();

    // Repack
    case 'r':
    {
        // Check args
        if (argc < 3 || argc > 4)
        {
            std::cerr << "Incorrect number of arguments for option '" << *argv[1] << "', pass 'h' for help." << std::endl;
            return EXIT_FAILURE;
        }

        // Instantiate
        Repacker r(argv[2], argc == 4 ? argv[3] : "");

        // Repack if possible
        if (r.IsReady())
            return r.Repack();
        else
            return EXIT_FAILURE;
    }

    // Unpack
    case 'u':
    {
        // Check args
        if (argc < 3 || argc > 4)
        {
            std::cerr << "Incorrect number of arguments for option '" << *argv[1] << "', pass 'h' for help." << std::endl;
            return EXIT_FAILURE;
        }

        // Instantiate
        Unpacker u(argv[2], argc == 4 ? argv[3] : "");

        // Unpack if possible
        if (u.IsReady())
            return u.Unpack();
        else
            return EXIT_FAILURE;
    }

    default:
    {
        std::cerr << "Unknown option '" << *argv[1] << "', pass 'h' for help." << std::endl;
        return EXIT_FAILURE;
    }
    }

    return EXIT_SUCCESS;
}

/* Writes a basic usage statement to output. */
int WriteUsage()
{
    std::cout << "Usage: " << PROGRAM << " " << USAGE << std::endl;

    return EXIT_SUCCESS;
}

/* Writes a detailed help page to output. */
int WriteHelp()
{
    std::cout << "Options:" << std::endl;
    for (std::string_view sv : OPTIONS)
        std::cout << "  " << sv << std::endl;

    return EXIT_SUCCESS;
}
