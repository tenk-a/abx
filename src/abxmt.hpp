/**
 *  @file   abxmt.hpp
 *  @brief  usage for abx
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license    Boost Software License Version 1.0
 *  add -xm multi thread version by misakichi (https://github.com/misakichi)
 */
#ifndef ABXMT_HPP
#define ABXMT_HPP

#include <vector>
#include <string>

void mtCmd(std::vector<std::string>& outBuf, unsigned threads);

#endif
