SECTIONS {
   .vers (COPY): {} /* version information */
}

-priority
--trampolines
--"diag_suppress=16002"

/*-llnknone.a64P*/
/*-lrtdx64xplus.lib*/      /* RTDX support */
-lrts64plus.lib        /* C and C++ run-time library support */

-stack 0x1000

MEMORY {
   L1DSRAM      : origin = 0x10f04000, len = 0xc000
   MLB_SYSCALLS : origin = 0x86001000, len = 0x1000  /* must match DSP_SYSCALLS_SIZE */
}
