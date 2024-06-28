change

void z_riscv_pmp_init(void)
{
	unsigned long pmp_addr[4];
	unsigned long pmp_cfg[1];
	unsigned int index = 0;

	/* The read and write area is always there for every mode */
	set_pmp_entry(&index, PMP_R | PMP_X | PMP_W | PMP_L,
		      (uintptr_t)__rom_region_start,
		      (size_t)__rom_region_size,
		      pmp_addr, pmp_cfg, ARRAY_SIZE(pmp_addr));

to

void z_riscv_pmp_init(void)
{
	unsigned long pmp_addr[4];
	unsigned long pmp_cfg[1];
	unsigned int index = 0;

	/* The read and write area is always there for every mode */
	set_pmp_entry(&index, PMP_R | PMP_X | PMP_L,
		      (uintptr_t)__rom_region_start,
		      (size_t)__rom_region_size,
		      pmp_addr, pmp_cfg, ARRAY_SIZE(pmp_addr));

end