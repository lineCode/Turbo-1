/******************************************************************************
* Turbo C++11 metaprogramming Library                                         *
*                                                                             *
* Copyright (C) 2013 - 2014, Manuel Sánchez Pérez                             *
*                                                                             *
* This file is part of The Turbo Library.                                     *
*                                                                             *
* The Turbo Library is free software: you can redistribute it and/or modify   *
* it under the terms of the GNU Lesser General Public License as published by *
* the Free Software Foundation, version 2 of the License.                     *
*                                                                             *
* The Turbo Library is distributed in the hope that it will be useful,        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              * 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU Lesser General Public License for more details.                         *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with The Turbo Library. If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************/

#ifndef FUNCTIONAL_HPP
#define	FUNCTIONAL_HPP

#include <type_traits>

namespace tml
{
    namespace impl
    {
        /*
         * The library assumes that any type with a 'result' member type is a function
         * (A function which result is stored in that 'result' member).
         * 
         * This is an internal (not dessigned to be used by the user) type trait to 
         * check the existence of that member type, that is, to check if a type is
         * a function.
         */
        template<typename T>
        struct is_function
        {
            template<typename U> static std::true_type test( typename U::result* );
            template<typename U> static std::false_type test( ... );
            
            static constexpr bool result = decltype( test<T>( nullptr ) )::value;
        };
        
        /*
         * Here we implement the user-side tml::eval<> metafunction. 
         * 
         * The purpose of that metafunction is to evaluate any kind of expression.
         * That serves to evaluate functions and other functional expressions.
         */   


        /*
         * The implementation has three parameters:
         * 
         *  - IS_FUNCTION: A boolean flag which says if the expression passed is a function.
         *    Its used to evaluate the expression correctly (Extracting its 'result' member type
         *    or not).
         * 
         *  - E: The expression to be evaluated.
         * 
         *  - ARGS...: evaluate could be used as a high-order metafunction to evaluate a given
         *             function entity with the specified parameters. This variadic pack is that
         *             set of parameters. The result of the evaluation is the result of evaluating
         *             the function entity E with the specified ARGS... arguments.
         *             Note that in this case the argumments are evaluated too (Just for the case they are
         *             functional expressions).
         * 
         *  Of course this metafunction is a function too, so it stores the result of the evaluation in a 'result' member type.
         */
        template<bool IS_FUNCTION , typename E , typename... ARGS>
        struct evaluate_impl;

        /* This is the most simple case: There are no evaluation parameters (So the expression could be any
         * kind of expression, not just a function) BUT the flag says the expression is not a function.
         * The result of evaluatiing such expression is the expression itself.
         */
        template<typename E>
        struct evaluate_impl<false,E>
        {
            using result = E;
        };

        /*
         * This specialization matches the case when the expression passed is a function.
         * The result of the evalutation is just forwarded to the implementation, to reduce
         * template instantation stack usage (Reduce template instantation depth).
         * 
         * So the implementation just inherit the function to get its result.
         */
        template<typename F>
        struct evaluate_impl<true,F> : public F 
        {};

        /*
         * This is the case when the expression passed is a function, and a set of parameters (At least one) is 
         * passed to evaluate the function with. 
         *
         * Note that the parameters of the function call are evaluated too.
         * 
         * The result is the result of evaluating the function with that parameters.
         */
        template<template<typename...> class F , typename... PLACEHOLDERS , typename FIRST_ARG , typename... ARGS>
        struct evaluate_impl<true,F<PLACEHOLDERS...> , FIRST_ARG,ARGS...> : public F<FIRST_ARG,ARGS...>
        {
          static_assert( sizeof...(PLACEHOLDERS) == (1 + sizeof...(ARGS)) , "Incorrect number of parameters to call the function." );  
        };
    }
    
    
    //Canonical function entity:
    template<typename RESULT>
    struct function
    {
        using result = RESULT;
    };
    
    
    //Metafunction to evaluate expressions:
    template<typename EXPRESSION , typename... ARGS>
    using eval = typename impl::evaluate_impl<impl::is_function<EXPRESSION>::result, EXPRESSION , ARGS...>::result;
    
}

#endif	/* FUNCTIONAL_HPP */

