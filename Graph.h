#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>


#ifndef __FUNCTION_NAME__
#ifdef _WIN32   //WINDOWS
#define _CRT_SECURE_NO_WARNINGS
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
private:
	template<typename T>
	static void notify_malloc_message(T* ptr, size_t size, const args& argc, bool alloc) {
		if (descriptor() != NULL) {
			fprintf(descriptor(), "%s %zu bytes at %p, line = %d, function = %s, file = %s\n", alloc ? "Allocate" : "Deallocate", size, (void*)ptr, argc.line, argc.function, argc.filename);
		}
	}
public:
	template <typename T>
	static T* malloc (size_t size, const args& argc) {
		T* ptr;
		if (size == 1) {
			ptr = new T{};
		} else {
			ptr = new T[size];
		}
		size *= sizeof (T);
		notify_malloc_message<T>(ptr, size, argc, true);
		HEAP_USED() += size;
		return ptr;
	}
	template<typename T>
	static void free (T* ptr, size_t size, const args& argc) {
		if (size == 1)  {
			delete ptr;
		} else {
			delete[] ptr;
		}
		size *= sizeof (T);
		notify_malloc_message (ptr, size, argc, false);
	}
	template<typename T>
	struct Allocator {
		using value_type = T;
		Allocator () {}
		T* allocate (size_t count) {
			return logger::malloc<T> (count, ARGS);
		}
		void deallocate (T* ptr, size_t count) {
			logger::free<T> (ptr, count, ARGS);
		}
	};
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
using name_t = unsigned char;
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
		if (roads != nullptr) {
			logger::free (roads, ncount, ARGS);
		}
	}
};
using node_ptr = node*;

using Road = node::road;

//Путь - массив дорог
struct path {
	Road *roads = nullptr;
	_size_t ncount = 0, length = 0;
	path& operator+= (const path& other);
	path operator+(const path& other) const;
	path() : roads(nullptr), ncount(0), length(0) {}
	path(const path& other) : roads(logger::malloc<Road>(other.ncount, ARGS)), ncount(other.ncount), length(other.length) {
		memcpy(roads, other.roads, sizeof(Road) * ncount);//Полное копирование
	}
	~path() {
		if (roads != nullptr) {
			logger::free (roads, ncount, ARGS);
		}
	}
};
std::ostream& operator<< (std::ostream& os, const path& p);

//Структура копируется проще статического массива.
template<typename T, size_t size>
struct proxy_array {
	T arr[size] = {};
	operator const T*() const {
		return static_cast<const T*>(arr);

	}
	operator T* () {
		return static_cast<T*>(arr);
	}
};

//В C++ разница между структурами и классами только в модификаторах доступа по умолчанию.
struct Graph {
	proxy_array<node, MAX_NODES_COUNT> nodes;
	node_ptr nmap[256] = {};
	//map<name_t, node*> nmap;
	_size_t ncount = 0;
	_size_t paths_count(name_t from, name_t to);
	path shortest_path(name_t from, name_t to);
	path longest_path(name_t from, name_t to);
	std::vector<index_t, logger::Allocator<index_t>> topsort(bool full = false, index_t from = CLEAR);
	void invert();
};

int read(Graph& g, const char* filename);

#endif
