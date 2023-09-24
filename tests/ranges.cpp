#include "gtest/gtest.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

TEST(cpp20, ranges1) {
    // Get even elements, drop first element, reverse elements

    std::vector<int> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto getEven = [](int i) { return i % 2 == 0; };

    std::vector<int> temp;
    std::copy_if(nums.begin(), nums.end(), std::back_inserter(temp), getEven);
    std::vector<int> temp2(temp.begin() + 1, temp.end());
    for (auto it = temp2.rbegin(); it != temp2.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    auto numsView = std::views::reverse(std::views::drop(std::views::filter(nums, getEven), 1));
    for (auto const &i : numsView) {
        std::cout << i << " ";
    }
    std::cout << "\n";

    std::vector<int> nums1(temp2.rbegin(), temp2.rend());
    std::vector<int> nums2(numsView.begin(), numsView.end());
    EXPECT_EQ(nums1, nums2);

    // Key Points
    // 1) Ranges: are an abstraction (generalization over iterator pairs begin(), end())
    // 2) Containers: (such as std::vector) own the elements
    // 3) Views: are ranges that don't own elements and are cheap to create, copy, and move.
    // 4) Lazy Evaluation: numsView doesn't do any work until we actually iterate through it.
}

TEST(cpp20, ranges2) {
    // Trim leading white space and transform to upper case
    std::string text("               Velo3d");
    std::string temp(text.begin() + text.find_first_not_of(' '), text.end());
    std::transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
    std::cout << std::quoted(temp) << "\n";

    auto textView = std::views::transform(std::views::drop_while(text, ::isspace), ::toupper);

    // Note still have to convert back to a string
    // C++23 will give us std::ranges::to<std::string>();
    std::string text2(textView.begin(), textView.end());
    std::cout << text2 << "\n";

    std::string text1(temp);
    EXPECT_EQ(text1, text2);

    auto text2View = text | std::views::drop_while(::isspace) | std::views::transform(::toupper);
    std::string text3(text2View.begin(), text2View.end());
    std::cout << text3 << "\n";
    EXPECT_EQ(text1, text3);
    EXPECT_EQ(text2, text3);
}

TEST(cpp20, ranges3) {
    // Iterate over keys and values
    std::map<int, char> data;
    for (auto i = 0; i < 5; ++i) {
        data[i] = 'a' + i;
    }
    std::cout << "Keys:     | ";
    for (auto const &key : data | std::views::keys) {
        std::cout << std::setw(9) << key << " | ";
    }
    std::cout << "\n"
                 "Values:   | ";
    for (auto const &value : data | std::views::values) {
        std::cout << std::setw(9) << value << " | ";
    }
    std::cout << "\n";
}

TEST(cpp20, ranges4) {
    // Iterate over keys and values
    const std::vector<std::tuple<std::string, double, bool>> quark_mass_charge{
        // name, MeV/c^2, has positive electric-charge:
        {"up", 2.3, true},      {"down", 4.8, false},   {"charm", 1275, true},
        {"strange", 95, false}, {"top", 173'210, true}, {"bottom", 4'180, false},
    };

    std::cout.imbue(std::locale("en_US.utf8"));
    std::cout << "Quark name:  |";
    for (std::string const &name : std::views::keys(quark_mass_charge))
        for (std::string const &name : quark_mass_charge | std::views::keys)
            std::cout << std::setw(9) << name << " | ";

    std::cout << "\n"
                 "Mass MeV/c^2: | ";
    // for (const double mass : std::views::values(quark_mass_charge))
    for (const double mass : quark_mass_charge | std::views::values)
        std::cout << std::setw(9) << mass << " | ";

    std::cout << "\n"
                 "E-charge:    | ";
    // for (const bool pos : std::views::elements<2>(quark_mass_charge))
    for (const bool pos : quark_mass_charge | std::views::elements<2>)
        std::cout << std::setw(9) << (pos ? "+2/3" : "-1/3") << " | ";

    std::cout << '\n';
}
