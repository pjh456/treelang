#include <doctest/doctest.h>

#include <string>
#include <vector>

#include "combat/spell.hpp"

using treelang::Element;
using treelang::ElementMask;
using treelang::LevelTable;
using treelang::Option;
using treelang::Spell;
using treelang::fuse_levels;

namespace
{
    Spell make_spell()
    {
        Spell sp;
        sp.id = "fire_storm";
        sp.set_level(Element::Fire, 2);
        sp.set_level(Element::Wind, 1);
        sp.base_damage = 20;
        sp.mana_cost = 15;
        sp.cooldown = 2;
        return sp;
    }
}

TEST_CASE("spell: 默认构造全零/空")
{
    Spell sp;
    CHECK(sp.id.empty());
    CHECK(sp.base_damage == 0);
    CHECK(sp.mana_cost == 0);
    CHECK(sp.cooldown == 0);
    CHECK(sp.elements_mask() == ElementMask::None);
}

TEST_CASE("spell: level_of 命中返回等级，未承载返回 None")
{
    const Spell sp = make_spell();

    CHECK(sp.level_of(Element::Fire).is_some());
    CHECK(sp.level_of(Element::Fire).unwrap() == 2);
    CHECK(sp.level_of(Element::Wind).unwrap() == 1);
    CHECK(sp.level_of(Element::Water).is_none());
    CHECK(sp.level_of(Element::Light).is_none());
}

TEST_CASE("spell: set_level 0 清除该元素")
{
    Spell sp = make_spell();
    sp.set_level(Element::Fire, 0);

    CHECK(sp.level_of(Element::Fire).is_none());
    CHECK(sp.elements_mask() == ElementMask::Wind);
}

TEST_CASE("spell: elements_mask 为等级表派生的集合视图")
{
    CHECK(make_spell().elements_mask() == (ElementMask::Fire | ElementMask::Wind));
}

TEST_CASE("spell: conduction_mask 排除触发元素，None 为全部承载")
{
    const Spell sp = make_spell();

    CHECK(sp.conduction_mask(Option<Element>::Some(Element::Fire)) == ElementMask::Wind);
    CHECK(sp.conduction_mask(Option<Element>::Some(Element::Light))
          == (ElementMask::Fire | ElementMask::Wind));
    CHECK(sp.conduction_mask(Option<Element>::None()) == (ElementMask::Fire | ElementMask::Wind));
}

TEST_CASE("spell: conduction_elements 展示视图按枚举序返回")
{
    const Spell sp = make_spell();

    const auto keys = sp.conduction_elements(Option<Element>::Some(Element::Fire));
    CHECK(keys.size() == 1);
    CHECK(keys[0] == Element::Wind);

    const auto all = sp.conduction_elements(Option<Element>::None());
    CHECK(all.size() == 2);
    CHECK(all[0] == Element::Fire);
    CHECK(all[1] == Element::Wind);
}

TEST_CASE("spell: 心流延续判定 = 相邻两回合 conduction_mask 交集非空")
{
    Spell prev;
    prev.set_level(Element::Fire, 1);
    prev.set_level(Element::Water, 1);
    Spell next;
    next.set_level(Element::Water, 2);
    next.set_level(Element::Earth, 1);
    Spell disjoint;
    disjoint.set_level(Element::Light, 1);

    const auto keys = prev.conduction_mask(Option<Element>::Some(Element::Fire));
    CHECK(keys == ElementMask::Water);

    CHECK((next.conduction_mask(Option<Element>::None()) & keys) == ElementMask::Water);
    CHECK((disjoint.conduction_mask(Option<Element>::None()) & keys) == ElementMask::None);
}

TEST_CASE("fuse: 逐槽等级相加（同名相加、互不覆盖）")
{
    Spell a;
    a.set_level(Element::Fire, 1);
    a.set_level(Element::Water, 1);
    Spell b;
    b.set_level(Element::Fire, 2);
    b.set_level(Element::Wind, 1);

    const auto out = fuse_levels(a.levels, b.levels);
    CHECK(out[static_cast<int>(Element::Fire)] == 3);
    CHECK(out[static_cast<int>(Element::Water)] == 1);
    CHECK(out[static_cast<int>(Element::Wind)] == 1);
    CHECK(out[static_cast<int>(Element::Earth)] == 0);
    CHECK(out[static_cast<int>(Element::Light)] == 0);
}

TEST_CASE("fuse: 任一侧为空时退化为另一侧")
{
    Spell empty;
    const Spell sp = make_spell();

    CHECK(fuse_levels(empty.levels, sp.levels) == sp.levels);
    CHECK(fuse_levels(sp.levels, empty.levels) == sp.levels);
}
