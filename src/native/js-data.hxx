
#pragma once

#include "common.hxx"

// JS Array -> std::vector<T>
template <typename T, typename Convert>
std::vector<T> toVector(const Array &array, Convert &&convert);

// std::vector<T> -> JS Array
template <typename T, typename Convert>
Array fromVector(const Env &env, const std::vector<T> &values, Convert &&convert);

std::vector<std::string> toStringVector(const Array &array);
Array fromStringVector(const Env &env, const std::vector<std::string> &strings);

Object fromPackage(const Env &env, const libdnf5::rpm::Package &pkg);
