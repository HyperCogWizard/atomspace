/*
 * opencog/atoms/reduct/MinusLink.cc
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

#include <opencog/atoms/base/atom_types.h>
#include <opencog/atoms/base/ClassServer.h>
#include <opencog/atoms/core/NumberNode.h>
#include "MinusLink.h"
#include "PlusLink.h"

using namespace opencog;

MinusLink::MinusLink(const HandleSeq& oset, Type t)
    : PlusLink(oset, t)
{
	init();
}

MinusLink::MinusLink(const Handle& a, const Handle& b)
    : PlusLink(MINUS_LINK, a, b)
{
	init();
}

MinusLink::MinusLink(Type t, const Handle& a, const Handle& b)
    : PlusLink(t, a, b)
{
	init();
}

MinusLink::MinusLink(const Link& l)
    : PlusLink(l)
{
	init();
}

void MinusLink::init(void)
{
	Type tscope = get_type();
	if (not classserver().isA(tscope, MINUS_LINK))
		throw InvalidParamException(TRACE_INFO, "Expecting a MinusLink");

	size_t sz = _outgoing.size();
	if (2 < sz or 0 == sz)
		throw InvalidParamException(TRACE_INFO,
			"Don't know how to subract that!");

	_commutative = false;
	knil = Handle(createNumberNode("0"));
}

static inline double get_double(const Handle& h)
{
	return NumberNodeCast(h)->get_value();
}

Handle MinusLink::kons(const Handle& fi, const Handle& fj) const
{
	// Are they numbers?
	if (NUMBER_NODE == fi->get_type() and
	    NUMBER_NODE == fj->get_type())
	{
		double diff = get_double(fi) - get_double(fj);
		return Handle(createNumberNode(diff));
	}

	// If fj is zero, just drop it.
	if (content_eq(fj, knil))
		return fi;

	// If we are here, we've been asked to subtracttwo things,
	// but they are not of a type that we know how to subtract.
	return Handle(createMinusLink(fi, fj));
}

DEFINE_LINK_FACTORY(MinusLink, MINUS_LINK)

// ============================================================
