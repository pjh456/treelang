/**
 * @file spell.hpp
 * @brief 术式数据定义（纯数据层）：承载元素等级表、基础伤害、咒力消耗、冷却回合，
 *        及属性融合工具。
 * @note 承载元素+等级存为按 Element 枚举值索引的 k_element_count 槽定长表（LevelTable），
 *       槽 >0 即承载该元素；集合判断一律走派生的 ElementMask，mask 不存储。
 *       战斗数值层/心流状态机对本结构只读，不反向依赖。
 */

#ifndef INCLUDE_TREELANG_COMBAT_SPELL_HPP
#define INCLUDE_TREELANG_COMBAT_SPELL_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "core/element.hpp"
#include "core/types.hpp"

namespace treelang
{
    /**
     * @brief 元素等级表：按 Element 枚举值索引，槽 >0 表示承载该元素。
     */
    using LevelTable = std::array<std::uint8_t, k_element_count>;

    /**
     * @brief 术式属性融合：逐槽等级相加（同名属性等级相加，README §5）。
     */
    inline LevelTable fuse_levels(const LevelTable &a, const LevelTable &b)
    {
        LevelTable out{};
        for (std::size_t i = 0; i < k_element_count; ++i)
            out[i] = static_cast<std::uint8_t>(a[i] + b[i]);
        return out;
    }

    /**
     * @class Spell
     * @brief 术式（技能）静态数据：战斗内只被读取，运行时状态
     *        （当前冷却剩余、心流参与等）由 combat 会话层自行维护。
     */
    struct Spell
    {
        std::string id;         /**< 术式 id（卡池内唯一） */
        int base_damage = 0;    /**< 基础伤害 */
        int mana_cost = 0;      /**< 咒力消耗 */
        int cooldown = 0;       /**< 冷却回合 */
        LevelTable levels = {}; /**< 承载元素+等级（>0 即承载） */

        /**
         * @brief 设置某元素等级；level = 0 表示清除该元素。
         */
        void set_level(Element e, int level)
        {
            levels[static_cast<std::size_t>(e)] = static_cast<std::uint8_t>(level);
        }

        /**
         * @brief 查询承载的属性等级；未承载该元素时为 None。
         */
        Option<int> level_of(Element e) const
        {
            const auto lv = levels[static_cast<std::size_t>(e)];
            return lv > 0 ? Option<int>::Some(static_cast<int>(lv)) : Option<int>::None();
        }

        /**
         * @brief 本术式元素集合位掩码（由等级表派生，不存储）。
         */
        ElementMask elements_mask() const
        {
            ElementMask m = ElementMask::None;
            for (std::size_t i = 0; i < k_element_count; ++i)
            {
                if (levels[i] > 0)
                    m |= mask_of(static_cast<Element>(i));
            }
            return m;
        }

        /**
         * @brief 传导属性位掩码（逻辑视图）：本术式承载元素中除触发元素外的全部。
         * @param trigger 本回合触发黑闪的元素；None 表示未触发，返回全部承载元素。
         * @note 心流延续判定简化为 `conduction_mask & 上一回合钥匙 !=
         * ElementMask::None`。
         */
        ElementMask conduction_mask(Option<Element> trigger) const
        {
            ElementMask m = elements_mask();
            if (trigger.is_some())
                m &= ~mask_of(trigger.unwrap());
            return m;
        }

        /**
         * @brief 传导属性（心流钥匙）元素列表（展示视图）：除触发元素外的全部承载元素，
         *        按枚举序返回；逻辑判定请用 conduction_mask。
         */
        std::vector<Element> conduction_elements(Option<Element> trigger) const
        {
            std::vector<Element> keys;
            keys.reserve(k_element_count);
            for (std::size_t i = 0; i < k_element_count; ++i)
            {
                if (levels[i] == 0)
                    continue;
                const Element e = static_cast<Element>(i);
                if (trigger.is_some() && e == trigger.unwrap())
                    continue;
                keys.push_back(e);
            }
            return keys;
        }
    };
}

#endif  // INCLUDE_TREELANG_COMBAT_SPELL_HPP
