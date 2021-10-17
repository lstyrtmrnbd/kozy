#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <chibi/eval.h> 

#include "mode.hpp"

using std::vector, std::string, std::pair;

/**
 * The Naive Mode
 * - it's all schemes
 */

using ipair = pair<int, int>;

// the atomic physical unit, some data and a transform
template <typename Data>
struct Physical {
  Data data;
  function<Data(Data)>& behavior;
};

template <typename Data>
sexp box(sexp& ctx, Data& data);

template <>
sexp box(sexp&ctx, ipair& data) {
  return sexp_cons(ctx,
                   sexp_make_fixnum(data.first),
                   sexp_make_fixnum(data.second));
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

// turn a sexp into a transform, given a box/unbox implementation
template <typename Data>
function<Data(Data)> scheme_behavior(sexp& ctx, sexp& behavior) {

  return [&](Data data) {
           return unbox(sexp_apply(ctx, behavior, box(ctx, data)));
         };
}

template <typename Var>
void embed_variable(sexp& ctx, string name, Var& var);

template <>
void embed_variable(sexp& ctx, string name, ipair& var) {
  sexp_env_define(ctx,
                  sexp_context_env(ctx),
                  sexp_string_to_symbol(ctx,
                                        sexp_c_string(ctx, name.c_str(), -1)),
                  box(ctx, var));
}


using naive = Physical<vector<ipair>>;

// Mode<naive> make_naive_mode() {

// }
