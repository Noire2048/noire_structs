#include "noire_structs.h"

VALUE rb_mNoireStructs;

void
Init_noire_structs(void)
{
  rb_mNoireStructs = rb_define_module("NoireStructs");
}
