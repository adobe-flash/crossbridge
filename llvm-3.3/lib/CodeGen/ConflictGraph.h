#ifndef CONFLICTGRAPH_H
#define CONFLICTGRAPH_H

#include "llvm/Support/DataTypes.h"

// To avoid problem with non-clang compilers not having this macro.
#if defined(__has_include)
	#if __has_include(<unordered_map>)
		#include <unordered_map>
	#else
		#include <tr1/unordered_map>
		namespace std {
			using tr1::unordered_map;
		}
	#endif
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
