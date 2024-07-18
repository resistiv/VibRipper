/* ------------------------------------------------ */
/* Project: VibRipper                               */
/* File: VibRipper.h                                */
/* Description: Main function definitions           */
/* ------------------------------------------------ */
/* Author: K. NeSmith                               */
/* GitHub: resistiv                                 */
/* ------------------------------------------------ */

#pragma once

#include <string>
#include <vector>

constexpr std::string_view PROGRAM = "VibRipper";
const int MAJORVER = 1;
const int MINORVER = 2;
const std::string VERSION = std::to_string(MAJORVER) + "." + std::to_string(MINORVER);
constexpr std::string_view AUTHOR = "ResistivKai";
constexpr std::string_view USAGE = "{ u <pakfile> [outdir] | r <indir> [tocfile] }";
const std::vector<std::string_view> OPTIONS =
{
    "h\t\t\tPrint a help page to output (hey, you're here!).",
    "u <pakfile> [outdir]\tUnpack a specified *.PAK file to an optionally defined directory.",
    "r <indir> [tocfile]\tRepack a specified directory using an optionally defined table of contents file."
};

/* Writes a basic usage statement to output. */
int WriteUsage();
/* Writes a detailed help page to output. */
int WriteHelp();
