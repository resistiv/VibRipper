/* ------------------------------------------------ */
/* Project: VibRipper                               */
/* File: Unpacker.cpp                               */
/* Description: PAK repacking module                */
/* ------------------------------------------------ */
/* Author: K. NeSmith                               */
/* GitHub: resistiv                                 */
/* ------------------------------------------------ */

#include "Repacker.h"
#include "VibRipper.h"

/* Initialize a Repacker to repack a directory with a given TOC file. */
Repacker::Repacker(std::string inDir, std::string tocFile)
{
	std::cout << "[R] Initializing Repacker..." << std::endl;

	// Validate input directory
	this->inputDir = std::filesystem::path(inDir);
	if (!std::filesystem::exists(inputDir) || !std::filesystem::is_directory(inputDir))
	{
		std::cerr << "[R] Could not find directory '" << inputDir.string() << "'." << std::endl;
		return;
	}
	this->inputDir = std::filesystem::absolute(inputDir);

	// Read in TOC from file
	if (tocFile != "")
	{
		std::cout << "[R] Reading TOC file..." << std::endl;
		if (!ReadTOCFile(tocFile))
			return;
	}
	// Read in TOC from directory
	else
	{
		std::cout << "[R] Reading directory..." << std::endl;
		if (!ReadDirectory(inputDir))
			return;
	}

	// Done!
	isReady = true;
}

/* Evaluates whether this Repacker was constructed without error. */
bool Repacker::IsReady() const
{
	return isReady;
}

/* Repack the given directory. */
int Repacker::Repack()
{
	std::cout << "[R] Repacking '" << inputDir.string() << "'..." << std::endl;

	// Create & open PAK
	std::ofstream pakFile;
	pakFile.open(pak.string(), std::ios::out | std::ios::binary);
	if (!pakFile.is_open() || !pakFile.good())
	{
		std::cerr << "[U] Could not create file '" << pak.string() << "' for writing." << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "[R] Generating header..." << std::endl;

	// Header size as first offset
	int headerSize = 4 + (4 * (int)names.size());
	offsets.push_back(headerSize);

	// Generate remaining offsets
	for (int i = 0; i < fileCount; i++)
	{
		// Find canonical path
		std::string tempName = names[i];
		std::replace(tempName.begin(), tempName.end(), '/', (char)std::filesystem::path::preferred_separator);
		std::filesystem::path tempPath = inputDir.string() + (char)std::filesystem::path::preferred_separator + tempName;
		// Add for later use
		paths.push_back(tempPath);

		// Get file size
		int tempSize = (int)std::filesystem::file_size(tempPath);
		lengths.push_back(tempSize);

		// Find null padding needed for file data
		int nullPd = 0;
		while ((tempSize + nullPd) % 4 != 0)
			nullPd++;
		nullPad.push_back(nullPd);

		// Find null padding for file name
		int nameLen = (int)names[i].size() + 1;
		int nullPdName = 0;
		while ((nameLen + nullPdName) % 4 != 0)
			nullPdName++;
		nullPadName.push_back(nullPdName);

		if (i != fileCount - 1)
			offsets.push_back(offsets[i] + (int)names[i].size() + 1 + nullPadName[i] + 4 + lengths[i] + nullPad[i]);
		// The above factors in the offset of the file, the name & name padding, the length field, the file data length, and the file data null padding
	}

	// Write PAK
	std::cout << "[R] Writing file count..." << std::endl;
	pakFile.write((char *)&fileCount, 4);

	std::cout << "[R] Writing offset table..." << std::endl;
	for (int o : offsets)
		pakFile.write((char *)&o, 4);

	for (int i = 0; i < fileCount; i++)
	{
		std::cout << "[R] Packing '" << names[i] << "'..." << std::endl;

		// Write file name
		pakFile.write(names[i].c_str(), (int)names[i].size() + 1);
		while (nullPadName[i]-- != 0)
			pakFile.put('\0');

		// Write length
		pakFile.write((char *)&lengths[i], 4);

		// Open file to read
		std::ifstream inFile;
		inFile.open(paths[i].string(), std::ios::in | std::ios::binary);
		if (!inFile.is_open() || !inFile.good())
		{
			std::cerr << "[U] Could not open file '" << paths[i].string() << "' for reading." << std::endl;
			return EXIT_FAILURE;
		}

		// Write data to PAK
		WriteBytes(lengths[i], inFile, pakFile);
		while (nullPad[i]-- != 0)
			pakFile.put('\0');
	}

	// Tie up loose ends
	std::cout << "[R] Done repacking files." << std::endl;
	pakFile.close();

	return EXIT_SUCCESS;
}

/* Reads a VibRipper TOC file. */
int Repacker::ReadTOCFile(std::string &tocPath)
{
	// Open TOC
	std::ifstream tocFile;
	tocFile.open(tocPath, std::ios::in);
	if (!tocFile.is_open() || !tocFile.good())
	{
		std::cerr << "[R] Could not open TOC file '" << tocPath << "' for reading." << std::endl;
		return 0;
	}

	// Read magic header
	std::string header;
	getline(tocFile, header);
	if (header.length() != 19 + PROGRAM.length() + VERSION.length())
	{
		std::cerr << "[R] Read invalid header in TOC file." << std::endl;
		return 0;
	}
	if (header[15] - '0' != MAJORVER)
	{
		std::cerr << "[R] Version mismatch in TOC header; this file is incompatible with " << PROGRAM << " v" << VERSION << "." << std::endl;
		return 0;
	}

	// Read output PAK file name
	std::string pakName;
	getline(tocFile, pakName);
	this->pak = std::filesystem::path(inputDir.parent_path().string() + (char)std::filesystem::path::preferred_separator + pakName);

	// Read file count
	std::string fileCountStr;
	getline(tocFile, fileCountStr);
	this->fileCount = std::stoi(fileCountStr);

	// Read in all file names
	for (int i = 0; i < fileCount; i++)
	{
		std::string nameBuf;
		if (!std::getline(tocFile, nameBuf))
		{
			std::cerr << "[R] Unexpected end-of-file encountered while reading TOC file." << std::endl;
			return 0;
		}
		names.push_back(nameBuf);
	}

	// Tie up loose ends
	tocFile.close();

	return 1;
}

/* Reads a directory to generate a TOC. */
int Repacker::ReadDirectory(std::filesystem::path &dir)
{
	// Create PAK name from directory
	pak = std::filesystem::path(dir.string() + ".PAK");

	// Get parent length to trim child paths, add separator length
	int parLen = (int)inputDir.string().length() + 1;

	// Traverse the directory
	std::filesystem::recursive_directory_iterator inDir(dir);
	for (const std::filesystem::directory_entry &dir_entry : inDir)
	{
		// Only add files
		if (!dir_entry.is_directory())
		{
			// Trim name
			std::string tempName = dir_entry.path().string().substr(parLen, std::string::npos);

			// Clean name
			if ((char)std::filesystem::path::preferred_separator != '/')
				std::replace(tempName.begin(), tempName.end(), (char)std::filesystem::path::preferred_separator, '/');

			names.push_back(tempName);
			fileCount++;
		}
	}

	return 1;
}

/* Reads a given number of bytes from an ifstream and writes them to an ofstream. */
void Repacker::WriteBytes(int n, std::ifstream& is, std::ofstream& os)
{
	int outBuf[RBUF];
	int bytesLeft = n;
	int toRead = 0;
	while (bytesLeft != 0)
	{
		toRead = (bytesLeft >= RBUF) ? RBUF : bytesLeft;
		is.read((char*)outBuf, toRead);
		bytesLeft -= toRead;
		os.write((char*)outBuf, toRead);
	}
}