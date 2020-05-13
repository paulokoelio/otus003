#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "ipf_basefncl.h"
#include "ip_filter.h"

namespace ipf
{

std::vector<std::string> split(const std::string &str, char d, const size_t parts)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    size_t iter = parts;
    while ((stop != std::string::npos) && (iter > 0))
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
        --iter;
    }
    if (iter > 0)
        r.push_back(str.substr(start));

    return r;
}

size_t convertoip(const std::vector<std::string> &vtxt,
                  const ipver ipver,
                  std::vector<int> &ip_bytes,
                  bool &ipnotconverted,
                  size_t &errorcode)
{
    size_t bytesnumber;
    switch (ipver)
    {
    case ipf::ipver::IPV4:
        bytesnumber = 4;
        break;
    case ipf::ipver::IPV6:
        bytesnumber = 16;
        break;
    }
    ipnotconverted = true;
    if (vtxt.size() < bytesnumber)
    {
        errorcode = 3;
        return errorcode;
    }
    if (ip_bytes.size() < bytesnumber)
    {
        ip_bytes.resize(bytesnumber);
    }
    std::stringstream convertor;
    errorcode = 0;
    ipnotconverted = false;
    for (size_t i = 0; i < bytesnumber; ++i)
    {
        convertor.str("");
        convertor.clear();
        convertor << vtxt.at(i);
        convertor >> ip_bytes.at(i);    // \todo: add check of non-digit simbols in tail of text
                                        // current conversion doesn't detect non-digits in the tail of text
                                        // since it is standard behavior of std::stringstream object
        if ((convertor.fail()) || (ip_bytes.at(i) > 255) || (ip_bytes.at(i) < 0))
        {
            errorcode = 2; // error during conversion of string to number (0-255)
            ipnotconverted = true;
            break;
        }
    }
    return errorcode;
}

bool maskpassed(const std::vector<int> &mask, const std::vector<int> &ipbytes)
{
    // \todo Add check of length of mask and adaptation of mask to size of ipbytes+1, using some rules like:
    // empty mask = 0,-1,-1,...; mask with 0,x = 0,x,x,..x; with 1,x = 1,x,-1,-1,..; 
    bool res = false;
    if (mask.at(0) == 1) // strict mask
    {
        res = ((mask.at(1) < 0) || (mask.at(1) == ipbytes.at(0)));
        for (std::size_t i = 1; (i < ipbytes.size()) && res; ++i)
        {
            if (mask.at(i + 1) < 0)
                continue;
            res = (mask.at(i + 1) == ipbytes.at(i));
        }
    }
    if (mask.at(0) == 0) // weak mask
    {
        int check_bytes = 0;
        for (std::size_t i = 0; (i < ipbytes.size()) && (!res); ++i)
        {
            if (mask.at(i + 1) < 0)
                continue;
            check_bytes++;
            res = (mask.at(i + 1) == ipbytes.at(i));            
        }
        if (check_bytes == 0)   // if mask contains only -1 then any ipaddress can pass this mask 
            res = true;
    }
    return res;
}

} // namespace ipf