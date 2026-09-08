/**
 * @file element.hpp
 * @brief 元素属性定义与元素工具函数。
 */

#ifndef INCLUDE_TREELANG_CORE_ELEMENT_HPP
#define INCLUDE_TREELANG_CORE_ELEMENT_HPP

#include <cstddef>
#include <cstdint>
#include <pjh_result/option.hpp>
#include <string_view>
#include <vector>

namespace treelang
{
    /**
     * @brief 元素属性。
     *
     * 元素之间不存在克制关系表；是否触发【黑闪】仅取决于敌人的弱点
     * 元素集合是否包含该元素。
     */
    enum class Element : std::uint8_t
    {
        Fire,  /**< 火 */
        Water, /**< 水 */
        Wind,  /**< 风 */
        Earth, /**< 土 */
        Light, /**< 光 */
    };

    /** @brief 元素个数（等级表槽位 / 掩码位数）。 */
    inline constexpr std::size_t k_element_count = 5;

    /**
     * @brief 元素集合位掩码：每个元素占一位，供纯「集合语义」的数据使用
     *        （敌人弱点、心流传导属性/钥匙等）；包含/交集/排除均为单位运算。
     * @note 每位对应 Element 枚举值（Fire=0 … Light=4）；~ 结果截断到 5 个元素位。
     */
    enum class ElementMask : std::uint8_t
    {
        None = 0,
        Fire = 1u << 0,
        Water = 1u << 1,
        Wind = 1u << 2,
        Earth = 1u << 3,
        Light = 1u << 4,
    };

    constexpr std::uint8_t mask_bits(ElementMask m) noexcept
    {
        return static_cast<std::uint8_t>(m);
    }

    constexpr ElementMask operator|(ElementMask a, ElementMask b) noexcept
    {
        return ElementMask(mask_bits(a) | mask_bits(b));
    }

    constexpr ElementMask operator&(ElementMask a, ElementMask b) noexcept
    {
        return ElementMask(mask_bits(a) & mask_bits(b));
    }

    /** @brief 按 5 个元素位取反（高位截断）。 */
    constexpr ElementMask operator~(ElementMask a) noexcept
    {
        return ElementMask(~mask_bits(a) & 0x1Fu);
    }

    /** @brief 全部五个元素。 */
    constexpr ElementMask all_elements() noexcept
    {
        return ElementMask::Fire | ElementMask::Water | ElementMask::Wind
             | ElementMask::Earth | ElementMask::Light;
    }

    constexpr ElementMask &operator|=(ElementMask &a, ElementMask b) noexcept
    {
        a = a | b;
        return a;
    }

    constexpr ElementMask &operator&=(ElementMask &a, ElementMask b) noexcept
    {
        a = a & b;
        return a;
    }

    /**
     * @brief 元素自身的单比特掩码。
     */
    constexpr ElementMask mask_of(Element e) noexcept
    {
        return ElementMask(1u << static_cast<int>(e));
    }

    /**
     * @brief 掩码展开为元素列表（按枚举序，供展示/迭代）。
     */
    inline std::vector<Element> elements_of(ElementMask m)
    {
        std::vector<Element> out;
        for (std::size_t i = 0; i < k_element_count; ++i)
        {
            if (mask_bits(m) & (1u << i))
                out.push_back(static_cast<Element>(i));
        }
        return out;
    }

    /**
     * @brief 获取元素在存档/配置中的稳定标识。
     *
     * 使用 ASCII 字符串，便于 JSON 序列化。
     *
     * @param e 元素。
     * @return 元素对应的 ASCII 标识，如 "fire"。
     */
    constexpr std::string_view element_id(Element e)
    {
        switch (e)
        {
        case Element::Fire:
            return "fire";
        case Element::Water:
            return "water";
        case Element::Wind:
            return "wind";
        case Element::Earth:
            return "earth";
        case Element::Light:
            return "light";
        }
        return {};
    }

    /**
     * @brief 从稳定标识解析元素。
     * @param id ASCII 标识，如 "water"。
     * @return 解析成功返回对应元素；未知标识返回 None。
     */
    inline pjh::result::Option<Element> element_from_id(std::string_view id)
    {
        using Opt = pjh::result::Option<Element>;
        if (id == "fire")
            return Opt::Some(Element::Fire);
        if (id == "water")
            return Opt::Some(Element::Water);
        if (id == "wind")
            return Opt::Some(Element::Wind);
        if (id == "earth")
            return Opt::Some(Element::Earth);
        if (id == "light")
            return Opt::Some(Element::Light);
        return Opt::None();
    }

    /**
     * @brief 获取元素的显示名（中文）。
     * @param e 元素。
     * @return 元素的显示名，如 "火"。
     */
    constexpr std::string_view element_name(Element e)
    {
        switch (e)
        {
        case Element::Fire:
            return "火";
        case Element::Water:
            return "水";
        case Element::Wind:
            return "风";
        case Element::Earth:
            return "土";
        case Element::Light:
            return "光";
        }
        return {};
    }
}

#endif  // INCLUDE_TREELANG_CORE_ELEMENT_HPP