void test_suite() {
    // 1. PERFECT CASE: Valid mnemonic, valid registers, correct clobbers
    asm("mov %%rax, %%rbx" : : : "rax", "rbx");

    // 2. MNEMONIC ERROR: Using 'move' (Intel/common typo) instead of 'mov'
    asm("move %%rax, %%rsi" : : : "rax", "rsi");

    // 3. REGISTER NAME ERROR: 'raxz' is not a real x86-64 register
    // Note: Clang's internal parser will also catch this!
    asm("mov %%raxz, %%rdi" : : : "raxz", "rdi");

    // 4. CLOBBER ERROR: 'rcx' is used but missing from the clobber list
    asm("add %%rcx, %%rdx" : : : "rdx");

    // 5. STACK ALIGNMENT: Unbalanced pushes and pops
    asm("push %%r8" : : : "r8");  // Depth: 1
    asm("push %%r9" : : : "r9");  // Depth: 2
    asm("pop %%r9"  : : : "r9");  // Depth: 1
    // Missing a pop for r8 here!
}

void test_underflow() {
    // 6. STACK UNDERFLOW: Popping more than you push
    asm("pop %%r10" : : : "r10"); 
}
