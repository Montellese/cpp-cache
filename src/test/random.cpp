/*
 *  Copyright (C) Sascha Montellese
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with cpp-signals; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <catch.hpp>

#include <cpp-cache/random-cache.h>

TEST_CASE("random", "[random]")
{
  using key_type = int;
  using value_type = std::string;
  const size_t cache_size = 2;

  const key_type one_key = 1;
  const value_type one_value = "one";
  const key_type two_key = 2;
  const value_type two_value = "two";
  const key_type three_key = 3;
  const value_type three_value = "three";
  const key_type four_key = 4;
  const value_type four_value = "four";

  cpp_cache::random_cache<key_type, value_type, cache_size> random_cache;

  REQUIRE(random_cache.max_size() == cache_size);
  REQUIRE(random_cache.size() == 0);
  REQUIRE(random_cache.empty() == true);

  REQUIRE(random_cache.has(one_key) == false);
  REQUIRE(random_cache.has(two_key) == false);
  REQUIRE(random_cache.has(three_key) == false);
  REQUIRE(random_cache.has(four_key) == false);

  value_type tmp;
  REQUIRE(random_cache.try_get(one_key, tmp) == false);
  REQUIRE(random_cache.try_get(two_key, tmp) == false);
  REQUIRE(random_cache.try_get(three_key, tmp) == false);
  REQUIRE(random_cache.try_get(four_key, tmp) == false);

  try
  {
    random_cache.get(one_key);
    REQUIRE(false);
  }
  catch (std::out_of_range&) { REQUIRE(true); }
  catch (...) { REQUIRE(false); }

  random_cache.insert(one_key, one_value);
  REQUIRE(random_cache.has(one_key) == true);
  REQUIRE(random_cache.try_get(one_key, tmp) == true);
  REQUIRE(random_cache.get(one_key) == one_value);
  REQUIRE(random_cache.size() == 1);
  REQUIRE(random_cache.empty() == false);

  random_cache.insert(two_key, two_value);
  REQUIRE(random_cache.has(two_key) == true);
  REQUIRE(random_cache.try_get(two_key, tmp) == true);
  REQUIRE(random_cache.get(two_key) == two_value);
  REQUIRE(random_cache.size() == 2);
  REQUIRE(random_cache.empty() == false);
  REQUIRE(random_cache.has(one_key) == true);
  REQUIRE(random_cache.try_get(one_key, tmp) == true);
  REQUIRE(random_cache.get(one_key) == one_value);

  random_cache.erase(three_key);
  REQUIRE(random_cache.size() == 2);
  REQUIRE(random_cache.empty() == false);
  REQUIRE(random_cache.has(one_key) == true);
  REQUIRE(random_cache.has(two_key) == true);

  random_cache.erase(one_key);
  REQUIRE(random_cache.size() == 1);
  REQUIRE(random_cache.empty() == false);
  REQUIRE(random_cache.has(one_key) == false);
  REQUIRE(random_cache.has(two_key) == true);

  random_cache.clear();
  REQUIRE(random_cache.size() == 0);
  REQUIRE(random_cache.empty() == true);
  REQUIRE(random_cache.has(one_key) == false);
  REQUIRE(random_cache.has(two_key) == false);

  random_cache.insert(one_key, one_value);
  REQUIRE(random_cache.has(one_key) == true);

  random_cache.insert(two_key, two_value);
  REQUIRE(random_cache.has(two_key) == true);
  REQUIRE(random_cache.has(one_key) == true);

  random_cache.insert(three_key, three_value);
  REQUIRE(random_cache.has(three_key) == true);
  REQUIRE((random_cache.has(one_key) == true) != (random_cache.has(two_key) == true));

  random_cache.insert(four_key, four_value);
  REQUIRE(random_cache.has(four_key) == true);
  REQUIRE(((random_cache.has(one_key) == true) != (random_cache.has(two_key) == true)) != (random_cache.has(three_key) == true));
}
