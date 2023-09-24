#include "gtest/gtest.h"
#include <coroutine>
#include <format>
#include <iostream>

// coroutines are functions that can invoke each other but do not share a stack
// so can suspend their execution at any point to enter a different coroutine

// co_await
// 1) ensures local variables in the current function -- which must be a coroutine
//    are saved to a heap-allocated object
// 2) Creates a callable object that when invoked, will resume execution of the coroutine at the point immediately
// following evaluation of the co_await expression 3) Calls (or jumps) to a method of co_await's target object calling
// object in (2)

struct ReturnObject {
    struct promise_type {
        ReturnObject get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_void() noexcept {}
    };
};

struct Awaiter {
    std::coroutine_handle<> *hp_;
    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) { *hp_ = h; }
    constexpr void await_resume() const noexcept {}
};

ReturnObject counter(std::coroutine_handle<> *continuationOut) {
    Awaiter a{continuationOut};
    for (auto i = 0;; ++i) {
        co_await a;
        std::cout << std::format("counter: {}\n", i);
    }
}

TEST(cpp20, coroutine1) {
     std::coroutine_handle<> h;
     counter(&h);
     for (auto i = 0; i < 10; ++i) {
         std::cout << "In test function\n";
         h();
     }
     h.destroy();
}

struct ReturnObject2 {
    struct promise_type {
        ReturnObject2 get_return_object() { return {._h = std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_void() noexcept {}
    };
    std::coroutine_handle<promise_type> _h;
    operator std::coroutine_handle<promise_type>() const { return _h; }
    // A coroutine_handle<promise_type> converts to coroutine_handle<>
    operator std::coroutine_handle<>() const { return _h; }
};

ReturnObject2 counter2() {
    for (unsigned i = 0;; ++i) {
        co_await std::suspend_always{};
        std::cout << "counter2: " << i << std::endl;
    }
}

TEST(cpp20, coroutine2) {
    std::coroutine_handle<> h = counter2();
    for (int i = 0; i < 10; ++i) {
        std::cout << "In main function\n";
        h();
    }
    h.destroy();
}