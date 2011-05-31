/*****************************************************************************/
/* FAME Fast and Accurate Motorola 68000 Emulation Core                      */
/* (c) 2005 Oscar Orallo Pelaez                                              */
/* Version: 1.24                                                             */
/* Date: 08-20-2005                                                          */
/* See FAME.HTML for documentation and license information                   */
/*****************************************************************************/

#ifndef __FAME_H__
#define __FAME_H__

#ifdef __cplusplus
extern "C" {
#endif


/************************************/
/* General library defines          */
/************************************/

#ifndef M68K_OK
    #define M68K_OK 0
#endif
#ifndef M68K_RUNNING
    #define M68K_RUNNING 1
#endif
#ifndef M68K_NO_SUP_ADDR_SPACE
    #define M68K_NO_SUP_ADDR_SPACE 2
#endif
#ifndef M68K_DOUBLE_BUS_FAULT
    #define M68K_DOUBLE_BUS_FAULT -1
#endif
#ifndef M68K_INV_REG
    #define M68K_INV_REG -1
#endif

/* Hardware interrupt state */

#ifndef M68K_IRQ_LEVEL_ERROR
    #define M68K_IRQ_LEVEL_ERROR -1
#endif
#ifndef M68K_IRQ_INV_PARAMS
    #define M68K_IRQ_INV_PARAMS -2
#endif

/* Defines to specify hardware interrupt type */

#ifndef M68K_AUTOVECTORED_IRQ
    #define M68K_AUTOVECTORED_IRQ -1
#endif
#ifndef M68K_SPURIOUS_IRQ
    #define M68K_SPURIOUS_IRQ -2
#endif

#ifndef M68K_AUTO_LOWER_IRQ
	#define M68K_AUTO_LOWER_IRQ 1
#endif
#ifndef M68K_MANUAL_LOWER_IRQ
	#define M68K_MANUAL_LOWER_IRQ 0
#endif

/* Defines to specify address space */

#ifndef M68K_SUP_ADDR_SPACE
    #define M68K_SUP_ADDR_SPACE 0
#endif
#ifndef M68K_USER_ADDR_SPACE
    #define M68K_USER_ADDR_SPACE 2
#endif
#ifndef M68K_PROG_ADDR_SPACE
    #define M68K_PROG_ADDR_SPACE 0
#endif
#ifndef M68K_DATA_ADDR_SPACE
    #define M68K_DATA_ADDR_SPACE 1
#endif


/*******************/
/* Data definition */
/*******************/

/* M68K registers */
typedef enum {
      M68K_REG_D0=0,
      M68K_REG_D1,
      M68K_REG_D2,
      M68K_REG_D3,
      M68K_REG_D4,
      M68K_REG_D5,
      M68K_REG_D6,
      M68K_REG_D7,
      M68K_REG_A0,
      M68K_REG_A1,
      M68K_REG_A2,
      M68K_REG_A3,
      M68K_REG_A4,
      M68K_REG_A5,
      M68K_REG_A6,
      M68K_REG_A7,
      M68K_REG_ASP,
      M68K_REG_PC,
      M68K_REG_SR
} m68k_register;

/* The memory blocks must be in native (Motorola) format */
struct M68K_PROGRAM {
	unsigned low_addr;
	unsigned high_addr;
	unsigned offset;
};

/* The memory blocks must be in native (Motorola) format */
struct M68K_DATA {
	unsigned low_addr;
	unsigned high_addr;
	void    *mem_handler;
	void    *data;
};

/* M68K CPU CONTEXT */
struct M68K_CONTEXT {
	struct M68K_PROGRAM *fetch;
	struct M68K_DATA *read_byte;
	struct M68K_DATA *read_word;
	struct M68K_DATA *write_byte;
	struct M68K_DATA *write_word;
	struct M68K_PROGRAM *sv_fetch;
	struct M68K_DATA *sv_read_byte;

	struct M68K_DATA *sv_read_word;
	struct M68K_DATA *sv_write_byte;
	struct M68K_DATA *sv_write_word;
	struct M68K_PROGRAM *user_fetch;
	struct M68K_DATA *user_read_byte;
	struct M68K_DATA *user_read_word;
	struct M68K_DATA *user_write_byte;
	struct M68K_DATA *user_write_word;
	void           (*reset_handler)(void);
	void           (*iack_handler)(unsigned level);
	unsigned *     icust_handler;
	unsigned       dreg[8];
	unsigned       areg[8];
	unsigned       asp;
	unsigned       pc;
	unsigned       cycles_counter;
	unsigned char  interrupts[8];
	unsigned short sr;
	unsigned short execinfo;
};


/************************/
/* Function definition  */
/************************/

/* General purpose functions */
void     m68k_init(void);
unsigned m68k_reset(void);
unsigned m68k_emulate(int n);
unsigned m68k_get_pc(void);
unsigned m68k_get_cpu_state(void);
int      m68k_fetch(unsigned address, unsigned memory_space);

/* Interrupt handling functions */
int  m68k_raise_irq(int level, int vector);
int  m68k_lower_irq(int level);
void m68k_burst_irq(int mask, int vector);
void m68k_set_irq_type(void *context, int type);
int  m68k_get_irq_vector(int level);
int  m68k_change_irq_vector(int level, int vector);

/* CPU context handling functions */
int  m68k_get_context_size(void);
void m68k_get_context(void *context);
void m68k_set_context(void *context);
int  m68k_get_register(m68k_register reg);
int  m68k_set_register(m68k_register reg, unsigned value);

/* Timing functions */
unsigned m68k_get_cycles_counter(void);
unsigned m68k_trip_cycles_counter(void);
unsigned m68k_control_cycles_counter(int n);
void     m68k_release_timeslice(void);
void     m68k_add_cycles(int cycles);
void     m68k_release_cycles(int cycles);


#ifdef __cplusplus
}
#endif

#endif
