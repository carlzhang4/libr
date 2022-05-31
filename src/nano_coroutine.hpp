#ifndef _NANO_COROUTINE_HPP_
#define _NANO_COROUTINE_HPP_

#define MAX_COROUTINE_NUMBER            (10)

#include <coroutine>
#include <utility>
#include <thread>
#include <iostream>
namespace nano_coroutine {
    class [[nodiscard]] AsyncTask  {
        protected:
            struct Promise {
                auto get_return_object() noexcept {
                    return AsyncTask(*this);
                }
                constexpr void return_void() const noexcept {}
                constexpr void unhandled_exception() const noexcept {}
                // default suspend when initialize coroutine
                constexpr auto initial_suspend() const noexcept { return std::suspend_always(); }
                // default suspend after coroutine finished
                constexpr auto final_suspend() const noexcept {return std::suspend_always(); }
            };
            std::coroutine_handle<Promise> handler;
            explicit AsyncTask(Promise &p) noexcept : handler{std::coroutine_handle<Promise>::from_promise(p)} {}
        public:
            using promise_type = Promise;
            // Disable Copy Semantics
            AsyncTask(AsyncTask& t) = delete;
            void operator=(AsyncTask& t) = delete;
            // Enable Move Semantics
            AsyncTask(AsyncTask&& t) noexcept : handler{std::exchange(t.handler, {})} {}
            void operator=(AsyncTask&& t) noexcept {
                this->handler = std::exchange(t.handler, {});
            }
            // Default Constructor
            AsyncTask() noexcept : handler{} {}
            ~AsyncTask() {
                if (handler) {
                    handler.destroy();
                }
            }
            // Resume
            bool resume() {
                if (!handler.done()) {
                    handler.resume();
                }
                return !handler.done();
            }
            // Get Handler, Used for Symmetric Transfer
            inline std::coroutine_handle<Promise>* getHandler() noexcept {
                return &handler;
            }
            // Update Handler, Used for Symmetric Transfer
            inline void updateHandler(std::coroutine_handle<Promise>& h) {
                handler = std::exchange(h, {});
            }
    };

    thread_local int32_t readyIndex{};
    thread_local AsyncTask taskPool[MAX_COROUTINE_NUMBER]{};
    thread_local int32_t readyTaskPool[MAX_COROUTINE_NUMBER]{};

    // Do Not Call This Directly
    struct AsymmetricTransfer {
        constexpr bool await_ready() const noexcept { return false; }
        constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
        constexpr void await_resume() const noexcept {}
    };

    // Do Not Call This Directly
    struct SymmetricTransfer {
        constexpr bool await_ready() const noexcept { return false; }
        auto await_suspend(std::coroutine_handle<AsyncTask::promise_type> continuation) noexcept {
            taskPool[readyTaskPool[readyIndex--]].updateHandler(continuation);
            return *taskPool[readyTaskPool[readyIndex]].getHandler();
        }
        constexpr void await_resume() const noexcept {}
    };

}
// Call `SYMMETRIC_TRANSFER();` For symmetric Transfer
#define SYMMETRIC_TRANSFER() if (nano_coroutine::readyIndex > 0)  co_await nano_coroutine::SymmetricTransfer(); else co_await nano_coroutine::AsymmetricTransfer()
// Call `ASYMMETRIC_TRANSFER();` For asymmetric Transfer
#define ASYMMETRIC_TRANSFER() co_await nano_coroutine::AsymmetricTransfer()

#endif