#pragma once

#include <functional>
#include <string>
#include <utility>

#include <chibi/eval.h> 

using std::string, std::pair, std::function;
using ipair = pair<int, int>;

//// Scheme interaction layer

template <typename Data>
sexp box(sexp& ctx, Data& data);
template <>
sexp box(sexp&ctx, ipair& data);
template <>
sexp box(sexp& ctx, string& data);

template <typename Data>
Data unbox(sexp& data);
template <>
ipair unbox(sexp& data);
template <>
string unbox(sexp& data);

// turn a sexp into a transform,
// given a box/unbox implementation for the Data type
template <typename Data>
function<Data(Data)> scheme_behavior(sexp& ctx, sexp& behavior) {

  return [&](Data data) {
           // not gonna work unless you handle R-values?
           return unbox(sexp_apply(ctx, behavior, box(ctx, data)));
         };
}

template <typename Data>
void embed_variable(sexp& ctx, string name, Data& var);

template <>
void embed_variable(sexp& ctx, string name, ipair& var);
