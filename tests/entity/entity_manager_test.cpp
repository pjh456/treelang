#include <doctest/doctest.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "core/error.hpp"
#include "core/event_bus.hpp"
#include "core/handler.hpp"
#include "entity/base.hpp"
#include "entity/event.hpp"
#include "entity/manager.hpp"
#include "entity/status.hpp"

using treelang::EntityManager;
using treelang::EntityStatusChangedEvent;
using treelang::Error;
using treelang::EventBus;
using treelang::Handler;
using treelang::HandlerContext;

namespace entity = treelang::entity;

namespace
{
    entity::StatusCollection make_status(int hp)
    {
        entity::StatusCollection st;
        st.get_hp() = entity::SingleStatus(hp);
        st.get_atk() = entity::SingleStatus(5);
        st.get_def() = entity::SingleStatus(3);
        return st;
    }
}

TEST_CASE("manager: create + find return the same entity")
{
    EventBus bus;  // 测试里 bus 由用例持有，扮演 Context 的角色
    EntityManager m(bus);

    auto r = m.create("a", make_status(20));
    REQUIRE(r.is_ok());
    auto *a = r.unwrap();

    auto found = m.find("a");
    REQUIRE(found.is_some());
    CHECK(found.unwrap() == a);

    a->get_status().get_hp().set_cur(7);
    CHECK(found.unwrap()->get_status().get_hp().get_cur() == 7);
}

TEST_CASE("manager: find/contains on unknown id are empty")
{
    EventBus bus;
    EntityManager m(bus);
    CHECK(m.empty());
    CHECK(m.size() == 0);
    CHECK_FALSE(m.contains("ghost"));
    CHECK_FALSE(m.find("ghost").is_some());
    m.remove("ghost");  // 幂等无操作
    CHECK(m.empty());
}

TEST_CASE("manager: duplicate id returns Err and leaves the manager unchanged")
{
    EventBus bus;
    EntityManager m(bus);

    auto r = m.create("a", make_status(10));
    REQUIRE(r.is_ok());
    auto *a = r.unwrap();

    auto dup = m.create("a", make_status(1));
    CHECK(dup.is_err());
    CHECK(dup.unwrap_err() == Error::EntityDuplicateId);
    CHECK(m.size() == 1);
    CHECK(m.find("a").unwrap() == a);
    CHECK(a->get_status().get_hp().get_cur() == 10);
}

TEST_CASE("manager: iteration order is creation order")
{
    EventBus bus;
    EntityManager m(bus);
    REQUIRE(m.create("c", make_status(1)).is_ok());
    REQUIRE(m.create("a", make_status(2)).is_ok());
    REQUIRE(m.create("b", make_status(3)).is_ok());

    std::vector<std::string> order;
    for (auto &up : m)
        order.push_back(up->get_id());

    CHECK(order == std::vector<std::string>{ "c", "a", "b" });
}

TEST_CASE("manager: remove keeps other entity pointers valid")
{
    EventBus bus;
    EntityManager m(bus);
    REQUIRE(m.create("a", make_status(10)).is_ok());
    auto rb = m.create("b", make_status(20));
    REQUIRE(rb.is_ok());
    auto *b = rb.unwrap();

    m.remove("a");
    CHECK_FALSE(m.contains("a"));
    CHECK_FALSE(m.find("a").is_some());
    CHECK(m.size() == 1);
    CHECK(b->get_status().get_hp().get_cur() == 20);  // b 的指针依然有效
    CHECK(m.find("b").unwrap() == b);
}

TEST_CASE("manager: created entities publish on the injected bus")
{
    EventBus bus;
    std::vector<std::uint64_t> seqs;
    auto h = bus.subscribe(Handler<EntityStatusChangedEvent>(
        [&](HandlerContext<EntityStatusChangedEvent> &ctx) { seqs.push_back(ctx.event.get_sequence()); }));

    EntityManager m(bus);
    auto r = m.create("a", make_status(10));
    REQUIRE(r.is_ok());
    r.unwrap()->get_status().get_hp().sub(4);

    CHECK(seqs.size() == 1);  // 实体的状态变化流入注入的总线
}

TEST_CASE("manager: combat-scoped handler unsubscribes with its handle")
{
    EventBus bus;
    int ticks = 0;
    EntityManager m(bus);
    auto r = m.create("a", make_status(10));
    REQUIRE(r.is_ok());
    auto *a = r.unwrap();

    {
        // 「战斗会话」：handler 注册在 Context 的总线上，句柄随会话生命周期
        auto h = bus.subscribe(Handler<EntityStatusChangedEvent>(
            [&](HandlerContext<EntityStatusChangedEvent> &) { ++ticks; }));
        a->get_status().get_hp().sub(1);
    }  // 「战斗结束」：句柄析构 → 监听卸载

    a->get_status().get_hp().sub(1);
    CHECK(ticks == 1);
}
