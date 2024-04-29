#include "Graph.h"
#include <iostream>
#include <cmath>
#include <memory>

#define PAUSE do { printf("Press any key to exit..."); static_cast<void>(getchar()); } while (0)
const char name[] = "file.txt";

int main() {
    const char* STACK_BEGIN; 
    char _val{ '\00' };
	STACK_BEGIN = &_val;

    logger::descriptor() = fopen("log.txt", "w");
	Graph* g = logger::malloc<Graph> (1, ARGS);
    if (read(*g, name)) {
        std::cerr << "File not found!!!";
        PAUSE;
        return 0;
    }
    for (int i = 0; i < g->ncount; ++i) {
        printf("%c ", g->nodes[i].name);
    }
    printf("\n");
    for (int i = 0; i < g->ncount; ++i) {
        auto& n = g->nodes[i];
        printf("%c: ", n.name);
        for (int j = 0; j < n.ncount; ++j) {
            printf("%c%d ", n.roads[j].to->name, n.roads[j].size);
        }
        printf("\n");
    }
    {//Test9
        auto longest = g->longest_path('A', 'K');
        std::cout << longest;
        printf(" (%d)\n", longest.length);
    }
    const char* STACK_END;
    char __val{ '\00' };
	STACK_END = &__val;
    size_t diff = std::abs((long long)(STACK_END)-(long long)(STACK_BEGIN));
    printf ("%zu stack, %zu heap bytes used.\n", diff, logger::HEAP_USED());
	logger::free (g, 1, ARGS);
    PAUSE;
    fclose (logger::descriptor());
	logger::descriptor() = NULL;
    return 0;
}
