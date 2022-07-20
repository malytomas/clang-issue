#include <cstdint>
#include <cstring>
#include <cstdio>

#ifdef _MSC_VER
#include <intrin.h>
#endif

struct cpu_info
{
	cpu_info() { memset(this, 0, sizeof(*this)); }

	bool m_has_fpu;
	bool m_has_mmx;
	bool m_has_sse;
	bool m_has_sse2;
	bool m_has_sse3;
	bool m_has_ssse3;
	bool m_has_sse41;
	bool m_has_sse42;
	bool m_has_avx;
	bool m_has_avx2;
	bool m_has_pclmulqdq;
};

static void extract_x86_flags(cpu_info &info, uint32_t ecx, uint32_t edx)
{
	info.m_has_fpu = (edx & (1 << 0)) != 0;
	info.m_has_mmx = (edx & (1 << 23)) != 0;
	info.m_has_sse = (edx & (1 << 25)) != 0;
	info.m_has_sse2 = (edx & (1 << 26)) != 0;
	info.m_has_sse3 = (ecx & (1 << 0)) != 0;
	info.m_has_ssse3 = (ecx & (1 << 9)) != 0;
	info.m_has_sse41 = (ecx & (1 << 19)) != 0;
	info.m_has_sse42 = (ecx & (1 << 20)) != 0;
	info.m_has_pclmulqdq = (ecx & (1 << 1)) != 0;
	info.m_has_avx = (ecx & (1 << 28)) != 0;
}

static void extract_x86_extended_flags(cpu_info &info, uint32_t ebx)
{
	info.m_has_avx2 = (ebx & (1 << 5)) != 0;
}

#ifndef _MSC_VER
static void do_cpuid(uint32_t eax, uint32_t ecx, uint32_t *regs)
{
	uint32_t ebx = 0, edx = 0;

#if defined(__PIC__) && defined(__i386__)
	__asm__("movl %%ebx, %%edi;"
		"cpuid;"
		"xchgl %%ebx, %%edi;"
		: "=D"(ebx), "+a"(eax), "+c"(ecx), "=d"(edx));
#else
	__asm__("cpuid;" : "+b"(ebx), "+a"(eax), "+c"(ecx), "=d"(edx));
#endif

	regs[0] = eax; regs[1] = ebx; regs[2] = ecx; regs[3] = edx;
}
#endif

static void get_cpuinfo(cpu_info &info)
{
	int regs[4];

#ifdef _MSC_VER
	__cpuid(regs, 0);
#else
	do_cpuid(0, 0, (uint32_t *)regs);
#endif

	const uint32_t max_eax = regs[0];

	if (max_eax >= 1U)
	{
#ifdef _MSC_VER
		__cpuid(regs, 1);
#else
		do_cpuid(1, 0, (uint32_t *)regs);
#endif
		extract_x86_flags(info, regs[2], regs[3]);
	}

	if (max_eax >= 7U)
	{
#ifdef _MSC_VER
		__cpuidex(regs, 7, 0);
#else
		do_cpuid(7, 0, (uint32_t *)regs);
#endif

		extract_x86_extended_flags(info, regs[1]);
	}
}

bool g_cpu_supports_sse41 = false;

void detect_sse41()
{
	cpu_info info;
	get_cpuinfo(info);

	// Check for everything from SSE to SSE 4.1
	g_cpu_supports_sse41 = info.m_has_sse && info.m_has_sse2 && info.m_has_sse3 && info.m_has_ssse3 && info.m_has_sse41;
}

int main()
{
	detect_sse41();
	printf(g_cpu_supports_sse41 ? "supported\n" : "unsupported\n");
	return 0;
}
