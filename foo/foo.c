#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

unsigned char code[] = "\x48\xc7\xc0\x2a\x00\x00\x00\xc3";

int main() {
    void *mem = mmap(NULL, 4096,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_ANON | MAP_PRIVATE,
                    -1, 0);

    if (mem == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    memcpy(mem, code, sizeof(code));

    int (*func)() = mem;
    int ret = func();

    return ret;
}
