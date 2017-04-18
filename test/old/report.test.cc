//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <chrono>
#include <thread>

#include "catch.hpp"

#include "../src/report.h"

using namespace libsemigroups;

// Classes for testing
class Class {
 public:
  Class() {}
};

class AnotherClass {
 public:
  AnotherClass() {}
};

TEST_CASE("Reporter: default report, 0 parameters, no call",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_ostream(os);
  r << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: no report, 0 parameters, no call",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(false);
  r.set_ostream(os);
  r << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: report, 0 parameters, no call",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r << str;
  REQUIRE(os->str() == str);
  delete os;
}

TEST_CASE("Reporter: default report, 0 parameters, call 1 parameter",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_ostream(os);
  r("function_name")
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: no report, 0 parameters, call 1 parameter",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(false);
  r.set_ostream(os);
  r("function_name")
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: report, 0 parameters, call 1 parameter",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r("function_name") << str;
  std::string prefix = "Thread #0: function_name: ";
  prefix.append(str);
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Nothing again";
  REQUIRE(os->str() == "Nothing again");
  delete os;
}

TEST_CASE("Reporter: default report, 0 parameters, call 2 parameters",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_ostream(os);
  r("function_name", 666)
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: no report, 0 parameters, call 2 parameters",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(false);
  r.set_ostream(os);
  r("function_name", 666)
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: report, 0 parameters, call 2 parameters",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r("function_name", 666) << str;
  std::string prefix = "Thread #666: function_name: ";
  prefix.append(str);
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Nothing again";
  REQUIRE(os->str() == "Nothing again");
  delete os;
}

TEST_CASE("Reporter: default report, 1 parameter, no call",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_ostream(os);
  r << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: no report, 1 parameter, no call",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(false);
  r.set_ostream(os);
  r << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: report, 1 parameter, no call", "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r << str;
  REQUIRE(os->str() == str);
  delete os;
}

TEST_CASE("Reporter: default report, 1 parameter, call 1 parameter",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_ostream(os);
  r("function_name")
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: no report, 1 parameter, call 1 parameter",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(false);
  r.set_ostream(os);
  r("function_name")
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: report, 1 parameter, call 1 parameter",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r("function_name") << str;
  std::string prefix = "Thread #0: Class::function_name: ";
  prefix.append(str);
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Something again";
  REQUIRE(os->str() == "Something again");
  delete os;
}

TEST_CASE("Reporter: default report, 1 parameter, call 2 parameters",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_ostream(os);
  r("function_name", 666)
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: no report, 1 parameter, call 2 parameters",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(false);
  r.set_ostream(os);
  r("function_name", 666)
      << "Nothing will happen, the string in the ostream will be empty";
  REQUIRE(os->str() == "");
  r << "Nothing again";
  REQUIRE(os->str() == "");
  delete os;
}

TEST_CASE("Reporter: report, 1 parameter, call 2 parameters",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r("function_name", 666) << str;
  std::string prefix = "Thread #666: Class::function_name: ";
  prefix.append(str);
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Something again";
  REQUIRE(os->str() == "Something again");
  delete os;
}

TEST_CASE("Reporter: << std::endl", "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);
  std::string str = "Something will happen, the string in the ostream will not "
                    "be empty";
  r("function_name", 666) << str << std::endl;
  std::string prefix = "Thread #666: Class::function_name: ";
  prefix.append(str);
  prefix.append("\n");
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Something again";
  REQUIRE(os->str() == "Something again");
  delete os;
}

TEST_CASE("Reporter: set_class_name", "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);
  AnotherClass ac = AnotherClass();
  r.set_class_name(ac);
  std::string str = "Something will happen!";
  r("function_name", 666) << str << std::endl;
  std::string prefix = "Thread #666: AnotherClass::function_name: ";
  prefix.append(str);
  prefix.append("\n");
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Something again";
  REQUIRE(os->str() == "Something again");

  os->str("");  // Clear the ostream
  r("function_name") << str << std::endl;
  prefix = "Thread #0: AnotherClass::function_name: ";
  prefix.append(str);
  prefix.append("\n");
  REQUIRE(os->str() == prefix);
  os->str("");  // Clear the ostream
  r << "Something again";
  REQUIRE(os->str() == "Something again");
  os->str("");  // Clear the ostream
  r("another_function_name") << str << std::endl;
  prefix = "Thread #0: AnotherClass::another_function_name: ";
  prefix.append(str);
  prefix.append("\n");
  REQUIRE(os->str() == prefix);
  delete os;
}

TEST_CASE("Reporter: report, 0 parameters, start/stop timer",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(true);
  r.set_ostream(os);

  r.stop_timer();  // does nothing
  REQUIRE(os->str() == "");

  r.start_timer();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  r.stop_timer();
  std::string prefix = "Thread #0: elapsed time = ";
  std::string result = os->str();
  REQUIRE(std::string(result.begin(), result.begin() + prefix.size())
          == prefix);

  os->str("");  // Clear the ostream

  r("function_name");
  r.start_timer();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  r.stop_timer();
  prefix = "Thread #0: function_name: elapsed time = ";
  result = os->str();
  REQUIRE(std::string(result.begin(), result.begin() + prefix.size())
          == prefix);

  os->str("");  // Clear the ostream

  r("function_name", 7);
  r.start_timer();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  r.stop_timer();
  prefix = "Thread #7: function_name: elapsed time = ";
  result = os->str();
  REQUIRE(std::string(result.begin(), result.begin() + prefix.size())
          == prefix);

  delete os;
}

TEST_CASE("Reporter: report, 1 parameter, start/stop timer",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c  = Class();
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);

  r.stop_timer();  // does nothing
  REQUIRE(os->str() == "");

  r.start_timer();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  r.stop_timer();
  std::string prefix = "Thread #0: Class: elapsed time = ";
  std::string result = os->str();
  REQUIRE(std::string(result.begin(), result.begin() + prefix.size())
          == prefix);

  os->str("");  // Clear the ostream

  r("function_name");
  r.start_timer();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  r.stop_timer();
  prefix = "Thread #0: Class::function_name: elapsed time = ";
  result = os->str();
  REQUIRE(std::string(result.begin(), result.begin() + prefix.size())
          == prefix);

  os->str("");  // Clear the ostream

  r("function_name", 7);
  r.start_timer();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  r.stop_timer();
  prefix = "Thread #7: Class::function_name: elapsed time = ";
  result = os->str();
  REQUIRE(std::string(result.begin(), result.begin() + prefix.size())
          == prefix);

  delete os;
}

TEST_CASE("Reporter: report, 0 parameters, threads",
          "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Reporter            r;
  r.set_report(true);
  r.set_ostream(os);
  size_t winner = -1;

  auto func = [&winner, &r](size_t wait_for_ms, size_t thread_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_for_ms));
    r.lock();
    r("func", thread_id) << "Something!";
    if (winner == static_cast<size_t>(-1)) {
      winner = thread_id;
    }
    r.unlock();
  };

  std::thread t0(func, 1, 0);
  std::thread t1(func, 1, 1);

  t0.join();
  t1.join();

  std::string expected;
  if (winner == 0) {
    expected = "Thread #0: func: Something!Thread #1: func: Something!";
  } else {
    expected = "Thread #1: func: Something!Thread #0: func: Something!";
  }

  REQUIRE(os->str() == expected);
  delete os;
}

TEST_CASE("Reporter: report, 1 parameter, threads", "[quick][util][reporter]") {
  std::ostringstream* os = new std::ostringstream();
  Class               c;
  Reporter            r(c);
  r.set_report(true);
  r.set_ostream(os);
  size_t winner = -1;

  auto func = [&winner, &r](size_t wait_for_ms, size_t thread_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_for_ms));
    r.lock();
    r("func", thread_id) << "Something!";
    if (winner == static_cast<size_t>(-1)) {
      winner = thread_id;
    }
    r.unlock();
  };

  std::thread t0(func, 1, 0);
  std::thread t1(func, 1, 1);

  t0.join();
  t1.join();

  std::string expected;
  if (winner == 0) {
    expected = "Thread #0: Class::func: Something!Thread #1: Class::func: "
               "Something!";
  } else {
    expected = "Thread #1: Class::func: Something!Thread #0: Class::func: "
               "Something!";
  }

  REQUIRE(os->str() == expected);
  delete os;
}
