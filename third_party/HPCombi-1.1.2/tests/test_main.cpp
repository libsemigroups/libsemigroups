//****************************************************************************//
//    Copyright (C) 2023-2024 James D. Mitchell <jdm3@st-andrews.ac.uk>       //
//    Copyright (C) 2023-2024 Florent Hivert <Florent.Hivert@lisn.fr>,        //
//                                                                            //
//  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  //
//                                                                            //
//  HP-Combi is free software: you can redistribute it and/or modify it       //
//  under the terms of the GNU General Public License as published by the     //
//  Free Software Foundation, either version 3 of the License, or             //
//  (at your option) any later version.                                       //
//                                                                            //
//  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      //
//  for  more details.                                                        //
//                                                                            //
//  You should have received a copy of the GNU General Public License along   //
//  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               //
//****************************************************************************//

#include <iostream>

#include <catch2/catch_test_case_info.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

struct HPCombiListener : Catch::EventListenerBase {
    using EventListenerBase::EventListenerBase;  // inherit constructor

    void testCaseStarting(Catch::TestCaseInfo const &testInfo) override {
        std::cout << testInfo.tagsAsString() << " " << testInfo.name
                  << std::endl;
    }
    void testCaseEnded(Catch::TestCaseStats const &) override {}
    void sectionStarting(Catch::SectionInfo const &) override {}
    void sectionEnded(Catch::SectionStats const &) override {}
    void testCasePartialStarting(Catch::TestCaseInfo const &,
                                 uint64_t) override {}
    void testCasePartialEnded(Catch::TestCaseStats const &, uint64_t) override {
    }
};

CATCH_REGISTER_LISTENER(HPCombiListener)
