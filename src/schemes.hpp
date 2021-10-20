#pragma once

#include <string>
#include <utility>

#include <chibi/eval.h> 

using std::string, std::pair;
using ipair = pair<int, int>;

//// Scheme interaction layer

template <typename Data>
sexp box(sexp& ctx, Data& data);

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

template <typename Data>
Data unbox(sexp& data);

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

// turn a sexp into a transform,
// given a box/unbox implementation for the Data type
template <typename Data>
function<Data(Data)> scheme_behavior(sexp& ctx, sexp& behavior) {

  return [&](Data data) {
           // not gonna work unless you handle R-values
           return unbox(sexp_apply(ctx, behavior, box(ctx, data)));
         };
}

template <typename Data>
void embed_variable(sexp& ctx, string name, Data& var);

template <>
void embed_variable(sexp& ctx, string name, ipair& var) {
  sexp_env_define(ctx,
                  sexp_context_env(ctx),
                  sexp_string_to_symbol(ctx,
                                        sexp_c_string(ctx, name.c_str(), -1)),
                  box(ctx, var));
}
