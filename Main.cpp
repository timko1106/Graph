#include "Graph.h"
#include <iostream>

#define PAUSE() do { printf("Press any key to exit..."); static_cast<void>(getchar()); } while (0)
const char name[] = "file.txt";

int main() {
    const char* STACK_BEGIN; 
    char _val{ '\00' }; STACK_BEGIN = &_val;

    logger::descriptor() = fopen("log.txt", "w");
    Graph g{};
    if (read(g, name)) {
        std::cerr << "File not found!!!";
        PAUSE();
        return 0;
    }
    for (int i = 0; i < g.ncount; ++i) {
        printf("%c ", g.nodes[i].name);
    }
    printf("\n");
    for (int i = 0; i < g.ncount; ++i) {
        auto& n = g.nodes[i];
        printf("%c: ", n.name);
        for (int j = 0; j < n.ncount; ++j) {
            printf("%c%d ", n.roads[j].to->name, n.roads[j].size);
        }
        printf("\n");
    }
    {//Test9
        auto longest = g.longest_path('A', 'K');
        std::cout << longest;
        printf(" (%d)\n", longest.length);
    }
    /*
    {//Test8
        auto shortest = g.shortest_path('A', 'G') + g.shortest_path('G', 'K');
        std::cout << shortest;
        printf(" (%d)\n", shortest.length);
    }*/
    /*
    {//Test7
        auto longest = g.longest_path('A', 'K');
        std::cout << longest << " (" << longest.ncount << ')' << '\n';
    }*/
    /* 
    {//Test6
        int count = g.paths_count('A', 'K');
        std::cout << count << '\n';
    }*/
    /*
    {//Test5
        int count = g.paths_count('A', 'L');
        std::cout << count << '\n';
    }*/
    /*
    {//Test4
        int count = g.paths_count('A', 'J');
        std::cout << count << '\n';
    }*/
    /*
    {//Test3
        int count = g.paths_count('A', 'H') * g.paths_count('H', 'L');
        std::cout << count << '\n';
    }*/
    /*
    {//Test2
        auto longest = g.longest_path('A', 'J');
        std::cout << longest;
        printf(" (%d)\n", longest.length);
    }*/
    /*
    //Test1
    {
        {
            int count = g.paths_count('A', 'C') * g.paths_count('C', 'L');
            std::cout << count << '\n';

            auto shortest = g.shortest_path('A', 'C') + g.shortest_path('C', 'L');
            std::cout << shortest;
            printf("(%d)\n", shortest.length);

            auto longest = g.longest_path('A', 'C') + g.longest_path('C', 'L');
            std::cout << longest;
            printf("(%d)\n", longest.length);
        }

        g.invert();
        {
            int count = g.paths_count('L', 'C') * g.paths_count('C', 'A');
            std::cout << count << '\n';

            auto shortest = g.shortest_path('L', 'C') + g.shortest_path('C', 'A');
            std::cout << shortest;
            printf("(%d)\n", shortest.length);

            auto longest = g.longest_path('L', 'C') + g.longest_path('C', 'A');
            std::cout << longest;
            printf("(%d)\n", longest.length);
        }
    }*/
    const char* STACK_END;
    char __val{ '\00' }; STACK_END = &__val;


    unsigned long long diff = abs((long long)(STACK_END)-(long long)(STACK_BEGIN));
    printf("%zu stack, %zu heap bytes used.\n", diff, logger::HEAP_USED());
    fclose(logger::descriptor());
    PAUSE();
    return 0;
}