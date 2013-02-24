#include "bibentry.h"
#include <iostream>
#include<fstream>
#include<iterator>
#include <string>
#include <complex>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

bibentry::bibentry()
{

}


// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

std::string  bibentry::getAttribute(const std::string& key) const
{
    std::string k2 = key ;
    k2=trim(k2);
    std::transform(k2.begin(), k2.end(), k2.begin(), ::tolower);

    for(std::map<std::string,std::string>::const_iterator iter = attribus.begin(); iter != attribus.end(); ++iter)
    {
        std::string k = iter->first ;
        k = trim(k);
        std::string v = iter->second;
        v=trim(v);

        std::transform(k.begin(), k.end(), k.begin(), ::tolower);

        //std::cout << "***" << k << "+++" << v << "+++" << k2 << std::endl;

        if (k.compare(k2) == 0)
        {
            return v;
        }
        //ignore value
        //Value v = iter->second;
    }
    return "";
}
