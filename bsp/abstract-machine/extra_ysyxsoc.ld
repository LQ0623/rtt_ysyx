MEMORY {
  sram  : ORIGIN = 0x0f000000, LENGTH = 8K
  mrom  : ORIGIN = 0x20000000, LENGTH = 4K
  flash : ORIGIN = 0x30000000, LENGTH = 16M
  psram : ORIGIN = 0x80000000, LENGTH = 4M
  sdram : ORIGIN = 0xa0000000, LENGTH = 32M
}

SECTIONS {
  .data.extra : {
    /* section information for finsh shell */
    __fsymtab_start = .;
    KEEP(*(FSymTab))
    __fsymtab_end = .;
    . = ALIGN(8);
    __vsymtab_start = .;
    KEEP(*(VSymTab))
    __vsymtab_end = .;
    . = ALIGN(8);

    /* section information for initial. */
    . = ALIGN(8);
    __rt_init_start = .;
    KEEP(*(SORT(.rti_fn*)))
    __rt_init_end = .;
    . = ALIGN(8);

    __rt_utest_tc_tab_start = .;
    KEEP(*(UtestTcTab))
    __rt_utest_tc_tab_end = .;

    . = ALIGN(8);
    __am_apps_data_start = .;
    *(__am_apps.data*)
    *(__am_apps.sdata*)
    __am_apps_data_end = .;
    . = ALIGN(8);
  } > sdram AT > flash : data

  /* 获取rtt的data段的虚拟内存地址(VMA) */
  _data_extra       = ADDR(.data.extra);
  _edata_extra      = _data_extra + SIZEOF(.data.extra);
  /* 获取rtt的data段的加载内存地址(LMA) */
  _data_extra_lma   = LOADADDR(.data.extra);
  _edata_extra_lma  = _data_extra_lma + SIZEOF(.data.extra);  /* 等于号不能直接连接到变量上面，不然就报错 */
} 
INSERT BEFORE .data;

SECTIONS {
  .bss.extra (NOLOAD) : {
    . = ALIGN(8);
    __am_apps_bss_start = .;
    *(__am_apps.bss*)
    *(__am_apps.sbss*)
    *(__am_apps.scommon*)
    __am_apps_bss_end = .;
    . = ALIGN(8);
  } > sdram

  /* 获取rtt的bss段的虚拟内存地址(VMA) */
  _bss_extra  = ADDR(.bss.extra);
  _ebss_extra = _bss_extra + SIZEOF(.bss.extra);
}
INSERT BEFORE .bss;
