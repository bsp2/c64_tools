SECTIONS {
        .bss:     {} > AREA_RAM

        .far:     {} > AREA_RAM

        .sysdata: {} > AREA_RAM

        .dsm: {} > AREA_RAM

        frt:    {} > AREA_RAM

        .mem:     {} > AREA_RAM

        .sysmem:  {} > AREA_RAM

        .cio:     {} > AREA_RAM

        .data:    {} > AREA_RAM

        .const:   {} > AREA_RAM

        .gio:     {} > AREA_RAM

        .pinit:   {} > AREA_RAM

        .sys:     {} > AREA_RAM

        .sysregs: {} > AREA_RAM

        .text:    {} > AREA_RAM

        .cinit:    {} > AREA_RAM

        .switch:    {} > AREA_RAM

        .gblinit:    {} > AREA_RAM

        .sysinit:    {} > AREA_RAM

        .rtdx_text: {}  > AREA_RAM

        .stack: align = 0x8 {
            GBL_stackbeg = .;
            *(.stack)
            GBL_stackend = GBL_stackbeg + 0x1000 - 1;
            _HWI_STKBOTTOM = GBL_stackbeg + 0x1000 - 8;
            _HWI_STKTOP = GBL_stackbeg;
        } > AREA_RAM
}

SECTIONS {
    .sec_mlb_syscalls :> MLB_SYSCALLS
    .sec_com          :> AREA_COM
}
