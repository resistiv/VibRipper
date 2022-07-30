/* ------------------------------------------------ */
/* Project: VibRipper                               */
/* File: Unpacker.h                                 */
/* Description: Unpacker definitions                */
/* ------------------------------------------------ */
/* Author: K. NeSmith                               */
/* GitHub: resistiv                                 */
/* ------------------------------------------------ */

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/* Unpacker buffer size. */
constexpr int UBUF = 2048;

class Unpacker
{
public:
    /* Initialize an Unpacker to unpack a PAK file into a given output directory. */
    Unpacker(std::string fileName, std::string outDir);
    /* Evaluates whether this Unpacker was constructed without error. */
    bool IsReady() const;
    /* Unpack the given PAK file. */
    int Unpack();
private:
    /* Attempts to open a PAK file for reading. */
    int OpenPAK();
    /* Reads the given PAK's table of contents. */
    int ReadTOC();
    /* Creates a directory on the disk. */
    int CreateDir(std::filesystem::path &dir);
    /* Reads a given number of bytes from an ifstream and writes them to an ofstream. */
    void WriteBytes(int n, std::ifstream &is, std::ofstream &os);
    /* Creates a text file representing a PAK TOC. */
    int WriteTOC();
    bool isReady = false;
    std::filesystem::path fileName;
    std::filesystem::path outputDir;
    std::ifstream pak;
    int fileSize;
    int fileCount;
    std::vector<int> toc;
    std::vector<std::string> names;
};