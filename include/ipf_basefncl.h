#ifndef IPFBASEFNCL_H_
#define IPFBASEFNCL_H_

#define UNUSED(variable) (void)variable

namespace ipf
{

const size_t default_parts = 1; // определяем, сколько слов достаточно изъять из строки
                                // для получения ip адреса

std::vector<std::string> split(const std::string &str, char d, const size_t parts = default_parts);

enum ipf_ipver : int
{
    EN_IPF_IPV4,
    EN_IPF_IPV6
};

struct parsedata
{
std::size_t original_order{0};
std::string ipstr;
std::vector<int> ipbytes;
bool skipped {false};
std::size_t errorcode{0};
};

struct filteredata
{
std::vector<int> mask;                  // mask.at(0) - element is for compare operation - logical (0 - OR, 1 - AND) 
                                        // between all bytes of ipaddr,
                                        // other elements of vector are mask using bytes value (0-255)
                                        // or if element is negative (-1) this position is not compared.
                                        // example: {1,46,72,-1,-1} - means if address has first byte 46 AND second byte 72
                                        // or: {0,46,46,46,46} - means if ANY of byte is equal to 46
std::vector<std::size_t> orig_order;
};

size_t convertoip(const std::vector<std::string> &vtxt,
                  const ipf_ipver ipver,
                  std::vector<int> &ip_bytes,
                  bool &ipnotconverted,
                  size_t &errorcode);
/* 
errorcodes:
            2 - error in conversion from text to int values of (0-255) 
            3 - size of input vector of vtxt is less than required
 */

bool maskpassed(const std::vector<int> &mask, const std::vector<int> &ipbytes);
// result is true if ipaddress (ipbytes) fits the mask (mask)


} // namespace ipf

#endif /* IPFBASEFNCL_H_ */