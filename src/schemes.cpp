#include "schemes.hpp"

template <>
sexp box(sexp&ctx, ipair& data) {
  return sexp_cons(ctx,
                   sexp_make_fixnum(data.first),
                   sexp_make_fixnum(data.second));
}

template <>
sexp box(sexp& ctx, string& data) {
  return sexp_c_string(ctx, data.c_str(), -1);
}

template <>
ipair unbox(sexp& data) {
  return ipair {
      sexp_unbox_fixnum(sexp_car(data)),
      sexp_unbox_fixnum(sexp_cdr(data))
      };
}

template <>
string unbox(sexp& data) {
  return string(sexp_string_data(data));
}

template <>
void embed_variable(sexp& ctx, string name, ipair& var) {
  sexp_env_define(ctx,
                  sexp_context_env(ctx),
                  sexp_string_to_symbol(ctx,
                                        sexp_c_string(ctx, name.c_str(), -1)),
                  box(ctx, var));
}
