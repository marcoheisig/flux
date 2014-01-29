
#include <FileReader.hh>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

void FileReader::registerIntParameter(const string &key, int init)
{
    parameters[key] = 0;
    setParameter(key, init);
}

void FileReader::registerRealParameter(const string &key, real init)
{
    parameters[key] = 1;
    setParameter(key, init);
}

void FileReader::registerStringParameter(const string &key, const string &init)
{
    parameters[key] = 2;
    setParameter(key, init);
}

void FileReader::setParameter(const string &key, const string &in)
{
    stringParameters[key] = in;
}

void FileReader::setParameter(const string &key, real in)
{
    realParameters[key] = in;
}

void FileReader::setParameter(const string &key, int in)
{
    intParameters[key] = in;
}


bool FileReader::readFile(const string &name)
{
    ifstream f(name);
    
    // Unable to open file?
    if(f.fail())
        return false;
    
    string line;
    while(getline(f, line)) {
        // Removing comments:
        size_t pos = line.find_first_of('#');
        if(pos != line.npos)
            line.erase(pos, line.npos);
        
        // Trimming:
        pos = line.find_first_not_of(" \t\n\r");
        if(pos == line.npos) {
            line.clear();
        } else {
            line.erase(0, pos);
        }
        pos = line.find_last_not_of(" \t\n\r");
        if(pos != line.npos)
            line.erase(pos+1, line.npos);
        
        // Is there anything left?
        if(line.size() > 0) {
            istringstream linestream(line);
            
            string key;
            linestream >> key;
            
            try {
                // Check for registered type
                short parametertype = parameters.at(key);
                
                if(parametertype == 0) { // int
                    int value;
                    linestream >> value;
                    setParameter(key, value);
                } else if(parametertype == 1) { // real
                    real value;
                    linestream >> value;
                    setParameter(key, value);
                } else if(parametertype == 2) { // string
                    string value;
                    linestream >> value;
                    setParameter(key, value);
                }
            } catch(const out_of_range& oor) {
                cerr << "Ignored unregistered parameter: " << key << endl;
                string thevoid;
                linestream >> thevoid;
            }
        }
    }
    
    return true;
}



void FileReader::printParameters() const
{
    for(auto it = parameters.begin(); it != parameters.end(); ++it) {
        cout << it->first << " is of type " << it->second;
        if(it->second == 0) 
            cout << " (int)";
        else if(it->second == 1) 
            cout << " (real)";
        else if(it->second == 2) 
            cout << " (string)";
        else
            cout << " (unknown)";
        cout << endl;
    }
    
    for(auto it = intParameters.begin(); it != intParameters.end(); ++it)
        cout << it->first << " = " << it->second << endl;
    for(auto it = realParameters.begin(); it != realParameters.end(); ++it)
        cout << it->first << " = " << it->second << endl;
    for(auto it = stringParameters.begin(); it != stringParameters.end(); ++it)
        cout << it->first << " = " << it->second << endl;
}


