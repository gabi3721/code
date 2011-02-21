#include <boost/spirit/include/qi.hpp>

#include <vector>
#include <iostream>
#include <numeric>

namespace client
{
  namespace qi    = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  template <typename It> 
  bool parse_numbers(It first, It last, std::vector<double>& v)
  {
    using qi::double_;
    using qi::phrase_parse;

    bool r = phrase_parse(first, last, (double_ % ','), ascii::space, v);

    return (first != last) ? false : r;
  }
}

int main(int argc, char const* argv[])
{
  std::string s = "1, 2, 3.3, 4, 5.56";
  std::vector<double> v; 
  client::parse_numbers(s.begin(), s.end(), v);

  std::cout << std::accumulate(v.begin(), v.end(), 0.0);

  return 0;
}