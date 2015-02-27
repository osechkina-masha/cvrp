#include "adopter.h"

bool try_kuhn (int vertex, const std::vector<std::vector<int>> &graph, std::unordered_set<int> &used,
				std::vector<int> &maxMatching)
{
	if (used.find(vertex) != used.end())
	{
		return false;
	}
	used.insert(vertex);
	for (unsigned i = 0; i < graph[vertex].size(); ++i)
	{
		int to = graph[vertex][i];
		if (maxMatching[to] == -1 || try_kuhn(maxMatching[to], graph, used, maxMatching))
		{
			maxMatching[to] = vertex;
			return true;
		}
	}
	return false;
}

unsigned int getMaxMatchingSize(std::vector<std::vector<int>> const &graph, int secondPartSize)
{
	std::vector<int> matching(secondPartSize, -1);
	for (unsigned int v = 0; v < graph.size(); ++v)
	{
		std::unordered_set<int> used;
		try_kuhn(v, graph, used, matching);
	}
	unsigned int usedVertexs = 0;
	for (unsigned int i = 0; i < matching.size(); i ++)
	{
		if (matching[i] < 0)
		{
			continue;
		}
		usedVertexs ++;
	}
	return usedVertexs;
}



std::vector<int> getMatching(const std::vector<std::vector<int> > &graph, int secondPartSize)
{
	std::vector<int> matching(secondPartSize, -1);
	for (unsigned int v = 0; v < graph.size(); ++v)
	{
		std::unordered_set<int> used;
		try_kuhn(v, graph, used, matching);
	}
	return matching;
}
