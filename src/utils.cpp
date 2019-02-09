#include "utils.hpp"

using namespace std;
using namespace boost::filesystem;

static string directory;
/* Constructor */
Utils::Utils ()
{
}
Utils::~Utils ()
{
}
/* Getters */

std::vector<string> Utils::getDirectories ()
{
    return this->_directories;
}

std::map<string, std::vector<string>> Utils::getFileNames ()
{
    return this->_fileNames;
}

std::map<string, std::vector<string>> Utils::getOutputPath ()
{
    return this->_outputPath;
}

/* Class methods */

bool Utils::listSubPath (const path &dirpath)
{
    try
    {
        directory = dirpath.string ();
        if (!exists (dirpath))
        {
            cout << "Inexistant path" << endl;
            return false;
        }
        for (auto &&x : directory_iterator (dirpath))
        {
            if (is_directory(x))
            {
                directory = x.path ().string ();
                _directories.push_back (directory);

                listSubPath (x.path ());
            }
            else
            {
                std::map<string, std::vector<string>>::iterator it = _fileNames.find (directory);
                if (it != _fileNames.end ())
                {
                    it->second.push_back (x.path ().filename ().string ());
                }
                else
                {
                    std::vector<string> vect;
                    vect.push_back (x.path ().filename ().string ());
                    _fileNames.insert (std::make_pair (directory, vect));
                }
            }
        }
        return true;
    }
    catch (const filesystem_error &ex)
    {
        cout << ex.what () << endl;
        return false;
    }
}

void Utils::generateOutputPath (std::map<string, std::vector<string>> listPath, string outputFileName)
{
    for (auto const &x : listPath)
    {
        std::vector<string> vect;
        vect = splitText (x.first, '/');
        vect[vect.size () - 2] = outputFileName;
        _outputPath.insert (std::make_pair (vectorToString (vect), x.second));
    }
}

void Utils::generateOutputPath (std::string inputP, std::string outputFileName, std::string &outputP)
{
    std::vector<string> vect = splitText (inputP, '/');
    vect[vect.size () - 2] = outputFileName;
    outputP = vectorToString (vect);
    string command = "mkdir "+outputP;
    int resultSys = system(command.c_str());
}

std::vector<string> Utils::splitText (std::string fileToSplit, char delimiter)
{
    std::vector<string> results;
    /* & <-- capturing variable */
    boost::split (results, fileToSplit, [&](char c) { return c == delimiter; });

    return results;
}

std::string Utils::vectorToString (std::vector<string> vectStr)
{
    string result;
    if (vectStr.size () == 0) return "";
    for (auto i = vectStr.begin (); i != vectStr.end (); i++)
    {
        if (i != vectStr.begin ()) result += '/';
        result += *i;
    }

    return result;
}

void Utils::displayMap (std::map<string, vector<string>> dico)
{
    for (auto const &x : dico)
    {
        cout << x.first << " has vector containing --- " << endl;
        displayVector (x.second);
    }
}

void Utils::displayVector (std::vector<string> vect)
{
    for (auto i = vect.begin (); i != vect.end (); i++)
    {
        cout << *i << " value of vector" << endl;
    }
}

void *Utils::recognitionOkResultAction(void *params) {

}

void *Utils::recognitionBadResultAction(void *params) {

}

void *Utils::authOkResultAction(void *params) {

}

void *Utils::authBadResultAction(void *params) {

}