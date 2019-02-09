#ifndef SQUEEZE_UTILS_HPP
#define SQUEEZE_UTILS_HPP

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <map>

class Utils
{
    private:
    std::vector<std::string> _directories;
    std::map<std::string, std::vector<std::string>> _fileNames;
    std::map<std::string, std::vector<std::string>> _outputPath;

    public:
    Utils ();
    ~Utils ();
    //------ Getters
    std::map<std::string, std::vector<std::string>> getFileNames ();
    std::vector<std::string> getDirectories ();
    std::map<std::string, std::vector<std::string>> getOutputPath ();
    // -------------
    bool listSubPath (const boost::filesystem::path &);
    void displayMap (std::map<std::string, std::vector<std::string>>);
    void displayVector (std::vector<std::string>);
    void generateOutputPath (std::map<std::string, std::vector<std::string>>, std::string);
    void generateOutputPath (std::string, std::string, std::string &);
    std::vector<std::string> splitText (std::string, char);
    std::string vectorToString (std::vector<std::string>);

    void *recognitionOkResultAction(void *params);

    void *recognitionBadResultAction(void *params);

    void *authOkResultAction(void *params);

    void *authBadResultAction(void *params);
};

#endif // SQUEEZE-UTILS_HPP