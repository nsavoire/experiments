SECTIONS
{   
    .foo1 : ALIGN(0x8000) {}
    .data : {}
    .foo2 : ALIGN(0x8000) {}
    .bss : {}
} INSERT BEFORE .text;
