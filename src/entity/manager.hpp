#ifndef INCLUDE_TREELANG_ENTITY_MANAGER_HPP
#define INCLUDE_TREELANG_ENTITY_MANAGER_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/event_bus.hpp"
#include "core/types.hpp"
#include "entity/base.hpp"
#include "entity/status.hpp"

namespace treelang
{
    /**
     * @class EntityManager
     * @brief 层作用域的实体容器 + id 索引。总线由拥有它的 Context 注入
     *        （须比本管理器存活更久），create 出的实体绑定该总线。
     *
     * 创建顺序 = 确定性迭代顺序（战斗回合序）；find 为 O(1)。
     * 不关心敌我关系与死亡规则：死实体由调用方在安静时刻（如战斗结束）
     * 显式 remove，避免事件分发中途改动容器。
     *
     * @note 不可拷贝、不可移动：须原地锚定在层运行时（Context）内。
     */
    class EntityManager
    {
    public:
        explicit EntityManager(EventBus &injected_bus) : bus(&injected_bus) {}

        EntityManager(const EntityManager &) = delete;
        EntityManager &operator=(const EntityManager &) = delete;
        EntityManager(EntityManager &&) = delete;
        EntityManager &operator=(EntityManager &&) = delete;

        /**
         * @brief 注册新实体（绑定到注入总线）。
         * @param id 实体 id，层内唯一。
         * @param status 初始属性。
         * @return Ok 时为实体指针（与 find 同稳定性：未 remove 前有效）；
         *         Err 时为 Error::EntityDuplicateId。
         */
        Result<entity::Entity *> create(std::string id, entity::StatusCollection status)
        {
            if (contains(id))
                return Result<entity::Entity *>::Err(Error::EntityDuplicateId);
            const std::size_t at = entities.size();
            entities.push_back(
                std::make_unique<entity::Entity>(id, std::move(status), *bus));
            index.emplace(std::move(id), at);
            return Result<entity::Entity *>::Ok(entities[at].get());
        }

        /** @brief O(1) 按 id 查询；不存在时为 None。 */
        Option<entity::Entity *> find(const std::string &id) const
        {
            auto it = index.find(id);
            if (it == index.end())
                return Option<entity::Entity *>::None();
            return Option<entity::Entity *>::Some(entities[it->second].get());
        }

        bool contains(const std::string &id) const
        {
            return index.find(id) != index.end();
        }

        /**
         * @brief 按 id 移除（不存在时幂等无操作）。
         * @note 其余实体的 Entity* 不受影响（堆对象不移动）。
         */
        void remove(const std::string &id)
        {
            auto it = index.find(id);
            if (it == index.end())
                return;
            entities.erase(entities.begin() + std::ptrdiff_t(it->second));
            rebuild_index();
        }

        std::size_t size() const noexcept { return entities.size(); }
        bool empty() const noexcept { return entities.empty(); }

        /** @brief 按创建序迭代（即战斗回合序）。 */
        auto begin() noexcept { return entities.begin(); }
        auto end() noexcept { return entities.end(); }
        auto begin() const noexcept { return entities.begin(); }
        auto end() const noexcept { return entities.end(); }

    private:
        EventBus *bus;
        std::vector<std::unique_ptr<entity::Entity>> entities;
        std::unordered_map<std::string, std::size_t> index;

        void rebuild_index()
        {
            index.clear();
            for (std::size_t i = 0; i < entities.size(); ++i)
                index.emplace(entities[i]->get_id(), i);
        }
    };
}

#endif  // INCLUDE_TREELANG_ENTITY_MANAGER_HPP
