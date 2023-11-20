#ifndef ENGINEY_TYPE_UTILS_HPP
#define ENGINEY_TYPE_UTILS_HPP

#include <type_traits>
#include <vector>
#include <array>
#include <ostream>

namespace EngineY {

namespace reflection {

namespace member_pointer {

namespace /* implementation */ {

    template <typename T>
    struct func_return_type;

    template<typename O , typename Ret , typename... Args>
    struct func_return_type<Ret(O::*)(Args...)> {
        using type = Ret;
    };

    template<typename O , typename Ret , typename... Args>
    struct func_return_type<Ret(O::*)(Args...) const> {
        using type = Ret;
    };

    template<typename T>
    struct object_return_type;

    template<typename O , typename Ret>
    struct object_return_type<Ret O::*> {
        using type = Ret;
    };


} // anonymous namespace

    template <typename T>
    struct return_type;

    template <typename T>
    struct return_type : std::enable_if<
        std::is_member_pointer_v<T> ,
        std::conditional_t<
            std::is_member_object_pointer_v<T> ,
            object_return_type<T> ,
            func_return_type<T>
        >
    > {};


} // namespace member_pointer

    template<typename , typename = void>
    struct is_specialized : std::false_type {};

    template <typename T>
    struct is_specialized<T , std::void_t<decltype(T{})>> : std::true_type {};

namespace /* is array implementation */ {

    template <typename T>
    struct is_array_impl : std::false_type {};

    template <typename T , std::size_t N>
    struct is_array_impl<std::array<T ,N>> : std::true_type {};

    template <typename... Args> 
    struct is_array_impl<std::vector<Args...>> : std::true_type {};

} // anonymous namespace


    template <typename T>
    struct is_array {
        static constexpr bool const value = is_array_impl<std::decay_t<T>>::value;
    };

    template <typename T>
    inline constexpr bool is_array_v = is_array<T>::value;


    template <typename T>
    class is_streamable {
        
        template <typename TT>
        static constexpr auto test(int) -> decltype(std::declval<std::ostream&>() << std::declval<TT>() , std::true_type{});

        template <typename>
        static constexpr auto test(...) -> std::false_type;

        public:
            static constexpr bool value = decltype(test<T>(0))::value;
    };

    template <typename T>
    inline constexpr bool is_streamable_v = is_streamable<T>::value;

    struct filter_void_alt {};

    template <typename T , typename Alt = filter_void_alt>
    struct filter_void {
        using type = std::conditional_t<std::is_void_v<T> , Alt , T>;
    };

    template <typename T>
    using filter_void_t = typename filter_void<T>::type;

#if __cplusplus >= 202002L
    
    template <typename Tuple , typename Fn>
    constexpr void for_each_tuple(Tuple&& tp , Fn&& fn)  {
        std::apply(
            [&fn]<typename ...T>(T&&... args) {
                (fn(std::forward<T>(args)) , ...);
            } , std::forward<Tuple>(tp)
        );
    }

#else

    template <typename Tuple , typename Fn>
    constexpr void for_each_tuple(Tuple&& tp , Fn&& fn) {
        std::apply(
            [&fn](auto&& ...args) {
                (fn(std::forward<decltype(args)>(args)) , ...);
            } , std::forward<Tuple>(tp)
        );
    }

#endif // !C++20
       
#if __cplusplus >= 202002L

    template<auto N>
    static constexpr auto unroll = [](auto expr) {
    	[expr] <auto...Is>(std::index_sequence<Is...>) {
    	    ((expr(), void(Is)), ...);
    	}(std::make_index_sequence<N>());
    };
    
    template<auto N, typename...Args>
    constexpr auto nth_element(Args... args) {
    	return[&]<size_t... Ns>(std::index_sequence<Ns...>) {
    	    return [](decltype((void*)Ns)..., auto* nth, auto*...) {
    		return *nth;
    	    }(&args...);
    	}(std::make_index_sequence<N>());
    }
    
    template<auto N>
    constexpr auto nth_element_l = [](auto... args) {
    	return[&]<std::size_t... Ns>(std::index_sequence<Ns...>) {
    	    return [](decltype((void*)Ns)..., auto* nth, auto*...) {
    		return *nth;
    	    }(&args...);
    	}
    	(std::make_index_sequence<N>{});
    };

#else

namespace /* nth element unroll impl */ {
	
    template<size_t... Ns, typename... Args>
    constexpr auto nth_element_unroll(std::index_sequence<Ns...>, Args...args) {
	return [](decltype((void*)Ns)..., auto* nth, auto*...) {
	    return *nth;
	}(&args...);
    }

}

    template<auto N, typename...Args>
    constexpr auto nth_element(Args... args) {
        return impl::nth_element_unroll(std::make_index_sequence<N>(), args...);
    }

#endif // !C++20

} // namespace reflection

} // namespace EngineY

#endif // !ENGINEY_TYPE_UTILS_HPP
