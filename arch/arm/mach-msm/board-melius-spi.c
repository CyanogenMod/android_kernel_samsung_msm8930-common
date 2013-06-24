/* arch/arm/mach-capri/board-baffin-spi.c
 *
 * Copyright (C) 2011 Samsung Electronics Co, Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/gpio.h>


#include <linux/irq.h>
#include <linux/spi/spi.h>
#include <mach/msm8930-gpio.h>
#include <linux/platform_device.h>


struct ipc_spi_platform_data {
	const char *name;
	unsigned gpio_ipc_mrdy;
	unsigned gpio_ipc_srdy;
	unsigned gpio_ipc_sub_mrdy;
	unsigned gpio_ipc_sub_srdy;

	void (*cfg_gpio)(void);
};

static struct ipc_spi_platform_data spi_modem_data;

struct bcom_mcspi_device_config {
	unsigned turbo_mode:1;
	/* Do we want one channel enabled at the same time? */
	unsigned single_channel:1;
	};

static struct bcom_mcspi_device_config board_spi_mcspi_config = {
	.turbo_mode =	1,
	.single_channel = 1,
};

static struct spi_board_info modem_if_spi_device[] __initdata = {
	{
		.modalias	= "if_spi_driver",
		.bus_num=0,	
		.max_speed_hz	= 12000000,
		.chip_select	= 0,
		.mode           = SPI_MODE_1,
		.controller_data = &board_spi_mcspi_config,
		//.controller_data = (void *) SPI_GPIO_NO_CHIPSELECT,
		
	},
};

static struct platform_device spi_modem = {
	.name = "if_spi_platform_driver",
	.id = -1,
	.dev = {
		.platform_data = &spi_modem_data,
	},
};




static void __init spi_modem_cfg_gpio(void)
{
	int ret;
	ret=gpio_request(GPIO_IPC_MRDY, "GPIO_IPC_MRDY");
	if (ret) {
				pr_err("gspi_modem_cfg: gpio_request "
					"failed for %d\n",GPIO_IPC_MRDY);
				;
			}
	ret=gpio_direction_output(GPIO_IPC_MRDY, 0);
	if (ret) {
				pr_err("spi_modem_cfg: gpio_direction_input failed"
					" for input %d\n",GPIO_IPC_MRDY);
			
			}
	ret= gpio_request(GPIO_IPC_SUB_MRDY, "GPIO_IPC_SUB_MRDY");
	if (ret) {
				pr_err("spi_modem_cfg: gpio_request "
					"failed for %d\n",GPIO_IPC_SUB_MRDY);
				;
			}
	ret=0;
	gpio_direction_output(GPIO_IPC_SUB_MRDY, 0);
	if (ret) {
				pr_err("spi_modem_cfg: gpio_direction_input failed"
					" for input %d\n",GPIO_IPC_SUB_MRDY);
			
			}
	ret=gpio_request(GPIO_IPC_SRDY, "GPIO_IPC_SRDY");
	if (ret) {
				pr_err("spi_modem_cfg_: gpio_request "
					"failed for %d\n",GPIO_IPC_SRDY);
				;
			}
	ret=0;
	gpio_direction_input(GPIO_IPC_SRDY);
	if (ret) {
				pr_err("spi_modem_cfg: gpio_direction_input failed"
					" for input %d\n",GPIO_IPC_SRDY);
			
			}
	ret=gpio_request(GPIO_IPC_SUB_SRDY, "GPIO_IPC_SUB_SRDY");
	if (ret) {
				pr_err("spi_modem_cfg: gpio_request "
					"failed for %d\n",GPIO_IPC_SUB_SRDY);
				;
			}
	ret=0;
	gpio_direction_input(GPIO_IPC_SUB_SRDY);
	if (ret) {
				pr_err("spi_modem_cfg: gpio_direction_input failed"
					" for input %d\n",GPIO_IPC_SUB_SRDY);
			
			}

	spi_modem_data.gpio_ipc_mrdy = GPIO_IPC_MRDY;
	spi_modem_data.gpio_ipc_srdy = GPIO_IPC_SRDY;
	spi_modem_data.gpio_ipc_sub_mrdy = GPIO_IPC_SUB_MRDY;
	spi_modem_data.gpio_ipc_sub_srdy = GPIO_IPC_SUB_SRDY;

	pr_info("[SPI] %s done\n", __func__);

}

static int __init init_spi(void)
{
	pr_info("[SPI] %s\n", __func__);
	spi_modem_cfg_gpio();
	platform_device_register(&spi_modem);

	spi_register_board_info(modem_if_spi_device,
		ARRAY_SIZE(modem_if_spi_device));
	return 0;
}

module_init(init_spi);
