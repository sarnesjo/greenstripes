#include <ruby.h>

static VALUE module_greenstripes;

void Init_greenstripes()
{
  module_greenstripes = rb_define_module("GreenStripes");
}
