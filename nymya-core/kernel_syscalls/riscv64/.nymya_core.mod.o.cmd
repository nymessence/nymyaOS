savedcmd_/nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o := gcc -Wp,-MMD,/nymyaOS/nymya-core/kernel_syscalls/riscv64/.nymya_core.mod.o.d -nostdinc -I/nymyaOS/kernel-src/arch/arm64/include -I./arch/arm64/include/generated -I/nymyaOS/kernel-src/include -I./include -I/nymyaOS/kernel-src/arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I/nymyaOS/kernel-src/include/uapi -I./include/generated/uapi -include /nymyaOS/kernel-src/include/linux/compiler-version.h -include /nymyaOS/kernel-src/include/linux/kconfig.h -include /nymyaOS/kernel-src/include/linux/compiler_types.h -D__KERNEL__ -mlittle-endian -DKASAN_SHADOW_SCALE_SHIFT= -fmacro-prefix-map=/nymyaOS/kernel-src/= -std=gnu11 -fshort-wchar -funsigned-char -fno-common -fno-PIE -fno-strict-aliasing -mgeneral-regs-only -DCONFIG_CC_HAS_K_CONSTRAINT=1 -Wno-psabi -mabi=lp64 -fno-asynchronous-unwind-tables -fno-unwind-tables -mbranch-protection=pac-ret -Wa,-march=armv8.5-a -DARM64_ASM_ARCH='"armv8.5-a"' -DKASAN_SHADOW_SCALE_SHIFT= -fno-delete-null-pointer-checks -O2 -fno-allow-store-data-races -fstack-protector-strong -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-stack-clash-protection -falign-functions=4 -fno-strict-overflow -fno-stack-check -fconserve-stack -Wall -Wundef -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Werror=strict-prototypes -Wno-format-security -Wno-trigraphs -Wno-frame-address -Wno-address-of-packed-member -Wmissing-declarations -Wmissing-prototypes -Wframe-larger-than=2048 -Wno-main -Wno-unused-but-set-variable -Wno-unused-const-variable -Wvla -Wno-pointer-sign -Wcast-function-type -Wno-stringop-overflow -Wno-array-bounds -Wno-alloc-size-larger-than -Wimplicit-fallthrough=5 -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wenum-conversion -Wno-unused-but-set-variable -Wno-unused-const-variable -Wno-restrict -Wno-packed-not-aligned -Wno-format-overflow -Wno-format-truncation -Wno-stringop-truncation -Wno-missing-field-initializers -Wno-type-limits -Wno-shift-negative-value -Wno-maybe-uninitialized -Wno-sign-compare -mstack-protector-guard=sysreg -mstack-protector-guard-reg=sp_el0 -mstack-protector-guard-offset=1080 -DMODULE -DKBUILD_BASENAME='"nymya_core.mod"' -DKBUILD_MODNAME='"nymya_core"' -D__KBUILD_MODNAME=kmod_nymya_core -c -o /nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o /nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.c

source_/nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o := /nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.c

deps_/nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o := \
    $(wildcard include/config/UNWINDER_ORC) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/RETPOLINE) \
  /nymyaOS/kernel-src/include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  /nymyaOS/kernel-src/include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  /nymyaOS/kernel-src/include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/FUNCTION_ALIGNMENT) \
    $(wildcard include/config/CC_IS_GCC) \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  /nymyaOS/kernel-src/include/linux/compiler_attributes.h \
  /nymyaOS/kernel-src/include/linux/compiler-gcc.h \
    $(wildcard include/config/GCC_ASM_GOTO_OUTPUT_WORKAROUND) \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/compiler.h \
    $(wildcard include/config/ARM64_PTR_AUTH_KERNEL) \
    $(wildcard include/config/ARM64_PTR_AUTH) \
    $(wildcard include/config/BUILTIN_RETURN_ADDRESS_STRIPS_PAC) \
  /nymyaOS/kernel-src/include/linux/module.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/ARCH_USES_CFI_TRAPS) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/TRACEPOINTS) \
    $(wildcard include/config/TREE_SRCU) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/TRACING) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/FTRACE_MCOUNT_RECORD) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  /nymyaOS/kernel-src/include/linux/list.h \
    $(wildcard include/config/LIST_HARDENED) \
    $(wildcard include/config/DEBUG_LIST) \
  /nymyaOS/kernel-src/include/linux/container_of.h \
  /nymyaOS/kernel-src/include/linux/build_bug.h \
  /nymyaOS/kernel-src/include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/OBJTOOL) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/rwonce.h \
    $(wildcard include/config/LTO) \
    $(wildcard include/config/AS_HAS_LDAPR) \
  /nymyaOS/kernel-src/include/asm-generic/rwonce.h \
  /nymyaOS/kernel-src/include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  /nymyaOS/kernel-src/include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  /nymyaOS/kernel-src/include/uapi/linux/types.h \
  arch/arm64/include/generated/uapi/asm/types.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/types.h \
  /nymyaOS/kernel-src/include/asm-generic/int-ll64.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/int-ll64.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/bitsperlong.h \
  /nymyaOS/kernel-src/include/asm-generic/bitsperlong.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/bitsperlong.h \
  /nymyaOS/kernel-src/include/uapi/linux/posix_types.h \
  /nymyaOS/kernel-src/include/linux/stddef.h \
  /nymyaOS/kernel-src/include/uapi/linux/stddef.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/posix_types.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/posix_types.h \
  /nymyaOS/kernel-src/include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  /nymyaOS/kernel-src/include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  /nymyaOS/kernel-src/include/linux/const.h \
  /nymyaOS/kernel-src/include/vdso/const.h \
  /nymyaOS/kernel-src/include/uapi/linux/const.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/barrier.h \
    $(wildcard include/config/ARM64_PSEUDO_NMI) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/alternative-macros.h \
  /nymyaOS/kernel-src/include/vdso/bits.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/cpucaps.h \
    $(wildcard include/config/ARM64_PAN) \
    $(wildcard include/config/ARM64_EPAN) \
    $(wildcard include/config/ARM64_SVE) \
    $(wildcard include/config/ARM64_SME) \
    $(wildcard include/config/ARM64_CNP) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/ARM64_BTI) \
    $(wildcard include/config/ARM64_TLB_RANGE) \
    $(wildcard include/config/UNMAP_KERNEL_AT_EL0) \
    $(wildcard include/config/ARM64_ERRATUM_843419) \
    $(wildcard include/config/ARM64_ERRATUM_1742098) \
    $(wildcard include/config/ARM64_ERRATUM_2645198) \
    $(wildcard include/config/ARM64_ERRATUM_2658417) \
    $(wildcard include/config/CAVIUM_ERRATUM_23154) \
    $(wildcard include/config/NVIDIA_CARMEL_CNP_ERRATUM) \
    $(wildcard include/config/ARM64_WORKAROUND_REPEAT_TLBI) \
  arch/arm64/include/generated/asm/cpucap-defs.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/insn-def.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/brk-imm.h \
  /nymyaOS/kernel-src/include/linux/stringify.h \
  /nymyaOS/kernel-src/include/asm-generic/barrier.h \
  /nymyaOS/kernel-src/include/linux/stat.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/stat.h \
    $(wildcard include/config/COMPAT) \
  arch/arm64/include/generated/uapi/asm/stat.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/stat.h \
  /nymyaOS/kernel-src/include/linux/time.h \
    $(wildcard include/config/POSIX_TIMERS) \
  /nymyaOS/kernel-src/include/linux/cache.h \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  /nymyaOS/kernel-src/include/uapi/linux/kernel.h \
  /nymyaOS/kernel-src/include/uapi/linux/sysinfo.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/cache.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  /nymyaOS/kernel-src/include/linux/bitops.h \
  /nymyaOS/kernel-src/include/linux/bits.h \
  /nymyaOS/kernel-src/include/linux/typecheck.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/generic-non-atomic.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/bitops.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/builtin-__ffs.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/builtin-ffs.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/builtin-__fls.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/builtin-fls.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/ffz.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/fls64.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/sched.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/hweight.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/arch_hweight.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/const_hweight.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/atomic.h \
  /nymyaOS/kernel-src/include/linux/atomic.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/atomic.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/cmpxchg.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/lse.h \
    $(wildcard include/config/ARM64_LSE_ATOMICS) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/atomic_ll_sc.h \
    $(wildcard include/config/CC_HAS_K_CONSTRAINT) \
  /nymyaOS/kernel-src/include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
  /nymyaOS/kernel-src/include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/linkage.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/alternative.h \
  /nymyaOS/kernel-src/include/linux/init.h \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
    $(wildcard include/config/STRICT_KERNEL_RWX) \
    $(wildcard include/config/STRICT_MODULE_RWX) \
    $(wildcard include/config/LTO_CLANG) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/atomic_lse.h \
  /nymyaOS/kernel-src/include/linux/atomic/atomic-arch-fallback.h \
    $(wildcard include/config/GENERIC_ATOMIC64) \
  /nymyaOS/kernel-src/include/linux/atomic/atomic-long.h \
  /nymyaOS/kernel-src/include/linux/atomic/atomic-instrumented.h \
  /nymyaOS/kernel-src/include/linux/instrumented.h \
  /nymyaOS/kernel-src/include/linux/kmsan-checks.h \
    $(wildcard include/config/KMSAN) \
  /nymyaOS/kernel-src/include/asm-generic/bitops/instrumented-atomic.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/lock.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/instrumented-lock.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/non-atomic.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/non-instrumented-non-atomic.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/le.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/byteorder.h \
  /nymyaOS/kernel-src/include/linux/byteorder/little_endian.h \
  /nymyaOS/kernel-src/include/uapi/linux/byteorder/little_endian.h \
  /nymyaOS/kernel-src/include/linux/swab.h \
  /nymyaOS/kernel-src/include/uapi/linux/swab.h \
  arch/arm64/include/generated/uapi/asm/swab.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/swab.h \
  /nymyaOS/kernel-src/include/linux/byteorder/generic.h \
  /nymyaOS/kernel-src/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /nymyaOS/kernel-src/include/linux/kasan-enabled.h \
    $(wildcard include/config/KASAN) \
  /nymyaOS/kernel-src/include/linux/static_key.h \
  /nymyaOS/kernel-src/include/linux/jump_label.h \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/jump_label.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/insn.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/cputype.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/sysreg.h \
    $(wildcard include/config/BROKEN_GAS_INST) \
    $(wildcard include/config/ARM64_PA_BITS_52) \
    $(wildcard include/config/ARM64_4K_PAGES) \
    $(wildcard include/config/ARM64_16K_PAGES) \
    $(wildcard include/config/ARM64_64K_PAGES) \
  /nymyaOS/kernel-src/include/linux/kasan-tags.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/gpr-num.h \
  arch/arm64/include/generated/asm/sysreg-defs.h \
  /nymyaOS/kernel-src/include/linux/bitfield.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/mte-def.h \
  /nymyaOS/kernel-src/include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  /nymyaOS/kernel-src/include/linux/math.h \
  arch/arm64/include/generated/asm/div64.h \
  /nymyaOS/kernel-src/include/asm-generic/div64.h \
  /nymyaOS/kernel-src/include/vdso/math64.h \
  /nymyaOS/kernel-src/include/linux/time64.h \
  /nymyaOS/kernel-src/include/vdso/time64.h \
  /nymyaOS/kernel-src/include/uapi/linux/time.h \
  /nymyaOS/kernel-src/include/uapi/linux/time_types.h \
  /nymyaOS/kernel-src/include/linux/time32.h \
  /nymyaOS/kernel-src/include/linux/timex.h \
  /nymyaOS/kernel-src/include/uapi/linux/timex.h \
  /nymyaOS/kernel-src/include/uapi/linux/param.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/param.h \
  /nymyaOS/kernel-src/include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  /nymyaOS/kernel-src/include/uapi/asm-generic/param.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/timex.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/arch_timer.h \
    $(wildcard include/config/ARM_ARCH_TIMER_OOL_WORKAROUND) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/hwcap.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/hwcap.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/cpufeature.h \
    $(wildcard include/config/ARM64_SW_TTBR0_PAN) \
    $(wildcard include/config/ARM64_DEBUG_PRIORITY_MASKING) \
    $(wildcard include/config/ARM64_BTI_KERNEL) \
    $(wildcard include/config/ARM64_PA_BITS) \
    $(wildcard include/config/ARM64_HW_AFDBM) \
    $(wildcard include/config/ARM64_AMU_EXTN) \
  /nymyaOS/kernel-src/include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/bug.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/asm-bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  /nymyaOS/kernel-src/include/asm-generic/bug.h \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  /nymyaOS/kernel-src/include/linux/instrumentation.h \
    $(wildcard include/config/NOINSTR_VALIDATION) \
  /nymyaOS/kernel-src/include/linux/once_lite.h \
  /nymyaOS/kernel-src/include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  /nymyaOS/kernel-src/include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
  /nymyaOS/kernel-src/include/linux/stdarg.h \
  /nymyaOS/kernel-src/include/linux/kern_levels.h \
  /nymyaOS/kernel-src/include/linux/ratelimit_types.h \
  /nymyaOS/kernel-src/include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/spinlock_types.h \
  /nymyaOS/kernel-src/include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/NR_CPUS) \
  /nymyaOS/kernel-src/include/asm-generic/qrwlock_types.h \
  /nymyaOS/kernel-src/include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  /nymyaOS/kernel-src/include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/PROVE_LOCKING) \
  /nymyaOS/kernel-src/include/linux/align.h \
  /nymyaOS/kernel-src/include/linux/array_size.h \
  /nymyaOS/kernel-src/include/linux/limits.h \
  /nymyaOS/kernel-src/include/uapi/linux/limits.h \
  /nymyaOS/kernel-src/include/vdso/limits.h \
  /nymyaOS/kernel-src/include/linux/hex.h \
  /nymyaOS/kernel-src/include/linux/kstrtox.h \
  /nymyaOS/kernel-src/include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  /nymyaOS/kernel-src/include/linux/minmax.h \
  /nymyaOS/kernel-src/include/linux/sprintf.h \
  /nymyaOS/kernel-src/include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
  /nymyaOS/kernel-src/include/linux/instruction_pointer.h \
  /nymyaOS/kernel-src/include/linux/cpumask.h \
    $(wildcard include/config/FORCE_NR_CPUS) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
  /nymyaOS/kernel-src/include/linux/threads.h \
    $(wildcard include/config/BASE_SMALL) \
  /nymyaOS/kernel-src/include/linux/bitmap.h \
  /nymyaOS/kernel-src/include/linux/errno.h \
  /nymyaOS/kernel-src/include/uapi/linux/errno.h \
  arch/arm64/include/generated/uapi/asm/errno.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/errno.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/errno-base.h \
  /nymyaOS/kernel-src/include/linux/find.h \
  /nymyaOS/kernel-src/include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
    $(wildcard include/config/FORTIFY_SOURCE) \
  /nymyaOS/kernel-src/include/linux/err.h \
  /nymyaOS/kernel-src/include/linux/overflow.h \
  /nymyaOS/kernel-src/include/uapi/linux/string.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/string.h \
    $(wildcard include/config/ARCH_HAS_UACCESS_FLUSHCACHE) \
  /nymyaOS/kernel-src/include/linux/bitmap-str.h \
  /nymyaOS/kernel-src/include/linux/gfp_types.h \
  /nymyaOS/kernel-src/include/linux/numa.h \
    $(wildcard include/config/NODES_SHIFT) \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  /nymyaOS/kernel-src/include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
    $(wildcard include/config/DEBUG_PREEMPT) \
  /nymyaOS/kernel-src/include/linux/smp_types.h \
  /nymyaOS/kernel-src/include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  /nymyaOS/kernel-src/include/linux/preempt.h \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPTION) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
  /nymyaOS/kernel-src/include/linux/cleanup.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/preempt.h \
  /nymyaOS/kernel-src/include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/HARDENED_USERCOPY) \
    $(wildcard include/config/SH) \
  /nymyaOS/kernel-src/include/linux/restart_block.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/current.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/thread_info.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/memory.h \
    $(wildcard include/config/ARM64_VA_BITS) \
    $(wildcard include/config/KASAN_SHADOW_OFFSET) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/RANDOMIZE_BASE) \
    $(wildcard include/config/DEBUG_VIRTUAL) \
    $(wildcard include/config/EFI) \
    $(wildcard include/config/ARM_GIC_V3_ITS) \
  /nymyaOS/kernel-src/include/linux/sizes.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/page-def.h \
    $(wildcard include/config/ARM64_PAGE_SHIFT) \
  /nymyaOS/kernel-src/include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_IRQSOFF) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/boot.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/sections.h \
  /nymyaOS/kernel-src/include/asm-generic/sections.h \
    $(wildcard include/config/HAVE_FUNCTION_DESCRIPTORS) \
  /nymyaOS/kernel-src/include/asm-generic/memory_model.h \
    $(wildcard include/config/FLATMEM) \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  /nymyaOS/kernel-src/include/linux/pfn.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/stack_pointer.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/smp.h \
    $(wildcard include/config/ARM64_ACPI_PARKING_PROTOCOL) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/percpu.h \
  /nymyaOS/kernel-src/include/asm-generic/percpu.h \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  /nymyaOS/kernel-src/include/linux/percpu-defs.h \
    $(wildcard include/config/DEBUG_FORCE_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  /nymyaOS/kernel-src/include/clocksource/arm_arch_timer.h \
    $(wildcard include/config/ARM_ARCH_TIMER) \
  /nymyaOS/kernel-src/include/linux/timecounter.h \
  /nymyaOS/kernel-src/include/asm-generic/timex.h \
  /nymyaOS/kernel-src/include/vdso/time32.h \
  /nymyaOS/kernel-src/include/vdso/time.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/compat.h \
  /nymyaOS/kernel-src/include/asm-generic/compat.h \
    $(wildcard include/config/COMPAT_FOR_U64_ALIGNMENT) \
  /nymyaOS/kernel-src/include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/LRU_GEN) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_CPU_PASID) \
    $(wildcard include/config/CPU_SUP_INTEL) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DEBUG_MUTEXES) \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/SCHED_MM_CID) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/MEMCG_KMEM) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/GCC_PLUGIN_STACKLEAK) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/RV) \
    $(wildcard include/config/USER_EVENTS) \
    $(wildcard include/config/PREEMPT_NONE) \
    $(wildcard include/config/PREEMPT_VOLUNTARY) \
    $(wildcard include/config/PREEMPT) \
  /nymyaOS/kernel-src/include/uapi/linux/sched.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/processor.h \
    $(wildcard include/config/KUSER_HELPERS) \
    $(wildcard include/config/ARM64_FORCE_52BIT) \
    $(wildcard include/config/HAVE_HW_BREAKPOINT) \
    $(wildcard include/config/ARM64_TAGGED_ADDR_ABI) \
  /nymyaOS/kernel-src/include/vdso/processor.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/vdso/processor.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/hw_breakpoint.h \
    $(wildcard include/config/CPU_PM) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/virt.h \
    $(wildcard include/config/KVM) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/ptrace.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/ptrace.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/sve_context.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/kasan.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/mte-kasan.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/pgtable-types.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
  /nymyaOS/kernel-src/include/asm-generic/pgtable-nopud.h \
  /nymyaOS/kernel-src/include/asm-generic/pgtable-nop4d.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/pgtable-hwdef.h \
    $(wildcard include/config/ARM64_CONT_PTE_SHIFT) \
    $(wildcard include/config/ARM64_CONT_PMD_SHIFT) \
    $(wildcard include/config/ARM64_VA_BITS_52) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/pointer_auth.h \
  /nymyaOS/kernel-src/include/uapi/linux/prctl.h \
  /nymyaOS/kernel-src/include/linux/random.h \
    $(wildcard include/config/VMGENID) \
  /nymyaOS/kernel-src/include/uapi/linux/random.h \
  /nymyaOS/kernel-src/include/uapi/linux/ioctl.h \
  arch/arm64/include/generated/uapi/asm/ioctl.h \
  /nymyaOS/kernel-src/include/asm-generic/ioctl.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/ioctl.h \
  /nymyaOS/kernel-src/include/linux/irqnr.h \
  /nymyaOS/kernel-src/include/uapi/linux/irqnr.h \
  /nymyaOS/kernel-src/include/linux/prandom.h \
  /nymyaOS/kernel-src/include/linux/once.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/spectre.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/fpsimd.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/sigcontext.h \
  /nymyaOS/kernel-src/include/linux/irqflags_types.h \
  /nymyaOS/kernel-src/include/linux/pid_types.h \
  /nymyaOS/kernel-src/include/linux/sem_types.h \
  /nymyaOS/kernel-src/include/linux/shm.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/page.h \
  /nymyaOS/kernel-src/include/linux/personality.h \
  /nymyaOS/kernel-src/include/uapi/linux/personality.h \
  /nymyaOS/kernel-src/include/asm-generic/getorder.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/shmparam.h \
  /nymyaOS/kernel-src/include/asm-generic/shmparam.h \
  /nymyaOS/kernel-src/include/linux/kmsan_types.h \
  /nymyaOS/kernel-src/include/linux/mutex_types.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
  /nymyaOS/kernel-src/include/linux/osq_lock.h \
  /nymyaOS/kernel-src/include/linux/spinlock_types.h \
  /nymyaOS/kernel-src/include/linux/rwlock_types.h \
  /nymyaOS/kernel-src/include/linux/plist_types.h \
  /nymyaOS/kernel-src/include/linux/hrtimer_types.h \
  /nymyaOS/kernel-src/include/linux/timerqueue_types.h \
  /nymyaOS/kernel-src/include/linux/rbtree_types.h \
  /nymyaOS/kernel-src/include/linux/timer_types.h \
  /nymyaOS/kernel-src/include/linux/seccomp_types.h \
    $(wildcard include/config/SECCOMP) \
  /nymyaOS/kernel-src/include/linux/nodemask_types.h \
  /nymyaOS/kernel-src/include/linux/refcount_types.h \
  /nymyaOS/kernel-src/include/linux/resource.h \
  /nymyaOS/kernel-src/include/uapi/linux/resource.h \
  arch/arm64/include/generated/uapi/asm/resource.h \
  /nymyaOS/kernel-src/include/asm-generic/resource.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/resource.h \
  /nymyaOS/kernel-src/include/linux/latencytop.h \
  /nymyaOS/kernel-src/include/linux/sched/prio.h \
  /nymyaOS/kernel-src/include/linux/sched/types.h \
  /nymyaOS/kernel-src/include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  /nymyaOS/kernel-src/include/uapi/linux/signal.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/signal.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/signal.h \
  /nymyaOS/kernel-src/include/asm-generic/signal.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/signal.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/signal-defs.h \
  arch/arm64/include/generated/uapi/asm/siginfo.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/siginfo.h \
  /nymyaOS/kernel-src/include/linux/syscall_user_dispatch_types.h \
  /nymyaOS/kernel-src/include/linux/mm_types_task.h \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
    $(wildcard include/config/SPLIT_PTLOCK_CPUS) \
    $(wildcard include/config/ARCH_ENABLE_SPLIT_PMD_PTLOCK) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/tlbbatch.h \
  /nymyaOS/kernel-src/include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  /nymyaOS/kernel-src/include/linux/posix-timers_types.h \
  /nymyaOS/kernel-src/include/uapi/linux/rseq.h \
  /nymyaOS/kernel-src/include/linux/seqlock_types.h \
  /nymyaOS/kernel-src/include/linux/kcsan.h \
  /nymyaOS/kernel-src/include/linux/rv.h \
    $(wildcard include/config/RV_REACTORS) \
  /nymyaOS/kernel-src/include/linux/livepatch_sched.h \
  /nymyaOS/kernel-src/include/linux/uidgid_types.h \
  arch/arm64/include/generated/asm/kmap_size.h \
  /nymyaOS/kernel-src/include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  /nymyaOS/kernel-src/include/linux/spinlock.h \
  /nymyaOS/kernel-src/include/linux/irqflags.h \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/irqflags.h \
  /nymyaOS/kernel-src/include/linux/bottom_half.h \
  /nymyaOS/kernel-src/include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  arch/arm64/include/generated/asm/mmiowb.h \
  /nymyaOS/kernel-src/include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/spinlock.h \
  arch/arm64/include/generated/asm/qspinlock.h \
  /nymyaOS/kernel-src/include/asm-generic/qspinlock.h \
  arch/arm64/include/generated/asm/qrwlock.h \
  /nymyaOS/kernel-src/include/asm-generic/qrwlock.h \
  /nymyaOS/kernel-src/include/linux/rwlock.h \
  /nymyaOS/kernel-src/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  /nymyaOS/kernel-src/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  /nymyaOS/kernel-src/include/linux/sched/task_stack.h \
    $(wildcard include/config/STACK_GROWSUP) \
    $(wildcard include/config/DEBUG_STACK_USAGE) \
  /nymyaOS/kernel-src/include/uapi/linux/magic.h \
  /nymyaOS/kernel-src/include/linux/refcount.h \
  /nymyaOS/kernel-src/include/uapi/linux/stat.h \
  /nymyaOS/kernel-src/include/linux/uidgid.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/USER_NS) \
  /nymyaOS/kernel-src/include/linux/highuid.h \
  /nymyaOS/kernel-src/include/linux/buildid.h \
    $(wildcard include/config/CRASH_CORE) \
  /nymyaOS/kernel-src/include/linux/kmod.h \
  /nymyaOS/kernel-src/include/linux/umh.h \
  /nymyaOS/kernel-src/include/linux/gfp.h \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/CONTIG_ALLOC) \
  /nymyaOS/kernel-src/include/linux/mmzone.h \
    $(wildcard include/config/ARCH_FORCE_MAX_ORDER) \
    $(wildcard include/config/CMA) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/UNACCEPTED_MEMORY) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/LRU_GEN_STATS) \
    $(wildcard include/config/LRU_GEN_WALKS_MMU) \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  /nymyaOS/kernel-src/include/linux/list_nulls.h \
  /nymyaOS/kernel-src/include/linux/wait.h \
  /nymyaOS/kernel-src/include/linux/seqlock.h \
  /nymyaOS/kernel-src/include/linux/mutex.h \
  /nymyaOS/kernel-src/include/linux/debug_locks.h \
  /nymyaOS/kernel-src/include/linux/nodemask.h \
  /nymyaOS/kernel-src/include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  /nymyaOS/kernel-src/include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/sparsemem.h \
  /nymyaOS/kernel-src/include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/ANON_VMA_NAME) \
    $(wildcard include/config/PER_VMA_LOCK) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/IOMMU_MM_DATA) \
    $(wildcard include/config/KSM) \
  /nymyaOS/kernel-src/include/linux/auxvec.h \
  /nymyaOS/kernel-src/include/uapi/linux/auxvec.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/auxvec.h \
  /nymyaOS/kernel-src/include/linux/kref.h \
  /nymyaOS/kernel-src/include/linux/rbtree.h \
  /nymyaOS/kernel-src/include/linux/rcupdate.h \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/RCU_LAZY) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/KVM_XFER_TO_GUEST_WORK) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  /nymyaOS/kernel-src/include/linux/context_tracking_irq.h \
    $(wildcard include/config/CONTEXT_TRACKING_IDLE) \
  /nymyaOS/kernel-src/include/linux/rcutree.h \
  /nymyaOS/kernel-src/include/linux/maple_tree.h \
    $(wildcard include/config/MAPLE_RCU_DISABLED) \
    $(wildcard include/config/DEBUG_MAPLE_TREE) \
  /nymyaOS/kernel-src/include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  /nymyaOS/kernel-src/include/linux/completion.h \
  /nymyaOS/kernel-src/include/linux/swait.h \
  /nymyaOS/kernel-src/include/linux/uprobes.h \
  /nymyaOS/kernel-src/include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/WQ_WATCHDOG) \
  /nymyaOS/kernel-src/include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
  /nymyaOS/kernel-src/include/linux/ktime.h \
  /nymyaOS/kernel-src/include/linux/jiffies.h \
  /nymyaOS/kernel-src/include/vdso/jiffies.h \
  include/generated/timeconst.h \
  /nymyaOS/kernel-src/include/vdso/ktime.h \
  /nymyaOS/kernel-src/include/linux/timekeeping.h \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  /nymyaOS/kernel-src/include/linux/clocksource_ids.h \
  /nymyaOS/kernel-src/include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  /nymyaOS/kernel-src/include/linux/workqueue_types.h \
  /nymyaOS/kernel-src/include/linux/percpu_counter.h \
  /nymyaOS/kernel-src/include/linux/percpu.h \
    $(wildcard include/config/RANDOM_KMALLOC_CACHES) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/mmu.h \
  /nymyaOS/kernel-src/include/linux/page-flags.h \
    $(wildcard include/config/ARCH_USES_PG_UNCACHED) \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/ARCH_USES_PG_ARCH_X) \
    $(wildcard include/config/HUGETLB_PAGE_OPTIMIZE_VMEMMAP) \
  /nymyaOS/kernel-src/include/linux/local_lock.h \
  /nymyaOS/kernel-src/include/linux/local_lock_internal.h \
  /nymyaOS/kernel-src/include/linux/zswap.h \
    $(wildcard include/config/ZSWAP) \
  /nymyaOS/kernel-src/include/linux/memory_hotplug.h \
    $(wildcard include/config/HAVE_ARCH_NODEDATA_EXTENSION) \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  /nymyaOS/kernel-src/include/linux/notifier.h \
  /nymyaOS/kernel-src/include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/NEED_SRCU_NMI_SAFE) \
  /nymyaOS/kernel-src/include/linux/rcu_segcblist.h \
  /nymyaOS/kernel-src/include/linux/srcutree.h \
  /nymyaOS/kernel-src/include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  /nymyaOS/kernel-src/include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
  /nymyaOS/kernel-src/include/linux/arch_topology.h \
    $(wildcard include/config/ACPI_CPPC_LIB) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/topology.h \
  /nymyaOS/kernel-src/include/asm-generic/topology.h \
  /nymyaOS/kernel-src/include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  /nymyaOS/kernel-src/include/uapi/linux/sysctl.h \
  /nymyaOS/kernel-src/include/linux/elf.h \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/elf.h \
    $(wildcard include/config/COMPAT_VDSO) \
  arch/arm64/include/generated/asm/user.h \
  /nymyaOS/kernel-src/include/asm-generic/user.h \
  /nymyaOS/kernel-src/include/uapi/linux/elf.h \
  /nymyaOS/kernel-src/include/uapi/linux/elf-em.h \
  /nymyaOS/kernel-src/include/linux/fs.h \
    $(wildcard include/config/READ_ONLY_THP_FOR_FS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/CGROUP_WRITEBACK) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/UNICODE) \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/BLOCK) \
  /nymyaOS/kernel-src/include/linux/wait_bit.h \
  /nymyaOS/kernel-src/include/linux/kdev_t.h \
  /nymyaOS/kernel-src/include/uapi/linux/kdev_t.h \
  /nymyaOS/kernel-src/include/linux/dcache.h \
  /nymyaOS/kernel-src/include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  /nymyaOS/kernel-src/include/linux/rculist_bl.h \
  /nymyaOS/kernel-src/include/linux/list_bl.h \
  /nymyaOS/kernel-src/include/linux/bit_spinlock.h \
  /nymyaOS/kernel-src/include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  /nymyaOS/kernel-src/include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  /nymyaOS/kernel-src/include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  /nymyaOS/kernel-src/include/linux/path.h \
  /nymyaOS/kernel-src/include/linux/list_lru.h \
  /nymyaOS/kernel-src/include/linux/shrinker.h \
    $(wildcard include/config/SHRINKER_DEBUG) \
  /nymyaOS/kernel-src/include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  /nymyaOS/kernel-src/include/linux/sched/mm.h \
    $(wildcard include/config/MMU_LAZY_TLB_REFCOUNT) \
    $(wildcard include/config/ARCH_HAS_MEMBARRIER_CALLBACKS) \
  /nymyaOS/kernel-src/include/linux/sync_core.h \
    $(wildcard include/config/ARCH_HAS_SYNC_CORE_BEFORE_USERMODE) \
  /nymyaOS/kernel-src/include/linux/radix-tree.h \
  /nymyaOS/kernel-src/include/linux/pid.h \
  /nymyaOS/kernel-src/include/linux/capability.h \
  /nymyaOS/kernel-src/include/uapi/linux/capability.h \
  /nymyaOS/kernel-src/include/linux/semaphore.h \
  /nymyaOS/kernel-src/include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  /nymyaOS/kernel-src/include/uapi/linux/fcntl.h \
  /nymyaOS/kernel-src/arch/arm64/include/uapi/asm/fcntl.h \
  /nymyaOS/kernel-src/include/uapi/asm-generic/fcntl.h \
  /nymyaOS/kernel-src/include/uapi/linux/openat2.h \
  /nymyaOS/kernel-src/include/linux/migrate_mode.h \
  /nymyaOS/kernel-src/include/linux/percpu-rwsem.h \
  /nymyaOS/kernel-src/include/linux/rcuwait.h \
  /nymyaOS/kernel-src/include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
  /nymyaOS/kernel-src/include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
    $(wildcard include/config/PROC_FS) \
  /nymyaOS/kernel-src/include/linux/sched/jobctl.h \
  /nymyaOS/kernel-src/include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  /nymyaOS/kernel-src/include/linux/uaccess.h \
    $(wildcard include/config/ARCH_HAS_SUBPAGE_FAULTS) \
  /nymyaOS/kernel-src/include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/uaccess.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/kernel-pgtable.h \
    $(wildcard include/config/RELOCATABLE) \
  /nymyaOS/kernel-src/arch/arm64/include/asm/asm-extable.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/mte.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/extable.h \
    $(wildcard include/config/BPF_JIT) \
  /nymyaOS/kernel-src/include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  /nymyaOS/kernel-src/include/linux/cred.h \
  /nymyaOS/kernel-src/include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  /nymyaOS/kernel-src/include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  /nymyaOS/kernel-src/include/linux/sched/user.h \
    $(wildcard include/config/VFIO_PCI_ZDEV_KVM) \
    $(wildcard include/config/IOMMUFD) \
    $(wildcard include/config/WATCH_QUEUE) \
  /nymyaOS/kernel-src/include/linux/ratelimit.h \
  /nymyaOS/kernel-src/include/linux/posix-timers.h \
  /nymyaOS/kernel-src/include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  /nymyaOS/kernel-src/include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  /nymyaOS/kernel-src/include/linux/hrtimer_defs.h \
  /nymyaOS/kernel-src/include/linux/timerqueue.h \
  /nymyaOS/kernel-src/include/linux/rcu_sync.h \
  /nymyaOS/kernel-src/include/linux/delayed_call.h \
  /nymyaOS/kernel-src/include/linux/uuid.h \
  /nymyaOS/kernel-src/include/linux/errseq.h \
  /nymyaOS/kernel-src/include/linux/ioprio.h \
  /nymyaOS/kernel-src/include/linux/sched/rt.h \
  /nymyaOS/kernel-src/include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  /nymyaOS/kernel-src/include/uapi/linux/ioprio.h \
  /nymyaOS/kernel-src/include/linux/fs_types.h \
  /nymyaOS/kernel-src/include/linux/mount.h \
  /nymyaOS/kernel-src/include/linux/mnt_idmapping.h \
  /nymyaOS/kernel-src/include/linux/slab.h \
    $(wildcard include/config/SLUB_DEBUG) \
    $(wildcard include/config/SLUB_TINY) \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/KFENCE) \
  /nymyaOS/kernel-src/include/linux/percpu-refcount.h \
  /nymyaOS/kernel-src/include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
  /nymyaOS/kernel-src/include/uapi/linux/fs.h \
  /nymyaOS/kernel-src/include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  /nymyaOS/kernel-src/include/uapi/linux/dqblk_xfs.h \
  /nymyaOS/kernel-src/include/linux/dqblk_v1.h \
  /nymyaOS/kernel-src/include/linux/dqblk_v2.h \
  /nymyaOS/kernel-src/include/linux/dqblk_qtree.h \
  /nymyaOS/kernel-src/include/linux/projid.h \
  /nymyaOS/kernel-src/include/uapi/linux/quota.h \
  /nymyaOS/kernel-src/include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  /nymyaOS/kernel-src/include/linux/sysfs.h \
  /nymyaOS/kernel-src/include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  /nymyaOS/kernel-src/include/linux/idr.h \
  /nymyaOS/kernel-src/include/linux/kobject_ns.h \
  /nymyaOS/kernel-src/include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/PPC64) \
  /nymyaOS/kernel-src/include/linux/rbtree_latch.h \
  /nymyaOS/kernel-src/include/linux/error-injection.h \
  /nymyaOS/kernel-src/include/asm-generic/error-injection.h \
  /nymyaOS/kernel-src/include/linux/tracepoint-defs.h \
  /nymyaOS/kernel-src/include/linux/dynamic_debug.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/module.h \
    $(wildcard include/config/DYNAMIC_FTRACE) \
  /nymyaOS/kernel-src/include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
    $(wildcard include/config/MODULES_USE_ELF_REL) \
    $(wildcard include/config/MODULES_USE_ELF_RELA) \
  /nymyaOS/kernel-src/include/linux/build-salt.h \
    $(wildcard include/config/BUILD_SALT) \
  /nymyaOS/kernel-src/include/linux/elfnote.h \
  /nymyaOS/kernel-src/include/linux/elfnote-lto.h \
  /nymyaOS/kernel-src/include/linux/export-internal.h \
    $(wildcard include/config/PARISC) \
  /nymyaOS/kernel-src/include/linux/vermagic.h \
    $(wildcard include/config/PREEMPT_BUILD) \
  include/generated/utsrelease.h \
  /nymyaOS/kernel-src/arch/arm64/include/asm/vermagic.h \

/nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o: $(deps_/nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o)

$(deps_/nymyaOS/nymya-core/kernel_syscalls/riscv64/nymya_core.mod.o):
