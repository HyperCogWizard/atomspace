/*
 * opencog/atoms/reduct/ArithmeticLink.cc
 *
 * Copyright (C) 2015 Linas Vepstas
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <limits>

#include <opencog/atoms/base/atom_types.h>
#include <opencog/atoms/base/ClassServer.h>
#include <opencog/atoms/core/NumberNode.h>
#include "ArithmeticLink.h"

using namespace opencog;

ArithmeticLink::ArithmeticLink(const HandleSeq& oset, Type t)
    : FoldLink(oset, t)
{
	init();
}

ArithmeticLink::ArithmeticLink(Type t, const Handle& a, const Handle& b)
    : FoldLink(t, a, b)
{
	init();
}

ArithmeticLink::ArithmeticLink(const Link& l)
    : FoldLink(l)
{
	init();
}

void ArithmeticLink::init(void)
{
	Type tscope = get_type();
	if (not classserver().isA(tscope, ARITHMETIC_LINK))
		throw InvalidParamException(TRACE_INFO, "Expecting an ArithmeticLink");

	_commutative = false;
}

// ===========================================================
/// reduce() -- reduce the expression by summing constants, etc.
///
/// No actual black-box evaluation or execution is performed. Only
/// clearbox reductions are performed.
///
/// Examples: the reduct of (PlusLink (NumberNode 2) (NumberNode 2))
/// is (NumberNode 4) -- its just a constant.
///
/// The reduct of (PlusLink (VariableNode "$x") (NumberNode 0)) is
/// (VariableNode "$x"), because adding zero to anything yeilds the
/// thing itself.
Handle ArithmeticLink::reduce(void) const
{
	Handle road(reorder());
	ArithmeticLinkPtr alp(ArithmeticLinkCast(road));

	Handle red(alp->FoldLink::reduce());

	alp = ArithmeticLinkCast(red);
	if (NULL == alp) return red;
	return alp->reorder();
}

// ============================================================

/// re-order the contents of an ArithmeticLink into "lexicographic" order.
///
/// The goal of the re-ordering is to simplify the reduction code,
/// by placing atoms where they are easily found.  For now, this
/// means:
/// first, all of the variables,
/// next, all compound expressions,
/// last, all number nodes
/// We do not currently sort the variables, but maybe we should...?
/// Sorting by variable names would hold consilidate them...
/// The FoldLink::reduce() method already returns expressions that are
/// almost in the correct order.
Handle ArithmeticLink::reorder(void) const
{
	if (not _commutative) return get_handle();

	HandleSeq vars;
	HandleSeq exprs;
	HandleSeq numbers;

	for (const Handle& h : _outgoing)
	{
		Type htype = h->get_type();

		// Hack for pattern matcher, which returns SetLinks of stuff.
		// Recurse exacly once.
		if (SET_LINK == htype)
		{
			for (const Handle& he : h->getOutgoingSet())
			{
				Type het = he->get_type();
				if (VARIABLE_NODE == het)
					vars.push_back(he);
				else if (NUMBER_NODE == het)
					numbers.push_back(he);
				else
					exprs.push_back(he);
			}
		}
		else if (VARIABLE_NODE == htype)
			vars.push_back(h);
		else if (NUMBER_NODE == htype)
			numbers.push_back(h);
		else
			exprs.push_back(h);
	}

	HandleSeq result;
	for (const Handle& h : vars) result.push_back(h);
	for (const Handle& h : exprs) result.push_back(h);
	for (const Handle& h : numbers) result.push_back(h);

	return Handle(createLink(result, get_type()));
}

// ===========================================================
/// execute() -- Execute the expression
Handle ArithmeticLink::execute(AtomSpace* as) const
{
	return reduce();
}

// ===========================================================
