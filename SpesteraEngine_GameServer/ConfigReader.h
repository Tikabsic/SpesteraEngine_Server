#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string>

class ConfigReader {
public:
    ConfigReader(const std::string& filename);

    std::string get(const std::string& section, const std::string& key) const;

private:
    boost::property_tree::ptree pt_;
};

#endif // CONFIGREADER_H