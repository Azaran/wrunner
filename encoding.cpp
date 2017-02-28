/* 
 * Copyright (C) 2016 Radek Hranicky
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 */

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include "encoding.h"

std::string uint8_t_base64_encode(uint8_t * start, int len) {
    std::string input((const char *)start, (size_t)len);
    return encode64(input);
}

void uint8_t_base64_decode(std::string input, uint8_t * output, int maxlen) {
    std::string sout = decode64(input);
    int len = (int)sout.size();
    if ((int)sout.size() > maxlen) {
        std::cout << "WARN!!!" << std::endl;
        len = maxlen;
    }
    ::memcpy(output, sout.c_str(), len);
}

std::string encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}

std::string decode64(const std::string& s) {
  namespace bai = boost::archive::iterators;
  std::stringstream os;
  typedef bai::transform_width<bai::binary_from_base64<const char *>, 8, 6> base64_dec;
  unsigned int size = s.size();
  if (size && s[size - 1] == '=') {
    --size;
    if (size && s[size - 1] == '=') --size;
  }
  if (size == 0) return std::string();
  std::copy(base64_dec(s.data()), base64_dec(s.data() + size),
            std::ostream_iterator<char>(os));

  return os.str();
}

/*
std::string decode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
        return c == '\0';
    });
}
*/
