/**
 * @file error.hpp
 * @brief 统一错误值。各模块失败时在 Result 的 E 槽位返回对应变体。
 */
#ifndef INCLUDE_TREELANG_CORE_ERROR_HPP
#define INCLUDE_TREELANG_CORE_ERROR_HPP

#include <cstdint>

namespace treelang
{
    /**
     * @brief 统一错误值（按域前缀命名变体）。
     * @note 需要携带上下文（如重复的 id）时，模块内部可先用带字段的错误
     *       结构，再在边界收敛为本枚举的变体。
     */
    enum class Error : std::uint8_t
    {
        EntityDuplicateId,  /**< 层内已存在相同 id 的实体 */
    };
}

#endif  // INCLUDE_TREELANG_CORE_ERROR_HPP
