// SPDX-License-Identifier: GPL-2.0
#ifndef __SAMSUNG_H
#define __SAMSUNG_H

/*
 * Driver for Samsung SoC onboard UARTs.
 *
 * Ben Dooks, Copyright (c) 2003-2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
*/

#include <linux/pm_qos.h>

#define S3C24XX_UART_PORT_RESUME		0x0
#define S3C24XX_UART_PORT_SUSPEND		0x3
#define S3C24XX_UART_PORT_LPM			0x5

#define S3C24XX_SERIAL_CTRL_NUM			0x4
#define S3C24XX_SERIAL_BUAD_NUM			0x2

struct s3c24xx_uart_dma {
    unsigned int            rx_chan_id;
    unsigned int            tx_chan_id;

    struct dma_slave_config     rx_conf;
    struct dma_slave_config     tx_conf;

    struct dma_chan         *rx_chan;
    struct dma_chan         *tx_chan;

    dma_addr_t          rx_addr;
    dma_addr_t          tx_addr;

    dma_cookie_t            rx_cookie;
    dma_cookie_t            tx_cookie;

    char                *rx_buf;

    dma_addr_t          tx_transfer_addr;

    size_t              rx_size;
    size_t              tx_size;

    struct dma_async_tx_descriptor  *tx_desc;
    struct dma_async_tx_descriptor  *rx_desc;

    int             tx_bytes_requested;
    int             rx_bytes_requested;
};


struct s3c24xx_uart_info {
	char			*name;
	unsigned int		type;
	unsigned int		fifosize;
	unsigned long		rx_fifomask;
	unsigned long		rx_fifoshift;
	unsigned long		rx_fifofull;
	unsigned long		tx_fifomask;
	unsigned long		tx_fifoshift;
	unsigned long		tx_fifofull;
	unsigned int		rts_trig_shift;
	unsigned int		def_clk_sel;
	unsigned long		num_clks;
	unsigned long		clksel_mask;
	unsigned long		clksel_shift;

	/* uart port features */

	unsigned int		has_divslot:1;

	/* uart controls */
	int (*reset_port)(struct uart_port *, struct s3c2410_uartcfg *);
};

struct s3c24xx_serial_drv_data {
	struct s3c24xx_uart_info	*info;
	struct s3c2410_uartcfg		*def_cfg;
	unsigned int			fifosize[CONFIG_SERIAL_SAMSUNG_UARTS];
};

struct uart_local_buf {
	unsigned char *buffer;
	unsigned int size;
	unsigned int index;
};

struct s3c24xx_uart_port {
	struct list_head		node;
	unsigned char			rx_claimed;
	unsigned char			tx_claimed;
	unsigned long			baudclk_rate;
    unsigned int            min_dma_size;

    unsigned int            tx_in_progress;
	unsigned int			rx_irq;
	unsigned int			tx_irq;
    unsigned int            tx_mode;
    unsigned int            rx_mode;

	int				check_separated_clk;
	unsigned int			src_clk_rate;
	struct s3c24xx_uart_info	*info;
	struct clk			*clk;
	struct clk			*separated_clk;
	struct clk			*baudclk;
	struct uart_port		port;
	struct s3c24xx_serial_drv_data	*drv_data;

	u32				uart_irq_affinity;
	s32				mif_qos_val;
	s32				cpu_qos_val;
	u32				use_default_irq;
	unsigned long			qos_timeout;
	unsigned int			usi_v2;
	unsigned int			uart_panic_log;
	struct pinctrl_state 	*uart_pinctrl_tx_dat;
	struct pinctrl_state 	*uart_pinctrl_rts;
	struct pinctrl_state 	*uart_pinctrl_default;
	struct pinctrl *default_uart_pinctrl;
	unsigned int		rts_control;
	unsigned int		rts_trig_level;

	struct regmap			*usi_reg;
	unsigned int			usi_offset;

	/* reference to platform data */
	struct s3c2410_uartcfg		*cfg;

	struct platform_device		*pdev;

    struct s3c24xx_uart_dma     *dma;

	struct pm_qos_request		s3c24xx_uart_mif_qos;
	struct pm_qos_request		s3c24xx_uart_cpu_qos;
	struct delayed_work		qos_work;

	unsigned int			in_band_wakeup;
	unsigned int dbg_mode;
	unsigned int			uart_logging;
	struct uart_local_buf		uart_local_buf;

	unsigned int dbg_uart_ch;
	unsigned int dbg_uart_baud;
	unsigned int dbg_word_len;
};

/* conversion functions */

#define s3c24xx_dev_to_port(__dev) dev_get_drvdata(__dev)

/* register access controls */

#define portaddr(port, reg) ((port)->membase + (reg))
#define portaddrl(port, reg) \
	((unsigned long *)(unsigned long)((port)->membase + (reg)))

#define rd_regb(port, reg) (readb_relaxed(portaddr(port, reg)))
#define rd_regl(port, reg) (readl_relaxed(portaddr(port, reg)))

#define wr_regb(port, reg, val) writeb_relaxed(val, portaddr(port, reg))
#define wr_regl(port, reg, val) writel_relaxed(val, portaddr(port, reg))

static inline void s3c24xx_set_bit(struct uart_port *port, int idx,
                   unsigned int reg)
{
    unsigned long flags;
    u32 val;

    local_irq_save(flags);
    val = rd_regl(port, reg);
    val |= (1 << idx);
    wr_regl(port, reg, val);
    local_irq_restore(flags);
}

static inline void s3c24xx_clear_bit(struct uart_port *port, int idx,
                     unsigned int reg)
{
    unsigned long flags;
    u32 val;

    local_irq_save(flags);
    val = rd_regl(port, reg);
    val &= ~(1 << idx);
    wr_regl(port, reg, val);
    local_irq_restore(flags);
}


#endif
