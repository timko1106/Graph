#include "Graph.h"
#include <iostream>
#include <cmath>
#include <memory>
#include <cstdio>

#define CATCH_SEGFAULT //only Linux
#ifdef CATCH_SEGFAULT
#include <signal.h>
#include <ucontext.h>
char get (int val) {
	return val >= 10 ? 'a' + (val - 10) : '0' + val;
}
void handler (int signal, siginfo_t* info, void* ptr) {
	ucontext_t* context = (ucontext_t*)ptr;
	printf ("SIGSEGV %d, SIGFPE, %d, SIGILL %d, SIGBUS %d, SIGCHLD %d\n", SIGSEGV, SIGFPE, SIGILL, SIGBUS, SIGCHLD);
	printf ("Signal %d, errno %d, code %d, architecture %u\n", info->si_signo, info->si_errno, info->si_code, info->si_arch);
	switch (signal) {
	case SIGSEGV:
	case SIGFPE:
	case SIGILL:
	case SIGBUS:
		printf ("address %p, lsb %d, %p lowaddr, %p upaddr, %u pkey\n", info->si_addr, (int)info->si_addr_lsb, info->si_lower, info->si_upper, info->si_pkey);
		break;
	case SIGCHLD:
		printf ("SIGCHLD: pid %d, uid %u, status %d\n", info->si_pid, info->si_uid, info->si_status);
		break;
	default:
		printf ("Not stated\n");
		break;
	}
	printf ("Context: %p, stack: %p\nUcontext:\n", context->uc_link, context->uc_stack.ss_sp);
	size_t size = sizeof (ucontext_t);
	char* s = (char*)ptr;
	for (size_t i = 0; i < size; ++i) {

			printf ("%c%c ", get(((unsigned char)*s) / 16), get (((unsigned char)*s) % 16));
			++s;
	}
	fclose (logger::descriptor ());
	printf ("\nSaved\n");
	return;
}
#endif

#define PAUSE do { printf("Press any key to exit..."); static_cast<void>(getchar()); } while (0)
const char name[] = "file.txt";


int main () {
#ifdef CATCH_SEGFAULT
	{
		struct sigaction s{};
	    s.sa_flags = SA_SIGINFO|SA_RESETHAND;
		s.sa_sigaction = handler;
		sigemptyset(&s.sa_mask);
		int signals[] = {SIGSEGV, SIGFPE, SIGILL, SIGBUS, SIGCHLD};
		for (int i = 0; i < sizeof (signals) / sizeof (*signals); ++i) {
			sigaction (signals[i], &s, (struct sigaction*)0);
		}
	}
#endif
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
    /*{//Test9
        auto longest = g->longest_path('A', 'K');
        std::cout << longest;
        printf(" (%d)\n", longest.length);
    }*/
	//int x = 6 / 0;
	
    /*{//Test8
        auto shortest = g->shortest_path('A', 'G') + g->shortest_path('G', 'K');
        std::cout << shortest;
        printf(" (%d)\n", shortest.length);
    }*/
    
    /*{//Test7
        auto longest = g->longest_path('A', 'K');
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
    
    /*{//Test3
        int count = g->paths_count('A', 'H') * g->paths_count('H', 'L');
        std::cout << count << '\n';
    }*/
    /*
    {//Test2
        auto longest = g->longest_path('A', 'J');
        std::cout << longest;
        printf(" (%d)\n", longest.length);
    }*/
    
    //Test1
    {
        {
            int count = g->paths_count('A', 'C') * g->paths_count('C', 'L');
            std::cout << count << '\n';
            auto shortest = g->shortest_path('A', 'C') + g->shortest_path('C', 'L');
            std::cout << shortest;
            printf("(%d)\n", shortest.length);
            auto longest = g->longest_path('A', 'C') + g->longest_path('C', 'L');
            std::cout << longest;
            printf("(%d)\n", longest.length);
        }
		g->invert();
        {
            int count = (g->paths_count('L', 'C')) * (g->paths_count('C', 'A'));
            std::cout << count << '\n';
            auto shortest = g->shortest_path('L', 'C') + g->shortest_path('C', 'A');
            std::cout << shortest;
            printf("(%d)\n", shortest.length);
            auto longest = g->longest_path('L', 'C') + g->longest_path('C', 'A');
            std::cout << longest;
            printf("(%d)\n", longest.length);
        }
		Graph* cpy = logger::malloc<Graph>(1, ARGS);
		(*cpy) = std::move (*g);
		std::cout << (cpy->longest_path ('A', 'C') + cpy->longest_path ('C', 'L')) << '\n';
		std::cout << (cpy->longest_path ('L', 'C') + cpy->longest_path ('C', 'A')) << '\n';
		logger::free<Graph> (cpy, 1, ARGS);
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
