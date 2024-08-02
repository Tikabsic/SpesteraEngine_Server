#include "ConfigReader.h"

ConfigReader::ConfigReader(const std::string& filename) {
    boost::property_tree::ini_parser::read_ini(filename, pt_);
}

std::string ConfigReader::get(const std::string& section, const std::string& key) const {
    return pt_.get<std::string>(section + "." + key);
}