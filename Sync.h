#pragma once
#include <mutex>
#include <shared_mutex>

/**
 * For when you want folly::Synchronized but don't want to install the universe.
 *
 * https://github.com/facebook/folly/blob/master/folly/docs/Synchronized.md
 *
 * If T has const member functions which are not thread-safe,
 * you're going to have a bad time.
 *
 * License: CC-BY-SA. Creative Commons Attribution-ShareAlike. github.com/csp256/sync
 */

namespace csp {

	template <typename T>
	class Sync {
		using Mutex = std::shared_mutex;
		using ReadLock = std::shared_lock<Mutex>;
		using WriteLock = std::unique_lock<Mutex>;

		mutable Mutex mutex_;
		T data_;

		//////////////////////////////////////////////////////////////////////////////

	public:
		template <typename T>
		class ReadPtr {
			ReadLock lock_;
			T const* const data_;

		public:
			explicit
			ReadPtr(Mutex& mutex, T const* const data)
				: lock_{ mutex }
				, data_{ data }
			{}

			inline T const&
			operator*() const
			{
				return *data_;
			}

			inline T const&
			operator->() const
			{
				return *data_;
			}
		
		}; // ReadPtr

		//////////////////////////////////////////////////////////////////////////////

		template <typename T>
		class WritePtr {
			WriteLock lock_;
			T* const data_;

		public:
			explicit
			WritePtr(Mutex& mutex, T* const data)
				: lock_{ mutex }
				, data_{ data }
			{}

			inline T&
			operator*() const
			{
				return *data_;
			}

			inline T&
			operator->() const
			{
				return *data_;
			}

		}; // WritePtr
    
   		//////////////////////////////////////////////////////////////////////////////

		template <typename ... Args>
		Sync(Args&& ... args)
			: data_{ std::forward<Args>(args)... }
		{}

		inline ReadPtr<T>
		rlock() const
		{
			return ReadPtr<T>(mutex_, &data_);
		}

		inline WritePtr<T>
		wlock()
		{
			return WritePtr<T>(mutex_, &data_);
		}

		template <typename Callable>
		inline auto
		with_rlock(Callable&& f)
		{
			auto lock = ReadLock{ mutex_ };
			auto const* const ptr = &data_;
			return f(ptr);
		}

		template <typename Callable>
		inline auto
		with_wlock(Callable&& f)
		{
			auto lock = WriteLock{ mutex_ };
			auto* const ptr = &data_;
			return f(ptr);
		}

		inline T
		copy()
		{
			auto lock = ReadLock{ mutex_ };
			return data_;
		}

		//// Requires C++20 
		//inline std::string
		//str() requires requires (T t) { std::to_string(t); }
		//{
		//	auto lock = RLock_{ mutex };
		//	return std::to_string(data);
		//}
    
	}; // Sync

} // namespace csp
