//
// Created by antimatter on 5/26/20.
//

#ifndef CCLI_COMMAND_H
#define CCLI_COMMAND_H
#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include "ccli/ccli_arguments.h"
#include "ccli/ccli_exceptions.h"
#include "ccli/ccli_item.h"

namespace ccli
{
	// TODO: Fix flipping issue and same setting issue
	// TODO: Fix parse command with ranges and stuff
	// Fixed commands being given too many arguments or not enough
  struct  CommandBase
  {
    virtual ~CommandBase() = default;
    virtual Item operator()(String &input) = 0;
		[[nodiscard]] virtual std::string Help() = 0;
		[[nodiscard]] virtual size_t ArgumentCount() const = 0;
  };

  template<typename Fn, typename ...Args>
  class CCLI_API Command : public CommandBase
  {

#ifdef __clang__
		template<size_t N>
		using m_MakeEvalIndexSequence = std::make_index_sequence<N>;
#else
		template <size_t ...Is>
		constexpr auto static inline ReverseIndexSequence(std::index_sequence<Is...> const &)
		->decltype(std::index_sequence<sizeof...(Is) - 1U - Is...>{}){}

		template <size_t N>
		using m_MakeEvalIndexSequence = decltype(ReverseIndexSequence(std::make_index_sequence<N>{}));
#endif

	public:
    Command(String name, String description, Fn function, Args... args)
            : m_Name(std::move(name)), m_Description(std::move(description)),
            m_Function(function), m_Arguments(args..., Arg<NULL_ARGUMENT>()) {}

		Item operator()(String &input) override
    {
      // try to parse and call the function
			try { Call(input, m_MakeEvalIndexSequence<sizeof ...(Args) + 1>{}, std::make_index_sequence<(sizeof ...(Args))>{}); }
			catch (Exception& ae) { return Item(ERROR) << (m_Name.m_String + ": " + ae.what()); }
			return Item(NONE);
    }

		[[nodiscard]] std::string Help() override
    {
			return "COMMAND: " + m_Name.m_String + "\n"
			     + "     Description - " + m_Description.m_String + "\n\n"
					 + "     Usage - " + m_Name.m_String + DisplayArguments(std::make_index_sequence<sizeof ...(Args)>{}) + "\n\n";
    }

		[[nodiscard]] size_t ArgumentCount() const override
		{
			return sizeof... (Args);
		}

  private:
    template<size_t... Is_p, size_t... Is_c>
    void Call(String &input, const std::index_sequence<Is_p...>&, const std::index_sequence<Is_c...>&)
    {
			size_t start = 0;

    	// parse arguments
  		Parse((std::get<Is_p>(m_Arguments).Parse(input, start))...);

      // call function with unpacked tuple
      m_Function((std::get<Is_c>(m_Arguments).m_Arg.m_Value)...);
    }

		// Used for the expansion of a tuple for parsing
		template<typename ...Ts> void Parse(Ts&...) {}

    template<size_t ...Is>
    std::string DisplayArguments(const std::index_sequence<Is...> &)
    {
    	return (std::get<Is>(m_Arguments).Info() + ...);
    }

    const String m_Name;
		const String m_Description;
    std::function<void(typename Args::ValueType...)> m_Function;
    std::tuple<Args..., Arg<NULL_ARGUMENT>> m_Arguments;
  };

	template<typename Fn>
	class CCLI_API Command<Fn> : public CommandBase
	{
	public:
		Command(String name, String description, Fn function)
						: m_Name(std::move(name)), m_Description(std::move(description)),
						m_Function(function), m_Arguments(Arg<NULL_ARGUMENT>()) {}

		Item operator()(String &input) override
		{
			// call the function
			size_t start = 0;
			std::get<0>(m_Arguments).Parse(input, start);
			m_Function();
			return Item(NONE);
		}

		[[nodiscard]] std::string Help() override
		{
			return "COMMAND: " + m_Name.m_String + "\n"
			     + "     Description - " + m_Description.m_String + "\n\n"
					 + "     Usage - " + m_Name.m_String + "\n\n";
		}

		[[nodiscard]] size_t ArgumentCount() const override
		{
			return 0;
		}
	private:

		const String m_Name;
		const String m_Description;
		std::function<void(void)> m_Function;
		std::tuple<Arg<NULL_ARGUMENT>> m_Arguments;
	};
}

#endif //CCLI_COMMAND_H
