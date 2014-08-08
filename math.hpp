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

#ifndef MATH_HPP
#define	MATH_HPP

#include "algorithm.hpp"
#include "algebra.hpp"
#include "integral.hpp"
#include "fixed_point.hpp"
#include "lazy.hpp"
#include "type_traits.hpp"
#include "control_structures.hpp"
#include "eval.hpp"

/*
 * This header defined some matematical metafunctions
 */

namespace tml
{
    /*
     * The tml::pow metafunction returns the value of BASE raised to the value of EXP.
     * Valid tml::sign , tml::mul , and tml::div specializations should exist.
     * 
     * Note that this operation is fully generic, relying only on the different algebraic operations
     * provided to operate on the parameters. That has some consequences, for example note that
     * negative exponent with integral values has no sense, but its perfectly computed and returns 
     * zero always.
     */
    template<typename BASE , typename EXP>
    struct pow
    {
        using integral_exp = tml::lconditional<tml::is_fixed_point<EXP>,
                                               tml::lazy<tml::func::to_integral,EXP>,
                                               EXP
                                              >;
                                                
        template<typename B , typename E>
        struct positive_pow : public tml::function<tml::foldl<tml::lambda<_1,_2 , tml::mul<_1,B>> , tml::one<B> , tml::repeat<void,E>>>
        {};
        
        template<typename B , typename E>
        struct negative_pow : public tml::function<tml::eval<tml::div<tml::one<B>,positive_pow<B,tml::abs<E>>>>>
        {};
        
        /*
         * Compute the result.
         * 
         * A lazy conditional is used as an optimization: Only the selected branch is executed (instantiated)
         */
        using result = tml::eval<tml::lconditional<tml::sign<integral_exp>,
                                                   tml::lazy<positive_pow,BASE,integral_exp>,
                                                   tml::lazy<negative_pow,BASE,integral_exp>
                                                  >
                                >;
    };
}

#endif	/* MATH_HPP */

