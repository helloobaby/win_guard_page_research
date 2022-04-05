    1: kd> !vad
    VAD Level Start End  Commit
    ffffa18f6191f4b0  4   7ffe0   7ffe0   1 Private  READONLY   
    ffffa18f6191ef60  3   7ffee   7ffee   1 Private  READONLY   
    ffffa18f6191f3c0  4 acc8c30 acc8d2f   6 Private  READWRITE  
    ffffa18f6191efb0  2 acc8e00 acc8fff   7 Private  READWRITE  
    ffffa18f61920720  3 acc9000 acc90ff   4 Private  READWRITE  
    ffffa18f61920310  4 acc9100 acc91ff   4 Private  READWRITE  
    ffffa18f61747060  11dbadd101dbadd1f   0 Mapped   READWRITE  Pagefile section, shared commit 0x10
    ffffa18f61747380  31dbadd201dbadd22   0 Mapped   READONLY   \Windows\System32\l_intl.nls
    ffffa18f617465c0  21dbadd301dbadd4e   0 Mapped   READONLY   Pagefile section, shared commit 0x1f
    ffffa18f61745e40  41dbadd501dbadd53   0 Mapped   READONLY   Pagefile section, shared commit 0x4
    ffffa18f617467a0  31dbadd601dbadd60   0 Mapped   READONLY   Pagefile section, shared commit 0x1
    ffffa18f6191f190  41dbadd701dbadd71   2 Private  READWRITE  
    ffffa18f61746b60  01dbadd801dbaddb0   0 Mapped   READONLY   \Windows\System32\C_936.NLS
    ffffa18f61747420  41dbaddc01dbaddc2   0 Mapped   READONLY   \Windows\System32\l_intl.nls
    ffffa18f6191f5a0  31dbaddd01dbadddc   2 Private  READWRITE  
    ffffa18f6191f690  41dbadde01dbadedf  17 Private  READWRITE  
    ffffa18f61747740  21dbadee01dbadfad   0 Mapped   READONLY   \Windows\System32\locale.nls
    ffffa18f617480a0  41dbadfb01dbadfe0   0 Mapped   READONLY   \Windows\System32\C_936.NLS
    ffffa18f61920860  31dbadff01dbadff0   0 Private  EXECUTE_READWRITE  
    ffffa18f61920810  51dbae0001dbae000   0 Private  EXECUTE_READWRITE GUARD 
    ffffa18f617471a0  4   7ff45b390   7ff45b48f   0 Mapped   READONLY   Pagefile section, shared commit 0x5
    ffffa18f6191f500  1   7ff45b490   7ff55b4af   0 Private  READWRITE  
    ffffa18f6191f410  4   7ff55b4b0   7ff55d4b0   1 Private  READWRITE  
    ffffa18f61746520  3   7ff55d4c0   7ff55d4c0   0 Mapped   READONLY   Pagefile section, shared commit 0x1
    ffffa18f617463e0  2   7ff7e5860   7ff7e5867   2 Mapped  Exe  EXECUTE_WRITECOPY  \Users\sbc\Desktop\alloc_test.exe
    ffffa18f61749360  4   7ff8e09c0   7ff8e0a58   6 Mapped  Exe  EXECUTE_WRITECOPY  \Windows\System32\msvcp140.dll
    ffffa18f617494a0  5   7ff8e37c0   7ff8e37d5   2 Mapped  Exe  EXECUTE_WRITECOPY  \Windows\System32\vcruntime140.dll
    ffffa18f61746ca0  3   7ff8ef530   7ff8ef8a3   9 Mapped  Exe  EXECUTE_WRITECOPY  \Windows\System32\KernelBase.dll
    ffffa18f617238e0  5   7ff8efac0   7ff8efbd0   4 Mapped  Exe  EXECUTE_WRITECOPY  \Windows\System32\ucrtbase.dll
    ffffa18f61746c00  4   7ff8f1570   7ff8f162c   7 Mapped  Exe  EXECUTE_WRITECOPY  \Windows\System32\kernel32.dll
    ffffa18f61745bc0  5   7ff8f1ee0   7ff8f20e6  15 Mapped  Exe  EXECUTE_WRITECOPY  \Windows\System32\ntdll.dll
    
    Total VADs: 31, average level: 4, maximum depth: 5
    Total private commit: 0x5a pages (360 KB)
    Total shared commit:  0x3a pages (232 KB)


用VirtualAlloc申请的内存具有Private属性


## VirtualAlloc出来的内存，不使用的话pte都invalid的，但是vad中有记载，应该是第一次使用触发#pf，然后异常中挂好pte

## windows PAGE_GUARD是如何实现的?
Vad->u.VadFlags.Protection = ProtectionMask; //vad中记录(PAGE_GUARD),只有第一次访问的时候会出异常?

    #define MM_ZERO_ACCESS 0  // this value is not used.
    #define MM_READONLY1
    #define MM_EXECUTE 2
    #define MM_EXECUTE_READ3
    #define MM_READWRITE   4  // bit 2 is set if this is writable.
    #define MM_WRITECOPY   5
    #define MM_EXECUTE_READWRITE   6
    #define MM_EXECUTE_WRITECOPY   7
    
    #define MM_NOCACHE0x8
    #define MM_GUARD_PAGE 0x10
    #define MM_DECOMMIT   0x10   //NO_ACCESS, Guard page
    #define MM_NOACCESS   0x18   //NO_ACCESS, Guard_page, nocache.
    #define MM_UNKNOWN_PROTECTION 0x100  //bigger than 5 bits!
    #define MM_LARGE_PAGES0x111

    _MMPTE_SOFTWARE.Protection : 5;  
    VAD中的Protection也是5位（2^5 = 32）

							ULONG MmProtectToValue[32] = {
                            PAGE_NOACCESS, 
                            PAGE_READONLY,
                            PAGE_EXECUTE,
                            PAGE_EXECUTE_READ,
                            PAGE_READWRITE,
                            PAGE_WRITECOPY,
                            PAGE_EXECUTE_READWRITE,
                            PAGE_EXECUTE_WRITECOPY,
                            PAGE_NOACCESS,
                            PAGE_NOCACHE | PAGE_READONLY,
                            PAGE_NOCACHE | PAGE_EXECUTE,
                            PAGE_NOCACHE | PAGE_EXECUTE_READ,
                            PAGE_NOCACHE | PAGE_READWRITE,
                            PAGE_NOCACHE | PAGE_WRITECOPY,
                            PAGE_NOCACHE | PAGE_EXECUTE_READWRITE,
                            PAGE_NOCACHE | PAGE_EXECUTE_WRITECOPY,
                            PAGE_NOACCESS,
                            PAGE_GUARD | PAGE_READONLY,
                            PAGE_GUARD | PAGE_EXECUTE,
                            PAGE_GUARD | PAGE_EXECUTE_READ,
                            PAGE_GUARD | PAGE_READWRITE,
                            PAGE_GUARD | PAGE_WRITECOPY,
                            PAGE_GUARD | PAGE_EXECUTE_READWRITE,
                            PAGE_GUARD | PAGE_EXECUTE_WRITECOPY,
                            PAGE_NOACCESS,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_READONLY,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_EXECUTE,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_EXECUTE_READ,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_READWRITE,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_WRITECOPY,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_EXECUTE_READWRITE,
                            PAGE_NOCACHE | PAGE_GUARD | PAGE_EXECUTE_WRITECOPY
                          };


0: kd> !pte 1ca84260000
   VA 000001ca84260000
PXE at FFFF844221108018PPE at FFFF844221003950PDE at FFFF84420072A108PTE at FFFF8400E5421300
contains 0A00000035169867  contains 0A0000007B0EA867  contains 0A0000004100A867  contains 0000000000000220
pfn 35169 ---DA--UWEV  pfn 7b0ea ---DA--UWEV  pfn 4100a ---DA--UWEV  not valid
  DemandZero
  Protect: 11 - Readonly G

0: kd> !pte 1ca84260000
   VA 000001ca84260000
PXE at FFFF844221108018PPE at FFFF844221003950PDE at FFFF84420072A108PTE at FFFF8400E5421300
contains 0A00000035169867  contains 0A0000007B0EA867  contains 0A0000004100A867  contains 80000000606C9025
pfn 35169 ---DA--UWEV  pfn 7b0ea ---DA--UWEV  pfn 4100a ---DA--UWEV  pfn 606c9     ----A--UR-V

VirtualProtect不动VAD的，所以!vad看不出来东西的，直接看soft pte的protection，windbg可以解析出来的,
PAGE_GUARD只有一次机会，进#pf后会取消他的。

PAGE_NOACESS也差不多，进veh或者seh重新VirtualProtect