#include "ConfigReader.h"

ConfigReader::ConfigReader(const std::string& filename) {
    boost::property_tree::ini_parser::read_ini(filename, pt_);
}

std::string ConfigReader::get(const std::string& section, const std::string& key) const {
    return pt_.get<std::string>(section + "." + key);
}

std::vector<short> ConfigReader::getZonePorts() const {
    std::vector<short> ports;
    for ( const auto& item : pt_.get_child("Zone") ) {
        try {
            int port = std::stoi(item.second.data());
            ports.push_back(port);
        }
        catch ( const std::invalid_argument& e ) {

        }
    }
    return ports;
}