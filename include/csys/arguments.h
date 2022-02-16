// Copyright (c) 2020-present, Roland Munguia & Tristan Florian Bouchard.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef CSYS_ARGUMENTS_H
#define CSYS_ARGUMENTS_H
#pragma once

#include "csys/api.h"
#include "csys/string.h"
#include "csys/exceptions.h"
#include "csys/argument_parser.h"
#include <vector>
#include <optional>
#include <string>

namespace csys
{
    using NULL_ARGUMENT = void (*)();    //!< Null argument typedef

    /*!
     * \brief
     *      Base case struct where a type is not supported
     */
    template<typename> struct is_supported_type { static constexpr bool value = false; };
    
    /*!
     * \brief
     *      Base case struct where a type is not supported
     */
    template<typename> struct type_name { inline static const std::string value = "Unsupported Type"; };

    /*!
     * \brief
     *      Macro for supporting trivial types
     */
#define SUPPORT_TYPE(TYPE, TYPE_NAME)\
    template<> struct is_supported_type<TYPE> { static constexpr bool value = true; }; \
    template<> struct type_name<TYPE> { inline static const std::string value = TYPE_NAME; };

    //! Supported types
    SUPPORT_TYPE(std::string, "String")

    SUPPORT_TYPE(bool, "Boolean")

    SUPPORT_TYPE(char, "Char")

    SUPPORT_TYPE(unsigned char, "Unsigned_Char")

    SUPPORT_TYPE(short, "Signed_Short")

    SUPPORT_TYPE(unsigned short, "Unsigned_Short")

    SUPPORT_TYPE(int, "Signed_Int")

    SUPPORT_TYPE(unsigned int, "Unsigned_Int")

    SUPPORT_TYPE(long, "Signed_Long")

    SUPPORT_TYPE(unsigned long, "Unsigned_Long")

    SUPPORT_TYPE(long long, "Signed_Long_Long")

    SUPPORT_TYPE(unsigned long long, "Unsigned_Long_Long")

    SUPPORT_TYPE(float, "Float")

    SUPPORT_TYPE(double, "Double")

    SUPPORT_TYPE(long double, "Long_Double")

    //! Supported containers
    template<typename U> struct is_supported_type<std::vector<U>> { static constexpr bool value = is_supported_type<U>::value; };
    template<typename U> struct type_name<std::vector<U>> { inline static const std::string value = "Vector_Of_" + type_name<U>::value; };


    /*!
     * \brief
     *      Wrapper around an argument for use of parsing a command line
     * \tparam T
     *      Data type
     */
    template<typename T>
    struct CSYS_API Arg
    {
        /*!
         * \brief
         *      Is true if type of U is a supported type
         * \tparam U
         *      Type to check if it is supported
         */
        template<typename U>
        static constexpr bool is_supported_type_v = is_supported_type<U>::value;
    public:

        using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;    //!< Type of this argument
        
        /*!
         * \brief
         *      Constructor for an argument
         */
        explicit Arg()
        {
            static_assert(is_supported_type_v<ValueType>,
                    "ValueType 'T' is not supported, see 'Supported types' for more help");
        }

        /*!
         * \brief
         *      Constructor for an argument for naming
         * \param name
         *      Name of the argument
         */
        explicit Arg(const std::string &name) : m_Name(name)
        {
            static_assert(is_supported_type_v<ValueType>,
                    "ValueType 'T' is not supported, see 'Supported types' for more help");
        }
        
        /*!
         * \brief
         *      Constructor for an argument for naming
         * \param name
         *      Name of the argument
         * \param defaultValue
         *      Value to be used if not enough parameters are provided to the command
         */
        explicit Arg(const std::string &name, T&& defaultValue) : m_Name(name), m_DefaultValue(std::forward<T>(defaultValue))
        {
            static_assert(is_supported_type_v<ValueType>,
                    "ValueType 'T' is not supported, see 'Supported types' for more help");
        }

        /*!
         * \brief
         *      Grabs its own argument from the command line and sets its value
         * \param input
         *      Command line argument list
         * \param start
         *      Start of its argument
         * \return
         *      Returns this
         */
        Arg<T> &Parse(std::string &input, size_t &start)
        {
            size_t index = start;

            // Check if there are more arguments to be read in
            if (NextPoi(input, index).first == EndPoi(input))
            {
                if (m_DefaultValue)
                {
                    m_Value = *m_DefaultValue;
                    return *this;
                }
                throw Exception("Not enough arguments were given", input);
            }
            // Set value grabbed from input aka command line argument
            m_Value = ArgumentParser<ValueType>(input, start).m_Value;
            return *this;
        }

        /*!
         * \brief
         *      Gets the info of the argument in the form of [name:type]
         * \return
         *      Returns a string containing the arugment's info
         */
        std::string Info()
        {
            auto info = std::string(" [") + m_Name + ":" + type_name<T>::value;
            if (m_DefaultValue)
            {
                info += ":default=" + to_string(*m_DefaultValue);
            }
            info += "]";
            return info;
        }

        const std::string m_Name;                       //!< Name of argument
        std::optional<ValueType> m_DefaultValue;        //!< Value used if not enough arguments are provided
        ValueType m_Value;                              //!< Actual value
    };

    /*!
     * \brief
     *      Template specialization for a null argument that gets appended to a command's argument list to check if more
     *      than the required number of arguments
     */
    template<>
    struct CSYS_API Arg<NULL_ARGUMENT>
    {
        /*!
         * \brief
         *      Checks if the input starting from param 'start' is all whitespace or not
         * \param input
         *      Command line argument list
         * \param start
         *      Start of its argument
         * \return
         *      Returns this
         */
        Arg<NULL_ARGUMENT> &Parse(std::string &input, size_t &start)
        {
            if (NextPoi(input, start).first != EndPoi(input))
                throw Exception("Too many arguments were given", input);
            return *this;
        }
    };
}

#endif //CSYS_ARGUMENTS_H
