#ifndef GRAPH_H
#define GRAPH_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>


#ifndef __FUNCTION_NAME__
#ifdef _WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__  
#else          //*NIX
#define __FUNCTION_NAME__   __func__ 
#endif
#endif
#ifndef ARGS
#define ARGS {__FILE__, __FUNCTION_NAME__, __LINE__}
#endif

/// <summary>
/// Сложно. очень сложно. "псевдологирование" вообще всех аллокаций
/// </summary>
class logger {
public:
	struct args {
		const char* filename;
		const char* function;
		int line;
	};
	/// <summary>
	/// »спользованная куча
	/// </summary>
	/// <returns>(в байтах)</returns>
	static size_t &HEAP_USED () {
		static size_t heap_used = 0;
		return heap_used;
	}
	/// <summary>
	/// файловый дескриптор для fprintf
	/// </summary>
	/// <returns></returns>
	static FILE*& descriptor() {
		static FILE* descriptor = nullptr;
		return descriptor;
	}
	template<typename T>
	static void notify_malloc_message(size_t size, const args& argc) {
		if (descriptor != nullptr) fprintf(descriptor(), "size %zu, line = %d, function = %s, file = %s\n", size * sizeof(T), argc.line, argc.function, argc.filename);
		HEAP_USED() += size;
	}
	template <typename T>
	static T* malloc (size_t size, const args& argc) {
		T* ptr = reinterpret_cast<T*>(new char[size * sizeof(T)]);
		notify_malloc_message<T>(size, argc);
		return ptr;
	}
};


#include <map>
#include <iosfwd>
#include <vector>
#include <cstring>
#include <cstdint>
using std::map;

const size_t MAX_NODES_COUNT = 26;
const int MAX_LENGTH = (1 << 31) - 1;//01111111111111111111111111111111=2^31-1
const int MIN_LENGTH = 1 << 31;//      10000000000000000000000000000000=-2^31
const int CLEAR = -1;
const int USING = -2;
using name_t = char;
using index_t = uint32_t;
using _size_t = int32_t;

//Стандартная вершина
struct node {
	struct road {//Дорога. Хранится исходный, конечный пункт и длина
		node * from{ nullptr }, * to{ nullptr };
		_size_t size = 1;
	};
	road *roads = nullptr;
	_size_t ncount = 0;
	name_t name{ '\00' };
	index_t index = 0;
	~node() {
		if (roads != nullptr)delete[] roads;
	}
};

using Road = node::road;

//Путь - массив дорог
struct path {
	Road *roads = nullptr;
	_size_t ncount = 0, length = 0;
	path& operator+= (const path& other);
	path operator+(const path& other) const;
	path() : roads(nullptr), ncount(0), length(0) {}
	path(const path& other) : ncount(other.ncount), length(other.length), roads(logger::malloc<Road>(other.ncount, ARGS)) {
		memcpy(roads, other.roads, sizeof(Road) * ncount);//Полное копирование
	}
	~path() {
		if (roads != nullptr)delete[] roads;
	}
};
std::ostream& operator<< (std::ostream& os, const path& p);

//Структура копируется проще статического массива.
template<typename T, size_t size>
struct proxy_array {
	T arr[size] = {};
	operator const T*() const { return static_cast<const T*>(arr); }
	operator T* () { return static_cast<T*>(arr); }
};

//В C++ разница между структурами и классами только в модификаторах доступа по умолчанию.
struct Graph {
	proxy_array<node, MAX_NODES_COUNT> nodes;
	map<name_t, node*> nmap;
	_size_t ncount = 0;
	_size_t paths_count(name_t from, name_t to);
	path shortest_path(name_t from, name_t to);
	path longest_path(name_t from, name_t to);
	std::vector<index_t> topsort(bool full = false, index_t from = CLEAR);
	void invert();
};

int read(Graph& g, const char* filename);

#endif