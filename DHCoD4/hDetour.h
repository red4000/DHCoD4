#ifndef HDETOUR_H
#define HDETOUR_H

void *DetourFunction(void *from, void *to, void **tramp = 0);    // returns a ptr to a trampoline for the detoured function
void *DetourFunctionJmp(void *from, void *to, void **tramp = 0); // returns a ptr to a trampoline for the detoured function
void RetourFunction(void *from, void *tramp);                    // unhooks a function
void *DetourForSteam(void *from, void *to);                      // checks if the *from is already hooked, and if it is, then follows the hook and writes a hook there.

size_t GetTrampolineLength(void *at, size_t lenOfHook);
void *BuildTrampoline(void *at, size_t len);

#define DETOURFUNC(x, y)          (x != 0 ? DetourFunction((void *)(x), (void *)(y)) : 0)
#define DETOURFUNCJ(x, y)         (x != 0 ? DetourFunctionJmp((void *)(x), (void *)(y)) : 0)

#define CREATE_1INSTR_TRAMP(x)    BuildTrampoline((x), GetTrampolineLength((x), 1))


#define ALLOCATE_UNREADABLE_PAGE VirtualAlloc(NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_WRITECOPY)

#define PRESERVE    _asm pushad /*\
                                 * _asm pushf*/

#define RESTORE   \
   /*_asm popf */ \
   _asm popad

#endif
