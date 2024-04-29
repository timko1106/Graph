#ifndef GRAPH_CPP
#define GRAPH_CPP

#include "Graph.h"
#include <fstream>
#include <set>


struct count_path_vars {
	index_t target;
	_size_t* states;//USING - вершина используется (повторно не проходить)
	//CLEAR - результат не посчитан
	//Иначе хранится кол-во путей
	const Graph& g;
};

/// <summary>
/// Поиск в глубину для нахождения количества путей
/// </summary>
/// <param name="curr">текущая вершина</param>
/// <param name="vars"></param>
/// <returns>кол-во путей</returns>
_size_t dfs_path(index_t curr, const count_path_vars& vars) {
	if (curr == vars.target)return 1;
	if (vars.states[curr] == USING)return 0;
	if (vars.states[curr] != CLEAR)return vars.states[curr];
	vars.states[curr] = USING;
	index_t next;
	_size_t sum = 0;
	for (index_t i = 0; i < vars.g.nodes[curr].ncount; ++i) {
		next = vars.g.nodes[curr].roads[i].to->index;
		if (vars.states[next] == USING)continue;
		if (vars.states[next] != CLEAR) {
			sum += vars.states[next];
			continue;
		}
		sum += dfs_path(next, vars);
	}
	vars.states[curr] = sum;
	return sum;
}

/// <summary>
/// Поиск количества путей из точки A в B
/// </summary>
/// <param name="from">A</param>
/// <param name="to">B</param>
/// <returns></returns>
_size_t Graph::paths_count(name_t from, name_t to) {
	//Состояния:
	try {
		_size_t* states = logger::malloc<_size_t>(ncount, ARGS);
		index_t target = nmap[to]->index, from_idx = nmap[from]->index;
		for (int i = 0; i < ncount; ++i)states[i] = CLEAR;
		_size_t count = dfs_path(from_idx, { target, states, *this });
		delete[] states;
		return count;
	}
	catch (...) { return 0; }
}

/// <summary>
/// Инвертирование графа (переворот направления всех рёбер), скорее всего O(N^2)
/// </summary>
void Graph::invert() {
	proxy_array<node, MAX_NODES_COUNT> __nodes{};
	for (int i = 0; i < ncount; ++i) {
		__nodes[i].index = i;
		__nodes[i].name = this->nodes[i].name;
		__nodes[i].ncount = 0;
		__nodes[i].roads = logger::malloc<Road>(ncount - 1, ARGS);
	}
	node* curr = this->nodes;
	for (int i = 0; i < ncount; ++i) {
		for (int j = 0; j < curr->ncount; ++j) {
			auto& road = curr->roads[j];
			auto& from = __nodes[road.to->index];
			auto _count = from.ncount;
			from.roads[_count].from = &from;
			from.roads[_count].to = &(__nodes[road.from->index]);
			from.roads[_count].size = road.size;
			++(from.ncount);
		}
		++curr;
	}
	curr = this->nodes;
	node* future = __nodes.arr;
	for (int i = 0; i < ncount; ++i) {//копирование
		curr->ncount = future->ncount;
		delete[] curr->roads;
		curr->roads = nullptr;
		if (future->ncount != 0)curr->roads = logger::malloc<Road>(future->ncount, ARGS);
		for (int j = 0; j < curr->ncount; ++j) {
			curr->roads[j].from = curr;
			curr->roads[j].to = &(nodes[future->roads[j].to->index]);
			curr->roads[j].size = future->roads[j].size;
		}
		++curr;
		++future;
	}
}

/// <summary>
/// Чтение графа
/// Формат:
/// N - количество вершин
/// a1 a2 ... an - названия вершин
/// K1 b1 d1 b2 d2 ... bk1 dk1 - (K - число рёбер, исходящих из вершины. b1 ... - точки, куда ведут пути. d1 ... - вес ребра)
/// K2 ...
/// ...
/// Kn ...
/// </summary>
/// <param name="g">Ссылка на пустой граф</param>
/// <param name="filename">Путь к файлу, где находится граф</param>
/// <returns>0 - если граф прочитан успешно. 1 - если не удалось открыть.</returns>
int read(Graph& g, const char* filename) {
	std::fstream in(filename, std::ios_base::in);
	if (!in.is_open())return 1;
	int N;
	in >> N;
	g.ncount = N;
	name_t name;
	for (int i = 0; i < N; ++i) {
		in >> name;
		g.nodes[i].name = name;
		g.nodes[i].index = i;
		g.nmap[name] = &(g.nodes[i]);
	}
	for (int i = 0; i < N; ++i) {
		int K;
		in >> K;
		auto curr = g.nodes + i;
		curr->ncount = K;
		curr->roads = nullptr;
		if (K == 0)continue;
		curr->roads = logger::malloc<Road>(K, ARGS);
		for (int j = 0; j < K; ++j) {
			name_t to;
			int size;
			in >> to >> size;
			index_t to_idx = g.nmap[to]->index;
			curr->roads[j].from = curr;
			curr->roads[j].to = g.nodes + to_idx;
			curr->roads[j].size = size;
		}
	}
	return 0;
}

/// <summary>
/// сложение путей и присвоение в 1
/// </summary>
/// <param name="other"></param>
/// <returns>путь №1</returns>
path& path::operator+= (const path& other) {
	Road* __roads = logger::malloc<Road>(ncount + other.ncount, ARGS);
	for (int i = 0; i < ncount; ++i) {
		__roads[i] = this->roads[i];
	}
	Road* curr = __roads + ncount;
	for (int i = 0; i < other.ncount; ++i) {
		*(curr++) = other.roads[i];
	}
	delete[] roads;
	ncount += other.ncount;
	roads = __roads;
	length += other.length;
	return *this;
}

/// <summary>
/// Реализуется через +=
/// </summary>
/// <param name="other"></param>
/// <returns></returns>
path path::operator+(const path& other) const {
	return path(*this) += other;
}

std::ostream& operator<< (std::ostream& os, const path& p) {//Вывод пути
	//&p != nullptr (UB)
	if (p.ncount == 0) return os << "Path not found";
	os << p.roads[0].from->name;
	for (int i = 0; i < p.ncount; ++i)os << " -> " << p.roads[i].to->name;
	return os;
}
struct topsorting_vars {//Переменные для топологической сортировки (выносить в глобальные очень нехорошо)
	bool* visited;
	const Graph& g;
	std::vector<index_t, logger::Allocator<index_t>>& v;
};

/// <summary>
/// Поиск в глубину
/// </summary>
/// <param name="current">индекс текущей вершины</param>
/// <param name="vars"></param>
void dfs_topsorting(index_t current, topsorting_vars& vars) {
	vars.visited[current] = true;
	for (int i = 0; i < vars.g.nodes[current].ncount; ++i) {
		index_t to_idx = vars.g.nodes[current].roads[i].to->index;
		if (!vars.visited[to_idx]) {
			dfs_topsorting(to_idx, vars);
		}
	}
	vars.v.push_back(current);
}

std::vector<index_t, logger::Allocator<index_t>> Graph::topsort(bool full, index_t from) {//Топологическая сортировка (результат нужно перевернуть)
	std::vector<index_t, logger::Allocator<index_t>> v;
	v.reserve(ncount);
	bool* visited = logger::malloc<bool>(ncount, ARGS);
	for (int i = 0; i < ncount; ++i)visited[i] = false;
	topsorting_vars variables = { visited, *this, v };
	if (full) {
		for (int i = 0; i < ncount; ++i) {
			if (!visited[i]) {
				dfs_topsorting(i, variables);
			}
		}
	}
	else {
		dfs_topsorting(from, variables);
	}
	logger::free<bool> (visited, ncount, ARGS);
	return v;
}

path Graph::shortest_path(name_t from, name_t to) {
	//Дейкстра за O ((N + M)log N), где N - кол-во вершин, M - кол-во рёбер.
	index_t from_idx = nmap[from]->index, to_idx = nmap[to]->index;
	_size_t* distances = logger::malloc<_size_t>(ncount, ARGS);//Массив расстояний до точки
	index_t* parents = logger::malloc<index_t>(ncount, ARGS);//Массив предков для восстановления пути
	bool* used = logger::malloc<bool>(ncount, ARGS);
	for (index_t i = 0; i < ncount; ++i) {
		distances[i] = MAX_LENGTH;
		used[i] = false;
		parents[i] = MAX_LENGTH;
	}
	std::set<std::pair<_size_t, int>> q{};
	q.insert ({0, from_idx});
	distances[from_idx] = 0;
	while (!q.empty ()) {
		int v = q.begin ()->second;
		_size_t base = distances[v];
		q.erase (q.begin ());
		for (int i = 0; i < nodes[v].ncount; ++i) {
			int u = nodes[v].roads[i].to->index;
			_size_t w = nodes[v].roads[i].size;
			if (distances[u] > base + w) {
				q.erase ({distances[u], u});
				distances[u] = base + w;
				parents[u] = v;
				q.insert ({distances[u], u});
			}
		}
	}
	index_t route[MAX_NODES_COUNT] = {};
	int sizes[MAX_NODES_COUNT] = {};
	size_t size{ 0 };
	for (int v = to_idx; v != from_idx; v = parents[v]) {
		route[size] = v;
		sizes[size] = distances[v] - distances[parents[v]];
		++size;
		if (parents[v] == MAX_LENGTH) {
			size = 0;
			break;
		}
	}
	path res{};
	if (size == 0) {
		res.roads = nullptr;
		res.ncount = 0;
		res.length = 0;
		goto FREE;
	}
	{
		res.roads = logger::malloc<Road>(size, ARGS);
		res.ncount = size;
		res.roads[0].to = &(nodes[route[size - 1]]);
		res.roads[0].size = sizes[size - 1];
		res.roads[0].from = &(nodes[from_idx]);
		res.length = sizes[0];
		{
			int j = 1;
			for (int i = size - 2; i >= 0; --i) {
				res.roads[j].to = &(nodes[route[i]]);
				res.roads[j].size = sizes[i];
				res.roads[j].from = res.roads[j - 1].to;
				res.length += sizes[i];
				++j;
			}
		}
	}
FREE:
	logger::free<_size_t> (distances, ncount, ARGS);
	logger::free<index_t> (parents, ncount, ARGS);
	logger::free<bool> (used, ncount, ARGS);
	return res;
}

path Graph::longest_path(name_t from, name_t to) {
	try {
		index_t from_idx = nmap[from]->index, to_idx = nmap[to]->index;
		std::vector<index_t, logger::Allocator<index_t>> topsorted (topsort (false, from_idx));
		//Полагаем, что цикла нет. Иначе нужно уточнять понятие "самый длинный путь".
		_size_t* distances = logger::malloc<_size_t>(ncount, ARGS);
		index_t* parents = logger::malloc<index_t>(ncount, ARGS);
		for (int i = 0; i < ncount; ++i) {
			distances[i] = MIN_LENGTH;
			parents[i] = MIN_LENGTH;
		}
		//Реализация через топологическую сортировку. Для произвольной вершины A со множеством B любой элемент
		//из B будет стоять до элемента A в топологической сортировке.
		//Поэтому алгоритм рабочий (в данном случае мы берём с конца перевёрнутую топологическую сортировку).
		distances[from_idx] = 0;
		while (topsorted.size() != 0) {
			int v = topsorted.back();
			topsorted.pop_back();
			if (distances[v] != MIN_LENGTH) {
				auto& node = nodes[v];
				for (int i = 0; i < node.ncount; ++i) {
					Road r = node.roads[i];
					int __to = r.to->index;
					if (distances[__to] < distances[v] + r.size) {
						parents[__to] = v;
						distances[__to] = distances[v] + r.size;
					}
				}
			}
		}
		index_t route[MAX_NODES_COUNT] = {};
		int sizes[MAX_NODES_COUNT] = {};
		size_t size{ 0 };
		for (int v = to_idx; v != from_idx; v = parents[v]) {
			route[size] = v;
			sizes[size] = distances[v] - distances[parents[v]];
			++size;
			if (parents[v] == MIN_LENGTH) {
				size = 0;
				break;
			}
		}
		path res{};
		if (size == 0) {
			res.roads = nullptr;
			res.ncount = 0;
			res.length = 0;
			goto FREE;
		}
		{
			res.roads = logger::malloc<Road>(size, ARGS);
			res.ncount = size;
			res.roads[0].to = &(nodes[route[size - 1]]);
			res.roads[0].size = sizes[size - 1];
			res.roads[0].from = &(nodes[from_idx]);
			res.length = sizes[0];
			{
				int j = 1;
				for (int i = size - 2; i >= 0; --i) {
					res.roads[j].to = &(nodes[route[i]]);
					res.roads[j].size = sizes[i];
					res.roads[j].from = res.roads[j - 1].to;
					res.length += sizes[i];
					++j;
				}
			}
		}
	FREE:
		logger::free<_size_t> (distances, ncount, ARGS);
		logger::free<index_t> (parents, ncount, ARGS);
		return res;
	} catch (...) {
		return {};
	}
}

#endif
