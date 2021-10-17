#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <chibi/eval.h> 

#include "mode.hpp"

using std::string, std::pair;

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
sexp box(Data& data);

template <ipair>
sexp box(ipair& data) {
  return sexp_cons(ctx,
                   sexp_make_fixnum(var.first),
                   sexp_make_fixnum(var.second));
}

template <typename Data>
Data unbox(sexp& data);

template <ipair>
ipair unbox(sexp& data) {
  return ipair {
      sexp_unbox_fixnum(sexp_car(data)),
      sexp_unbox_fixnum(sexp_cdr(data))
      };
}

// turn a sexp into a transform, given a box/unbox implementation
template <typename Data>
function<Data(Data)> scheme_behavior(sexp& ctx, sexp& behavior) {

  return [](Data data) {
           retun unbox(sexp_apply(ctx, behavior, box(data)));
         };
}

template <typename Var>
void embed_variable(sexp& ctx, string name, Var& var);

template <ipair>
void embed_variable(sexp& ctx, string name, ipair& var) {
  sexp_env_define(ctx,
                  sexp_context_env(ctx),
                  sexp_string_to_symbol(ctx,
                                        sexp_c_string(ctx, name.c_str(), -1)),
                  box(var));
}

