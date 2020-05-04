#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "version.h"
#include "ip_filter.h"
#include "ipf_basefncl.h"

using namespace ipf;

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    try
    {
        std::vector<std::string> lines_in;
        std::vector<parsedata> ip_parsed;
        using mmapgreater = std::multimap<unsigned int, std::size_t, std::greater<unsigned int>>;
        mmapgreater ip_sorted;
        using mmap_data = mmapgreater::value_type;

        size_t counts = 0;
        parsedata pline;
        pline.ipstr = "";
        pline.ipstr.reserve(ipaddr_str_length);
        pline.ipbytes.reserve(ipaddr_str_length);

        for (std::string line; std::getline(std::cin, line); ++counts)
        {
            pline.original_order = counts;
            pline.errorcode = 0;

            lines_in.push_back(line);

            auto v = split(line, field_delimiter, ipaddr_position + 1);
            auto vsub = split(v.at(ipaddr_position), ipaddr_delimiter, ipaddr_byte_length);

            pline.skipped = false;
            unsigned int idfromipv4;

            if (
                (v.at(ipaddr_position).size() > ipaddr_str_length) ||
                (v.at(ipaddr_position).size() < ipaddr_str_minlength) ||
                (vsub.size() < ipaddr_byte_length))
            {
                pline.errorcode = 1; // too long or short or incorrect format of ip address in line
                pline.skipped = true;
            }
            else
            {
                convertoip(vsub, EN_IPF_IPV4, pline.ipbytes, pline.skipped, pline.errorcode);
                if (!pline.skipped)
                {
                    idfromipv4 = 256 * 256 * 256 * pline.ipbytes.at(0) + 256 * 256 * pline.ipbytes.at(1) +
                                 256 * pline.ipbytes.at(2) + pline.ipbytes.at(3);
                    pline.ipstr = v.at(ipaddr_position);
                    mmap_data mmappair(idfromipv4, pline.original_order);
                    ip_sorted.insert(mmappair);
                }
            }

/* 
            for (auto lineout = v.cbegin(); lineout != v.cend(); lineout++)
            {
                std::cout << "#" << *lineout;
            }
            std::cout << std::endl;

            for (auto lineout = vsub.cbegin(); lineout != vsub.cend(); lineout++)
            {
                if (lineout != vsub.cbegin())
                    std::cout << ".";
                std::cout << *lineout;
            }
*/
/*             if (pline.skipped)
                std::cout << "\t line was skipped with warning = " << pline.errorcode;
            else
            {
                std::cout << "\t" << pline.ipbytes[0];
                for (int iter = 1; iter < ipaddr_byte_length; ++iter)
                    std::cout << "." << pline.ipbytes[iter];
                std::cout << "\t" << idfromipv4;
                std::cout << "\t" << pline.ipstr;
                std::cout << "\t" << pline.original_order;
            }
            std::cout << std::endl; 
*/
            ip_parsed.push_back(pline);
        }
/*         
        for (auto iterp = ip_sorted.cbegin(); iterp != ip_sorted.cend(); ++iterp)
        {
            std::cout << " [" << iterp->first << ':' << iterp->second << ']';
            std::cout << "\t\t\t" << ip_parsed.at(iterp->second).ipstr;
            std::cout << '\n';
        }
 */
        std::vector<std::size_t> cur_sortedv;
        cur_sortedv.resize(ip_sorted.size());
        filteredata p1_1{{1, 1, -1, -1, -1}, {}};
        filteredata p2_46_70{{1, 46, 70, -1, -1}, {}};
        filteredata p3_any46{{0, 46, 46, 46, 46}, {}};
        counts = 0;
        for (auto iters = ip_sorted.cbegin(); iters != ip_sorted.cend(); ++iters)
        {
            cur_sortedv.at(counts) = iters->second;
            if (maskpassed(p1_1.mask, ip_parsed.at(iters->second).ipbytes))
                p1_1.orig_order.push_back(iters->second);
            if (maskpassed(p2_46_70.mask, ip_parsed.at(iters->second).ipbytes))
                p2_46_70.orig_order.push_back(iters->second);
            if (maskpassed(p3_any46.mask, ip_parsed.at(iters->second).ipbytes))
                p3_any46.orig_order.push_back(iters->second);
            counts++;
        };

        // std::cout << "Sorted vector" << '\n';
        for (auto iterp = cur_sortedv.cbegin(); iterp != cur_sortedv.cend(); ++iterp)
        {
            std::cout << ip_parsed.at(*iterp).ipstr << '\n';
        }
        // std::cout << "filtered p1_1" << '\n';
        for (auto iterf = p1_1.orig_order.cbegin(); iterf != p1_1.orig_order.cend(); ++iterf)
        {
            std::cout << ip_parsed.at(*iterf).ipstr << '\n';
        }
        // std::cout << "filtered p2_46_70" << '\n';
        for (auto iterf = p2_46_70.orig_order.cbegin(); iterf != p2_46_70.orig_order.cend(); ++iterf)
        {
            std::cout << ip_parsed.at(*iterf).ipstr << '\n';
        }
        // std::cout << "filtered p3_any46" << '\n';
        for (auto iterf = p3_any46.orig_order.cbegin(); iterf != p3_any46.orig_order.cend(); ++iterf)
        {
            std::cout << ip_parsed.at(*iterf).ipstr << '\n';
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "ip_filter version " << version() << std::endl;
    }

    return 0;
}
