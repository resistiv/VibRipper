/* ------------------------------------------------ */
/* Project: VibRipper                               */
/* File: Unpacker.cpp                               */
/* Description: PAK unpacking module                */
/* ------------------------------------------------ */
/* Author: K. NeSmith                               */
/* GitHub: resistiv                                 */
/* ------------------------------------------------ */

#include <algorithm>
#include "Unpacker.h"
#include "VibRipper.h"

/* Initialize an Unpacker to unpack a PAK file into a given output directory. */
Unpacker::Unpacker(std::string fileName, std::string outDir)
{
    std::cout << "[U] Initializing Unpacker..." << std::endl;

    this->fileName = std::filesystem::path(fileName);

    if (!OpenPAK())
        return;

    // Get file size for error checking 
    fileSize = (int)std::filesystem::file_size(fileName);

    // Get full directory path
    if (outDir == "")
    {
        std::filesystem::path fullPath = std::filesystem::absolute(this->fileName);
        outputDir = std::filesystem::path(fullPath.string() + "_out");
    }
    else
        outputDir = std::filesystem::absolute(std::filesystem::path(outDir));

    // Create main output dir
    if (!CreateDir(outputDir))
        return;

    // Done!
    isReady = true;
}

/* Evaluates whether this Unpacker was constructed without error. */
bool Unpacker::IsReady() const
{
    return isReady;
}

/* Unpack the given PAK file. */
int Unpacker::Unpack()
{
    std::cout << "[U] Unpacking '" << fileName.filename().string() << "'..." << std::endl;

    if (!ReadTOC())
        return 0;

    // Traverse archive
    for (int i = 0; i < fileCount; i++)
    {
        // Seek to file
        pak.seekg(toc[i], std::ios_base::beg);

        // Read name
        std::string name = "";
        char c;
        while ((c = pak.get()) != '\0')
            name += c;

        // Add name for output later
        names.push_back(name);

        // Skip name padding (next 4-byte border):
        // For clarity, we subtract from 3 as we've already consumed
        // one of the null bytes in the above code, so the most we
        // advance is three bytes.
        pak.seekg(3 - (name.size() % 4), std::ios_base::cur);

        // Read file length
        int fileLen;
        pak.read((char *)&fileLen, 4);

        // Clean name
        if ((char)std::filesystem::path::preferred_separator != '/')
            std::replace(name.begin(), name.end(), '/', (char)std::filesystem::path::preferred_separator);

        // Create directories if needed
        size_t slash = name.find_last_of(std::filesystem::path::preferred_separator);
        if (slash != std::string::npos)
        { // Subdirectories present, create them!!
            std::filesystem::path newDir = outputDir.string() + (char)std::filesystem::path::preferred_separator + name.substr(0, slash);
            if (!CreateDir(newDir))
                return EXIT_FAILURE;
        }

        // Create output
        std::ofstream outFile(outputDir.string() + (char)std::filesystem::path::preferred_separator + name, std::ios::out | std::ios::binary);
        if (!outFile.is_open() || !outFile.good())
        {
            std::cerr << "[U] Could not open file '" << name << "' for writing." << std::endl;
            return EXIT_FAILURE;
        }

        // Write bytes to output
        std::cout << "[U] Unpacking " << name << "..." << std::endl;
        WriteBytes(fileLen, pak, outFile);

        outFile.close();
    }

    std::cout << "[U] Done unpacking files." << std::endl;

    // Write table of contents
    std::cout << "[U] Writing table of contents..." << std::endl;
    if (!WriteTOC())
        return EXIT_FAILURE;
    std::cout << "[U] Done writing table of contents." << std::endl;

    // Tie up loose ends
    pak.close();

    return EXIT_SUCCESS;
}

/* Attempts to open a PAK file for reading. */
int Unpacker::OpenPAK()
{
    pak.open(fileName.string(), std::ios::in | std::ios::binary);
    if (!pak.is_open() || !pak.good())
    {
        std::cerr << "[U] Could not open file '" << fileName.string() << "' for reading." << std::endl;
        return 0;
    }

    return 1;
}

/* Reads the given PAK's table of contents. */
int Unpacker::ReadTOC()
{
    int temp;

    // Read file count
    pak.read((char*)&fileCount, 4);
    std::cout << "[U] " << fileCount << " files to unpack." << std::endl;

    // Traverse table of contents
    for (int i = 0; i < fileCount; i++)
    {
        pak.read((char*)&temp, 4);
        toc.push_back(temp);

        // Validate offsets
        if (toc.back() > fileSize)
        {
            std::cerr << "[U] Received out-of-range offset '0x" << std::hex << toc.back() << std::dec << "' in the table of contents." << std::endl;
            return 0;
        }
    }

    return 1;
}

/* Creates a directory on the disk. */
int Unpacker::CreateDir(std::filesystem::path &dir)
{
    // Create directory and all sub-directories
    try
    {
        std::filesystem::create_directories(dir);
    }
    catch (std::filesystem::filesystem_error &err)
    {
        std::cerr << "[U] Failed to create output directory with path '" << dir << "':" << std::endl;
        std::cerr << "[U] " << err.code() << ": " << err.what() << std::endl;
        return 0;
    }

    return 1;
}

/* Reads a given number of bytes from an ifstream and writes them to an ofstream. */
void Unpacker::WriteBytes(int n, std::ifstream &is, std::ofstream &os)
{
    int outBuf[UBUF];
    int bytesLeft = n;
    int toRead = 0;
    while (bytesLeft != 0)
    {
        toRead = (bytesLeft >= UBUF) ? UBUF : bytesLeft;
        is.read((char *)outBuf, toRead);
        bytesLeft -= toRead;
        os.write((char *)outBuf, toRead);
    }
}

/* Creates a text file representing a PAK TOC. */
int Unpacker::WriteTOC()
{
    // Create TOC
    std::ofstream tocFile(fileName.string() + "_TOC.txt", std::ios::out);
    if (!tocFile.is_open() || !tocFile.good())
    {
        std::cerr << "[U] Could not open TOC file for writing." << std::endl;
        return 0;
    }

    // Header
    tocFile << "### " << PROGRAM << " v" << VERSION << " TOC File ###" << std::endl;
    tocFile << fileName.filename().string() << std::endl;
    tocFile << std::to_string(fileCount) << std::endl;

    // Write filenames
    for (int i = 0; i < fileCount; i++)
        tocFile << names[i] << std::endl;

    tocFile.close();

    return 1;
}
