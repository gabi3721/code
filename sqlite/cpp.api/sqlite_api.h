#include <string>
#include <vector>
#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
class SQLiteApi
{
public:
  SQLiteApi(char const* path);
 ~SQLiteApi();

  auto open() const -> bool;

public:
  auto schema(char const* ofTable) const -> std::string;
  auto table_list() const -> std::vector<std::string>;
  auto table_info(char const* name) const -> std::map<std::string, std::string>;

private:
  class impl; std::unique_ptr<impl> pimpl;
};

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
