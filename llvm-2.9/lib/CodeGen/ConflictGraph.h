// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef CONFLICTGRAPH_H
#define CONFLICTGRAPH_H

#include "llvm/Support/DataTypes.h"

#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__) || defined(_MSC_VER)
#include <unordered_map>
#else
#include <tr1/unordered_map>
namespace std {
using tr1::unordered_map;
}
#endif

#include <vector>

class ConflictGraphVertex;

// simple conflict graph class w/ greedy coloring
class ConflictGraph
{
	// id => vertex
	typedef std::unordered_map<uintptr_t, ConflictGraphVertex *> Vertices;
	Vertices _vertices;

public:
	// id => color numbered from [0..n]
	typedef std::vector< std::pair<uintptr_t, unsigned> > Coloring;

	~ConflictGraph();
	// add a single vertex with an arbitrary id
	// redundant vertex adds are legal
	void addVertex(uintptr_t a);
	// add an edge between two arbitrary vertex ids (which may have but need not have been addVertex-ed)
	// redundant edge adds are legal
	void addEdge(uintptr_t a, uintptr_t b);
	void color(Coloring *result) const; // generate a coloring in result
};

#endif
