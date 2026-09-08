#include <doctest/doctest.h>

#include "core/element.hpp"
#include "core/rng.hpp"
#include "core/types.hpp"

using treelang::Element;
using treelang::ElementAttr;
using treelang::ElementMask;
using treelang::Rng;
using treelang::RoomType;
using treelang::all_elements;
using treelang::elements_of;
using treelang::mask_of;

TEST_CASE("core: element id/name round-trip")
{
    CHECK(treelang::element_id(Element::Fire) == "fire");
    CHECK(treelang::element_id(Element::Water) == "water");
    CHECK(treelang::element_id(Element::Wind) == "wind");
    CHECK(treelang::element_id(Element::Earth) == "earth");
    CHECK(treelang::element_id(Element::Light) == "light");

    CHECK(treelang::element_name(Element::Fire) == "火");
    CHECK(treelang::element_name(Element::Water) == "水");
    CHECK(treelang::element_name(Element::Wind) == "风");
    CHECK(treelang::element_name(Element::Earth) == "土");
    CHECK(treelang::element_name(Element::Light) == "光");

    CHECK(treelang::element_from_id("fire").contains(Element::Fire));
    CHECK(treelang::element_from_id("water").contains(Element::Water));
    CHECK(treelang::element_from_id("wind").contains(Element::Wind));
    CHECK(treelang::element_from_id("earth").contains(Element::Earth));
    CHECK(treelang::element_from_id("light").contains(Element::Light));
    CHECK(treelang::element_from_id("metal").is_none());
}

TEST_CASE("core: room type / constants / element attr")
{
    CHECK(treelang::k_map_size == 5);

    ElementAttr attr{Element::Fire, 3};
    CHECK(attr.element == Element::Fire);
    CHECK(attr.level == 3);

    CHECK(static_cast<int>(RoomType::Start) == 0);
    CHECK(static_cast<int>(RoomType::Story) == 4);
}

TEST_CASE("core: element mask bit ops")
{
    CHECK(mask_of(Element::Fire) == ElementMask::Fire);
    CHECK(mask_of(Element::Light) == ElementMask::Light);

    const auto both = ElementMask::Fire | ElementMask::Water;
    CHECK((both & ElementMask::Fire) != ElementMask::None);
    CHECK((both & ElementMask::Light) == ElementMask::None);
    CHECK((both | ElementMask::Wind)
          == (ElementMask::Fire | ElementMask::Water | ElementMask::Wind));

    CHECK(~ElementMask::Fire
          == (ElementMask::Water | ElementMask::Wind | ElementMask::Earth | ElementMask::Light));
    CHECK(~ElementMask::None == all_elements());
    CHECK(~all_elements() == ElementMask::None);

    ElementMask m = ElementMask::None;
    m |= mask_of(Element::Earth);
    m &= ElementMask::Earth;
    CHECK(m == ElementMask::Earth);
    m &= ~ElementMask::Earth;
    CHECK(m == ElementMask::None);
}

TEST_CASE("core: elements_of expands in enum order")
{
    const auto els = elements_of(ElementMask::Light | ElementMask::Fire);
    CHECK(els.size() == 2);
    CHECK(els[0] == Element::Fire);
    CHECK(els[1] == Element::Light);

    CHECK(elements_of(ElementMask::None).empty());
    CHECK(elements_of(all_elements()).size() == 5);
}

TEST_CASE("core: rng is deterministic and in-range")
{
    Rng a{42};
    Rng b{42};
    for (int i = 0; i < 100; ++i) CHECK(a() == b());

    for (int i = 0; i < 1000; ++i)
    {
        int v = a.uniform_int(1, 6);
        CHECK(v >= 1);
        CHECK(v <= 6);

        double r = a.uniform_real(0.0, 1.0);
        CHECK(r >= 0.0);
        CHECK(r < 1.0);
    }

    CHECK(a.uniform_int(5, 5) == 5);
    CHECK(a.chance(1.0));
    CHECK(!a.chance(0.0));
}