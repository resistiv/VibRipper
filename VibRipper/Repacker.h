/* ------------------------------------------------ */
/* Project: VibRipper                               */
/* File: Repacker.h                                 */
/* Description: Repacker definitions                */
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

/* Repacker buffer size. */
constexpr int RBUF = 2048;

class Repacker
{
public:
	/* Initialize a Repacker to repack a directory with a given TOC file. */
	Repacker(std::string inDir, std::string tocFile);
	/* Evaluates whether this Repacker was constructed without error. */
	bool IsReady() const;
	/* Repack the given directory. */
	int Repack();
private:
	/* Reads a VibRipper TOC file. */
	int ReadTOCFile(std::string &tocPath);
	/* Reads a directory to generate a TOC. */
	int ReadDirectory(std::filesystem::path &dir);
	/* Reads a given number of bytes from an ifstream and writes them to an ofstream. */
	void WriteBytes(int n, std::ifstream& is, std::ofstream& os);
	bool isReady = false;
	std::filesystem::path inputDir;
	std::filesystem::path pak;
	int fileCount;
	std::vector<int> toc;
	std::vector<std::string> names;
	std::vector<std::filesystem::path> paths;
	std::vector<int> offsets;
	std::vector<int> lengths;
	std::vector<int> nullPad;
	std::vector<int> nullPadName;
};