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
  struct  CommandBase
  {
    virtual ~CommandBase() = default;
    virtual Item operator()(String &input) = 0;
    virtual std::string Help() = 0;
		virtual bool TakesArguments() const = 0;
  };

  template<typename Fn, typename ...Args>
  class CCLI_API Command : public CommandBase
  {
  public:
    Command(String name, String description, Fn function, Args... args)
            : m_Name(std::move(name)), m_Description(std::move(description)), m_Function(function), m_Arguments(args...) {}

		Item operator()(String &input) override
    {
      // call the function
			size_t start = 0;
			try { Call(input, start, std::make_index_sequence<sizeof... (Args)>{}); }
			catch (Exception& ae) { return Item(ERROR) << (m_Name.m_String + ": " + ae.what()); }
			return Item(NONE);
    }

		[[nodiscard]] std::string Help() override
    {
			return "COMMAND: " + m_Name.m_String + "\n"
			     + "     Description - " + m_Description.m_String + "\n\n"
					 + "     Usage - " + m_Name.m_String + DisplayArguments(std::make_index_sequence<sizeof... (Args)>{}) + "\n\n";
    }

		[[nodiscard]] bool TakesArguments() const override
		{
			return true;
		}
  private:
    template<size_t... Is>
    void Call(String &input, size_t &start, std::index_sequence<Is...>)
    {
      // call function with unpacked tuple
      m_Function((std::get<Is>(m_Arguments).Parse(input, start).m_Arg.m_Value)...);
    }

    template<size_t ...Is>
    std::string DisplayArguments(std::index_sequence<Is...>)
    {
    	return (std::get<Is>(m_Arguments).Info() + ...);
    }

    const String m_Name;
		const String m_Description;
    std::function<void(typename Args::ValueType...)> m_Function;
    std::tuple<Args...> m_Arguments;
  };

	template<typename Fn>
	class CCLI_API Command<Fn> : public CommandBase
	{
	public:
		Command(String name, String description, Fn function)
						: m_Name(std::move(name)), m_Description(std::move(description)), m_Function(function) {}

		Item operator()(String &input) override
		{
			// call the function
			for (auto c : input.m_String)
				if (!isspace(c))
					return Item(ERROR) << (m_Name.m_String + ": Called with arguments");
			m_Function();
			return Item(NONE);
		}

		[[nodiscard]] std::string Help() override
		{
			return "COMMAND: " + m_Name.m_String + "\n"
			     + "     Description - " + m_Description.m_String + "\n\n"
					 + "     Usage - " + m_Name.m_String + "\n\n";
		}

		[[nodiscard]] bool TakesArguments() const override
		{
			return false;
		}
	private:
		template<size_t... Is>
		[[maybe_unused]] void Call(String &input, size_t &start, std::index_sequence<Is...>);

		template<size_t ...Is>
		std::string DisplayArguments(std::index_sequence<Is...>) { return ""; }

		const String m_Name;
		const String m_Description;
		std::function<void(void)> m_Function;
		std::tuple<> m_Arguments;
	};
}

#endif //CCLI_COMMAND_H
