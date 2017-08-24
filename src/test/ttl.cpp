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

#include <thread>

#include <catch.hpp>

#include <cpp-cache/ttl-cache.h>

TEST_CASE("ttl", "[ttl]")
{
  using key_type = int;
  using value_type = std::string;
  const size_t ttl_max_age_ms = 200;

  const key_type one_key = 1;
  const value_type one_value = "one";
  const key_type two_key = 2;
  const value_type two_value = "two";
  const key_type three_key = 3;
  const value_type three_value = "three";
  const key_type four_key = 4;
  const value_type four_value = "four";

  using ttl_cache_t = cpp_cache::ttl_cache<key_type, value_type, ttl_max_age_ms>;
  ttl_cache_t ttl_cache;

  REQUIRE(ttl_cache.default_max_age() == ttl_max_age_ms);
  REQUIRE(ttl_cache.size() == 0);
  REQUIRE(ttl_cache.empty() == true);

  REQUIRE(ttl_cache.has(one_key) == false);
  REQUIRE(ttl_cache.has(two_key) == false);
  REQUIRE(ttl_cache.has(three_key) == false);
  REQUIRE(ttl_cache.has(four_key) == false);

  value_type tmp;
  REQUIRE(ttl_cache.try_get(one_key, tmp) == false);
  REQUIRE(ttl_cache.try_get(two_key, tmp) == false);
  REQUIRE(ttl_cache.try_get(three_key, tmp) == false);
  REQUIRE(ttl_cache.try_get(four_key, tmp) == false);

  try
  {
    ttl_cache.get(one_key);
    REQUIRE(false);
  }
  catch (std::out_of_range&) { REQUIRE(true); }
  catch (...) { REQUIRE(false); }

  ttl_cache.insert(one_key, one_value);
  REQUIRE(ttl_cache.has(one_key) == true);
  REQUIRE(ttl_cache.try_get(one_key, tmp) == true);
  REQUIRE(ttl_cache.get(one_key) == one_value);
  REQUIRE(ttl_cache.size() == 1);
  REQUIRE(ttl_cache.empty() == false);

  ttl_cache.insert(two_key, two_value);
  REQUIRE(ttl_cache.has(two_key) == true);
  REQUIRE(ttl_cache.try_get(two_key, tmp) == true);
  REQUIRE(ttl_cache.get(two_key) == two_value);
  REQUIRE(ttl_cache.size() == 2);
  REQUIRE(ttl_cache.empty() == false);
  REQUIRE(ttl_cache.has(one_key) == true);
  REQUIRE(ttl_cache.try_get(one_key, tmp) == true);
  REQUIRE(ttl_cache.get(one_key) == one_value);

  ttl_cache.erase(three_key);
  REQUIRE(ttl_cache.size() == 2);
  REQUIRE(ttl_cache.empty() == false);
  REQUIRE(ttl_cache.has(one_key) == true);
  REQUIRE(ttl_cache.has(two_key) == true);

  ttl_cache.erase(one_key);
  REQUIRE(ttl_cache.size() == 1);
  REQUIRE(ttl_cache.empty() == false);
  REQUIRE(ttl_cache.has(one_key) == false);
  REQUIRE(ttl_cache.has(two_key) == true);

  ttl_cache.clear();
  REQUIRE(ttl_cache.size() == 0);
  REQUIRE(ttl_cache.empty() == true);
  REQUIRE(ttl_cache.has(one_key) == false);
  REQUIRE(ttl_cache.has(two_key) == false);

  ttl_cache.insert(one_key, one_value);
  REQUIRE(ttl_cache.has(one_key) == true);

  ttl_cache.insert(two_key, two_value);
  REQUIRE(ttl_cache.has(two_key) == true);
  REQUIRE(ttl_cache.has(one_key) == true);

  ttl_cache.insert(three_key, three_value);
  REQUIRE(ttl_cache.has(three_key) == true);
  REQUIRE(ttl_cache.has(two_key) == true);
  REQUIRE(ttl_cache.has(one_key) == true);

  ttl_cache.insert(four_key, four_value);
  REQUIRE(ttl_cache.has(four_key) == true);
  REQUIRE(ttl_cache.has(three_key) == true);
  REQUIRE(ttl_cache.has(two_key) == true);
  REQUIRE(ttl_cache.has(one_key) == true);

  std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(2 * ttl_max_age_ms));

  REQUIRE(ttl_cache.has(four_key) == false);
  REQUIRE(ttl_cache.has(three_key) == false);
  REQUIRE(ttl_cache.has(two_key) == false);
  REQUIRE(ttl_cache.has(one_key) == false);

  ttl_cache.insert(one_key, one_value);
  REQUIRE(ttl_cache.has(one_key) == true);

  ttl_cache.insert(two_key, two_value);
  REQUIRE(ttl_cache.has(two_key) == true);
  REQUIRE(ttl_cache.has(one_key) == true);

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(0.5 * ttl_max_age_ms));

  REQUIRE(ttl_cache.has(one_key) == true);
  REQUIRE(ttl_cache.has(two_key) == true);

  REQUIRE(ttl_cache.get(one_key) == one_value);

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(0.7 * ttl_max_age_ms));

  REQUIRE(ttl_cache.has(one_key) == true);
  REQUIRE(ttl_cache.has(two_key) == false);

  ttl_cache.touch(one_key);

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(0.5 * ttl_max_age_ms));

  REQUIRE(ttl_cache.has(one_key) == true);

  std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(ttl_max_age_ms));

  REQUIRE(ttl_cache.has(one_key) == false);

  ttl_cache.insert(one_key, one_value, ttl_cache_t::duration_type(2 * ttl_max_age_ms));
  REQUIRE(ttl_cache.has(one_key) == true);

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1.5 * ttl_max_age_ms));

  REQUIRE(ttl_cache.has(one_key) == true);

  std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(ttl_max_age_ms));

  REQUIRE(ttl_cache.has(one_key) == false);
}
