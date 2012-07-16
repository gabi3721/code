#include "mruby_basis.h"
#include "data_store_ext.h"
#include "file_system_ext.h"
#include "file_ext.h"

#include <string>
#include <iostream>
#include <fstream>

using namespace std;
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[])
{
  auto load_script = [](char const* script) {
    ifstream ifs(script);
    string line, code;
    while (getline(ifs, line)) 
      code += line + "\n";

    return code;
  };

  auto mrb = mrb_open();

  init_data_store(mrb);
  init_file_system(mrb);
  init_file(mrb);

  auto code = load_script("myscript.rb");
  auto p = mrb_parse_string(mrb, code.c_str(), 0);
  auto n = mrb_generate_code(mrb, p->tree);
  mrb_run(mrb, mrb_proc_new(mrb, mrb->irep[n]), mrb_top_self(mrb));
  if (mrb->exc)
    mrb_p(mrb, mrb_obj_value(mrb->exc));

  mrb_close(mrb);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////