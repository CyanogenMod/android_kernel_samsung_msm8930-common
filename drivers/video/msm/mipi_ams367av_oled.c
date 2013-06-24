/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/lcd.h>
#include <linux/wakelock.h>
#include "mipi_ams367av_oled.h"
#include "mdp4.h"

#if defined(CONFIG_FB_MDP4_ENHANCE)
#include "mdp4_video_enhance.h"
#elif defined(CONFIG_MDNIE_LITE_TUNING)
#include "mdnie_lite_tuning.h"
#endif

#define DDI_VIDEO_ENHANCE_TUNING
#if defined(DDI_VIDEO_ENHANCE_TUNING)
#include <linux/syscalls.h>
#endif

static struct mipi_samsung_driver_data msd;
static unsigned int recovery_boot_mode;

#define WA_FOR_FACTORY_MODE
#define READ_MTP_ONCE

#define CONFIG_HBM_PSRE_DEBUG 1

unsigned char bypass_lcd_id;
static char elvss_value;
int is_lcd_connected = 1;
struct mutex dsi_tx_mutex;

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
static int current_fps;
static int goal_fps = 0;
#endif

#ifdef USE_READ_ID

static char manufacture_id1[2] = {0xDA, 0x00}; /* DTYPE_DCS_READ */
static char manufacture_id2[2] = {0xDB, 0x00}; /* DTYPE_DCS_READ */
static char manufacture_id3[2] = {0xDC, 0x00}; /* DTYPE_DCS_READ */

static struct dsi_cmd_desc samsung_manufacture_id1_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id1), manufacture_id1};
static struct dsi_cmd_desc samsung_manufacture_id2_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id2), manufacture_id2};
static struct dsi_cmd_desc samsung_manufacture_id3_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id3), manufacture_id3};

static uint32 mipi_samsung_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dsi_buf *rp, *tp;

	struct dsi_cmd_desc *cmd;
	uint32 id;

	mutex_lock(&dsi_tx_mutex);

	tp = &msd.samsung_tx_buf;
	rp = &msd.samsung_rx_buf;
	mipi_dsi_buf_init(rp);
	mipi_dsi_buf_init(tp);

	cmd = &samsung_manufacture_id1_cmd;
	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);

/*	pr_info("%s: manufacture_id1=%x\n", __func__, *rp->data); */
	id = *((uint8 *)rp->data);
	id <<= 8;

	mipi_dsi_buf_init(rp);
	mipi_dsi_buf_init(tp);

	cmd = &samsung_manufacture_id2_cmd;
	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);

/*	pr_info("%s: manufacture_id2=%x\n", __func__, *rp->data); */
	bypass_lcd_id = *rp->data;
	id |= *((uint8*)rp->data);
	id <<= 8;

	mipi_dsi_buf_init(rp);
	mipi_dsi_buf_init(tp);

	cmd = &samsung_manufacture_id3_cmd;
	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);

/*	pr_info("%s: manufacture_id3=%x\n", __func__, *rp->data); */
	elvss_value = *rp->data;
	id |= *((uint8 *)rp->data);

	pr_info("%s: manufacture_id=%x\n", __func__, id);

#ifdef FACTORY_TEST
	if (id == 0x00) {
		pr_info("Lcd is not connected\n");
		is_lcd_connected = 0;
	}
#endif
	mutex_unlock(&dsi_tx_mutex);
	return id;
}
#endif

static char *mean_mipi_cmd(enum mipi_samsung_cmd_list cmd)
{
	static char *dbg_str;
	switch (cmd) {
	case PANEL_ON:
		dbg_str = "PANEL_ON";
		break;

	case PANEL_OFF:
		dbg_str = "PANEL_OFF";
		break;

	case PANEL_BRIGHT_CTRL:
		dbg_str = "PANEL_BRIGHT_CTRL";
		break;

	case PANEL_MTP_ENABLE:
		dbg_str = "PANEL_MTP_ENABLE";
		break;

	case PANEL_MTP_DISABLE:
		dbg_str = "PANEL_MTP_DISABLE";
		break;

	case PANEL_NEED_FLIP:
		dbg_str = "PANEL_NEED_FLIP";
		break;

	case PANEL_ACL_CONTROL:
		dbg_str = "PANEL_ACL_CONTROL";
		break;

	case PANLE_TEMPERATURE:
		dbg_str = "PANLE_TEMPERATURE";
		break;

	case PANLE_TOUCH_KEY:
		dbg_str = "PANLE_TOUCH_KEY";
		break;

	default:
		dbg_str = "UNKNOWN";
		break;
	}

	return dbg_str;
}

static int mipi_samsung_disp_send_cmd(struct msm_fb_data_type *mfd,
		enum mipi_samsung_cmd_list cmd,
		unsigned char lock)
{
	struct dsi_cmd_desc *cmd_desc;
	struct dcs_cmd_req cmdreq;
	int cmd_size = 0;

#ifdef CMD_DEBUG
	int i,j;
#endif

	/* while lcd is off, ignore brightness cmd
	 * because, it make LCD not to on
	 */
	if (mfd->resume_state == MIPI_SUSPEND_STATE) {
		switch (cmd) {
		case PANEL_BRIGHT_CTRL:
			pr_err("%s : 0x%d return (MIPI_SUSPEND_STATE)\n", __func__, cmd);
			return -EPERM;
		default:
			break;
		}
	}

	pr_info("%s cmd = 0x%x %s\n", __func__, cmd, mean_mipi_cmd(cmd));

	if (mfd->panel.type == MIPI_VIDEO_PANEL)
		mutex_lock(&dsi_tx_mutex);
	else {
		if (lock)
			mutex_lock(&mfd->dma->ov_mutex);
	}

		switch (cmd) {
		case PANEL_ON:
			cmd_desc = msd.mpd->on.cmd;
			cmd_size = msd.mpd->on.size;
			break;

		case PANEL_OFF:
			cmd_desc = msd.mpd->off.cmd;
			cmd_size = msd.mpd->off.size;
			break;

#if 0	/*temp*/
		case PANEL_LATE_ON:
			cmd_desc = msd.mpd->late_on.cmd;
			cmd_size = msd.mpd->late_on.size;
			break;
		case PANEL_EARLY_OFF:
			cmd_desc = msd.mpd->early_off.cmd;
			cmd_size = msd.mpd->early_off.size;
			break;
#endif
		case PANEL_BRIGHT_CTRL:
			cmd_desc = msd.mpd->brightness.cmd;
			cmd_size = msd.mpd->brightness.size;
			break;

		case PANEL_MTP_ENABLE:
			cmd_desc = msd.mpd->mtp_enable.cmd;
			cmd_size = msd.mpd->mtp_enable.size;
			break;

		case PANEL_MTP_DISABLE:
			cmd_desc = msd.mpd->mtp_disable.cmd;
			cmd_size = msd.mpd->mtp_disable.size;
			break;

		case PANEL_NEED_FLIP:
			cmd_desc = msd.mpd->need_flip.cmd;
			cmd_size = msd.mpd->need_flip.size;
			break;

		case PANEL_ACL_CONTROL:
			cmd_desc = msd.mpd->acl_cmds.cmd;
			cmd_size = msd.mpd->acl_cmds.size;
			break;

		case PANLE_TEMPERATURE:
			cmd_desc = msd.mpd->temperature.cmd;
			cmd_size = msd.mpd->temperature.size;
			break;

		case PANLE_TOUCH_KEY:
			cmd_desc = msd.mpd->touch_key.cmd;
			cmd_size = msd.mpd->touch_key.size;
			break;

		default:
			pr_info("%s UNKNOW CMD", __func__);
			goto unknown_command;
			;
	}

#ifdef CMD_DEBUG
	if (cmd == PANEL_BRIGHT_CTRL || cmd == PANEL_ACL_CONTROL) {
		pr_info("+++ cmd_size = %d\n",cmd_size);
		for(i=0; i<cmd_size; i++){
			printk("cmd[%d] : ",i);
			for(j=0; j<cmd_desc[i].dlen; j++)
				printk("%x ",cmd_desc[i].payload[j]);
			printk("\n");
		}
		pr_info("--- end\n");
	}
#endif

	if (!cmd_size)
		goto unknown_command;

	cmdreq.cmds = cmd_desc;
	cmdreq.cmds_cnt = cmd_size;
	if (cmd == PANEL_BRIGHT_CTRL)
		cmdreq.flags = CMD_REQ_COMMIT | CMD_REQ_SINGLE_TX;
	else
		cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

	if (mfd->panel.type == MIPI_VIDEO_PANEL)
		mutex_unlock(&dsi_tx_mutex);
	else {
		if (lock)
			mutex_unlock(&mfd->dma->ov_mutex);
	}
	return 0;

unknown_command:
	if (mfd->panel.type == MIPI_VIDEO_PANEL)
		mutex_unlock(&dsi_tx_mutex);
	else {
		if (lock)
			mutex_unlock(&mfd->dma->ov_mutex);
	}

	return 0;
}

static void read_mtp(char srcReg, int readFrom, int srcLength, char *destBuffer,
				struct msm_fb_data_type *pMFD)
{
	struct dcs_cmd_req cmdreq;
	int one_read_size = 6;
	int loop_limit = (srcLength / one_read_size) + 1;

	/* first byte = read-register */
	char read_reg[2] = { 0xFF, 0x00 };
	struct dsi_cmd_desc read_reg_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(read_reg), read_reg };

	/* first byte is size of Register */
	char packet_size[] = { 0x00, 0x00 };
	struct dsi_cmd_desc packet_size_cmd = {
	DTYPE_MAX_PKTSIZE, 1, 0, 0, 0, sizeof(packet_size), packet_size };

	/* second byte is Read-position */
	char reg_read_pos[] = { 0xB0, 0x00 };
	struct dsi_cmd_desc read_pos_cmd = {
	DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(reg_read_pos), reg_read_pos };

	int read_pos;
	int readed_size;
	int show_cnt;

	int read_size, read_loop;
	char show_buffer[256] = {0,};
	int show_buffer_pos = 0;

	mutex_lock(&dsi_tx_mutex);

	read_reg[0] = srcReg;

	show_buffer_pos +=
	    snprintf(show_buffer, 256, "read_reg : %X[%d] : ",
		 srcReg, srcLength);

	read_pos = readFrom;
	readed_size = 0;

	packet_size[0] = (char)srcLength;
	mipi_dsi_buf_init(&msd.samsung_tx_buf);

	cmdreq.cmds = &packet_size_cmd;
	cmdreq.cmds_cnt = 1;
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

	show_cnt = 0;
	for (read_loop = 0; read_loop < loop_limit; read_loop++) {
		reg_read_pos[1] = read_pos;

		mipi_dsi_buf_init(&msd.samsung_tx_buf);

		cmdreq.cmds = &read_pos_cmd;
		cmdreq.cmds_cnt = 1;
		cmdreq.flags = CMD_REQ_COMMIT;
		cmdreq.rlen = 0;
		cmdreq.cb = NULL;

		if (mipi_dsi_cmdlist_put(&cmdreq) < 1) {
			show_buffer_pos +=
			    snprintf(show_buffer + show_buffer_pos, 256,
				    "Tx command FAILED");
			break;
		}

		mipi_dsi_buf_init(&msd.samsung_tx_buf);
		mipi_dsi_buf_init(&msd.samsung_rx_buf);
		readed_size =
		    mipi_dsi_cmds_rx(pMFD, &msd.samsung_tx_buf,
				     &msd.samsung_rx_buf, &read_reg_cmd,
				     one_read_size);

		for (read_size = 0; read_size < readed_size;
						read_size++, show_cnt++) {
			show_buffer_pos +=
			 snprintf(show_buffer + show_buffer_pos, 256, "%02x ",
				    msd.samsung_rx_buf.data[read_size]);
			if (destBuffer != NULL && show_cnt < srcLength) {
				pr_debug("show_cnt(%d),srcLength(%d), msd.samsung_rx_buf.data[%d] = %x \n",
					show_cnt,srcLength,read_size,msd.samsung_rx_buf.data[read_size]);
				destBuffer[show_cnt] =
				    msd.samsung_rx_buf.data[read_size];
			}
		}

		show_buffer_pos += snprintf(show_buffer +
			show_buffer_pos, 256, ".");
		read_pos += readed_size;

		if ((read_pos - readFrom) >= srcLength)
			break;
	}

	mutex_unlock(&dsi_tx_mutex);

	if (read_loop == loop_limit)
		show_buffer_pos +=
		    snprintf(show_buffer + show_buffer_pos, 256, "Overrun");

	pr_info("%s\n", show_buffer);
}

static void execute_panel_init(struct msm_fb_data_type *mfd)
{
	struct SMART_DIM *psmart = &(msd.mpd->smart_s6e88a[msd.mpd->lcd_no]);
	char *mtp_buffer = (char *)&(msd.mpd->smart_s6e88a[msd.mpd->lcd_no].MTP_ORIGN);

	/* LSI HBM */
	char *mtp_buffer2 = (char *)&(msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b5_reg);
	char *mtp_buffer3 = (char *)&(msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b6_reg);
	int i;

	mipi_samsung_disp_send_cmd(mfd, PANEL_MTP_ENABLE, false);

	/* read LDi ID */
	msd.mpd->manufacture_id = mipi_samsung_manufacture_id(mfd);
	// msd.mpd->cmd_set_change(PANEL_ON, msd.mpd->manufacture_id);

	/* smart dimming & AID*/
	psmart->plux_table = msd.mpd->lux_table;
	psmart->lux_table_max = msd.mpd->lux_table_max_cnt;
	psmart->ldi_revision = msd.mpd->manufacture_id;

	read_mtp(MTP_START_ADDR, 0, GAMMA_SET_MAX, mtp_buffer, mfd);

	/* LSI HBM */
	read_mtp(0xb5, 13-1, 16, mtp_buffer2, mfd); // read b5h 13~28th
	read_mtp(0xb6, 3-1, 12, mtp_buffer3, mfd); // read b6h 3~14th
	msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b5_reg_19 = mtp_buffer2[6]; // save b5h 19th

	i = 0;
#ifdef CONFIG_HBM_PSRE_DEBUG
	printk("[HBM] b5_reg : ");
	for(i=0; i<16; i++)
		printk("%x ",msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b5_reg[i]);
	pr_info("\n");
	printk("[HBM] b6_reg : ");
	for(i=0; i<12; i++)
		printk("%x ",msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b6_reg[i]);
	pr_info("\n");
	printk("[HBM] b5_19th_reg : ");
	printk("%x ",msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b5_reg_19);
#endif

	smart_dimming_init(&(msd.mpd->smart_s6e88a[msd.mpd->lcd_no]));

	pr_info("%s - [%d]\n", __func__, msd.mpd->lcd_no);
}

int get_disp_switch(void);
static void mipi_samsung_disp_backlight(struct msm_fb_data_type *mfd);
static int mipi_samsung_disp_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;

	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	mipi = &mfd->panel_info.mipi;

	msd.mpd->lcd_no = get_disp_switch();
	if (!msd.dstat.is_smart_dim_loaded[msd.mpd->lcd_no]) {
		execute_panel_init(mfd);
		mfd->bl_level = DEFAULT_BL;
		msd.dstat.is_smart_dim_loaded[msd.mpd->lcd_no] = true;
	}

	if (get_auto_brightness() >= HBM_MODE_ID)
		msd.mpd->first_bl_hbm_psre = 1;

	mipi_samsung_disp_send_cmd(mfd, PANEL_ON, false);

	mfd->resume_state = MIPI_RESUME_STATE;

#if defined(CONFIG_MDNIE_LITE_TUNING)
	is_negative_on();
#endif

	msd.mpd->rst_brightness = true;
	mipi_samsung_disp_backlight(mfd);

	pr_info("%s - [%d], id=0x%x\n", __func__, msd.mpd->lcd_no, msd.mpd->manufacture_id);

	return 0;
}

static int mipi_samsung_disp_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	if(mfd->resume_state == MIPI_SUSPEND_STATE) {
		pr_info( "%s : already MIPI_SUSPEND_STATE. return\n", __func__ );
		return 0;
	}

	mfd->resume_state = MIPI_SUSPEND_STATE;	/* it need to be set before PANEL_OFF cmd. read mipi_samsung_disp_send_cmd() */
	mipi_samsung_disp_send_cmd(mfd, PANEL_OFF, false);

	if (msd.mpd->reset_bl_level != NULL)
		msd.mpd->reset_bl_level();

	pr_info("%s - [%d]\n", __func__, msd.mpd->lcd_no);

	return 0;
}

static void __devinit mipi_samsung_disp_shutdown(struct platform_device *pdev)
{
	static struct mipi_dsi_platform_data *mipi_dsi_pdata;

	if (pdev->id != 0)
		return;

	mipi_dsi_pdata = pdev->dev.platform_data;
	if (mipi_dsi_pdata == NULL) {
		pr_err("LCD Power off failure: No Platform Data\n");
		return;
	}

/* Power off Seq:2: Sleepout mode->ResetLow -> delay 120ms->VCI,VDD off */
	if (mipi_dsi_pdata && mipi_dsi_pdata->dsi_power_save) {
		mipi_dsi_pdata->lcd_rst_down();
		msleep(120);
		pr_info("LCD POWER OFF\n");
	}
}

static void mipi_samsung_disp_backlight(struct msm_fb_data_type *mfd)
{

	mfd->backlight_ctrl_ongoing = FALSE;
	if (mfd->resume_state == MIPI_RESUME_STATE) {
		if (msd.mpd->backlight_control(mfd->bl_level)) {
			mipi_samsung_disp_send_cmd(mfd, PANEL_BRIGHT_CTRL, true);
			pr_info("mipi_samsung_disp_backlight %d\n", mfd->bl_level);
		}
		msd.mpd->first_bl_hbm_psre = 0;
	} else {
		msd.mpd->first_bl_hbm_psre = 0;
		pr_info("%s : panel is off state!!\n", __func__);
	}
	return;
}

#if defined(CONFIG_HAS_EARLYSUSPEND)
static void mipi_samsung_disp_early_suspend(struct early_suspend *h)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd)) {
		pr_info("%s NO PDEV.\n", __func__);
		return;
	}
	if (unlikely(mfd->key != MFD_KEY)) {
		pr_info("%s MFD_KEY is not matched.\n", __func__);
		return;
	}

	pr_info("[lcd] %s\n", __func__);

}

static void mipi_samsung_disp_late_resume(struct early_suspend *h)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd)) {
		pr_info("%s NO PDEV.\n", __func__);
		return;
	}
	if (unlikely(mfd->key != MFD_KEY)) {
		pr_info("%s MFD_KEY is not matched.\n", __func__);
		return;
	}

	mfd->resume_state = MIPI_RESUME_STATE;

	pr_info("%s", __func__);
}
#endif

static int atoi(const char *name)
{
	int val = 0;

	for (;; name++) {
		switch (*name) {
		case '0' ... '9':
			val = 10*val+(*name-'0');
			break;
		default:
			return val;
		}
	}
}

unsigned char get_auto_brightness(void)
{
	return  msd.dstat.auto_brightness;
}
char* get_b5_reg(void)
{
	return msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b5_reg;
}
char get_b5_reg_19(void) /* for HBM ELVSS */
{
	return msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b5_reg_19;
}
char* get_b6_reg(void)
{
	return msd.mpd->smart_s6e88a[msd.mpd->lcd_no].hbm_reg.b6_reg;
}

#if defined(CONFIG_LCD_CLASS_DEVICE)
#ifdef WA_FOR_FACTORY_MODE
static ssize_t mipi_samsung_disp_get_power(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct msm_fb_data_type *mfd;
	int rc;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	rc = snprintf((char *)buf, sizeof(*buf), "%d\n", mfd->panel_power_on);
	pr_info("mipi_samsung_disp_get_power(%d)\n", mfd->panel_power_on);

	return rc;
}

static ssize_t mipi_samsung_disp_set_power(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;
	unsigned int power;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (sscanf(buf, "%u", &power) != 1)
		return -EINVAL;

	if (power == mfd->panel_power_on)
		return 0;

	if (power) {
		mfd->fbi->fbops->fb_blank(FB_BLANK_UNBLANK, mfd->fbi);
		mfd->fbi->fbops->fb_pan_display(&mfd->fbi->var, mfd->fbi);
		mipi_samsung_disp_send_cmd(mfd, PANEL_LATE_ON, true);
		mipi_samsung_disp_backlight(mfd);
	} else {
		mfd->fbi->fbops->fb_blank(FB_BLANK_POWERDOWN, mfd->fbi);
	}

	pr_info("%s : %d\n", __func__, power );

	return size;
}
#else
static int mipi_samsung_disp_get_power(struct lcd_device *dev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	pr_info("mipi_samsung_disp_get_power(%d)\n", mfd->panel_power_on);

	return mfd->panel_power_on;
}

static int mipi_samsung_disp_set_power(struct lcd_device *dev, int power)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (power == mfd->panel_power_on)
		return 0;

	if (power) {
		mfd->fbi->fbops->fb_blank(FB_BLANK_UNBLANK, mfd->fbi);
		mfd->fbi->fbops->fb_pan_display(&mfd->fbi->var, mfd->fbi);
		mipi_samsung_disp_send_cmd(mfd, PANEL_LATE_ON, true);
		mipi_samsung_disp_backlight(mfd);
	} else {
		mfd->fbi->fbops->fb_blank(FB_BLANK_POWERDOWN, mfd->fbi);
	}

	pr_info("mipi_samsung_disp_set_power : %d\n", power );
	return 0;
}
#endif

static ssize_t mipi_samsung_disp_lcdtype_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char temp[20];

	snprintf(temp, strnlen(msd.mpd->panel_name, 20) + 1,
						msd.mpd->panel_name);
	strncat(buf, temp, 20);
	return strnlen(buf, 20);
}

static ssize_t mipi_samsung_disp_gamma_mode_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(*buf), "%d\n", msd.dstat.gamma_mode);
	pr_info("%s: %d\n", __func__, *buf);

	return rc;
}

static ssize_t mipi_samsung_disp_gamma_mode_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	if (sysfs_streq(buf, "1") && !msd.dstat.gamma_mode) {
		/* 1.9 gamma */
		msd.dstat.gamma_mode = GAMMA_1_9;
		pr_info("%s : GAMMA_1_9\n", __func__ );
	} else if (sysfs_streq(buf, "0") && msd.dstat.gamma_mode) {
		/* 2.2 gamma */
		msd.dstat.gamma_mode = GAMMA_2_2;
		pr_info("%s : GAMMA_2_2\n", __func__ );
	} else {
		pr_info("%s: Invalid argument!!", __func__);
	}

	return size;
}

static ssize_t mipi_samsung_disp_acl_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(*buf), "%d\n", msd.dstat.acl_on);
	pr_info("%s: %d\n", __func__, *buf);

	return rc;
}

static ssize_t mipi_samsung_disp_acl_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (sysfs_streq(buf, "1"))
		msd.mpd->acl_status = true;
	else if (sysfs_streq(buf, "0"))
		msd.mpd->acl_status = false;
	else {
		pr_info("%s: Invalid argument!!", __func__);
		return size;
	}

	if (mfd->panel_power_on) {
		if (msd.mpd->acl_control(mfd->bl_level))
			mipi_samsung_disp_send_cmd(mfd,
						PANEL_ACL_CONTROL, true);
	} else
		pr_info("%s : panel is off state. updating state value.\n", __func__);

	pr_info("%s : acl_status (%d) siop_status (%d)",
			__func__, msd.mpd->acl_status, msd.mpd->siop_status);

	return size;
}

static ssize_t mipi_samsung_disp_siop_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", msd.mpd->siop_status);
	pr_info("siop status: %d\n", *buf);

	return rc;
}

static ssize_t mipi_samsung_disp_siop_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (sysfs_streq(buf, "1"))
		msd.mpd->siop_status = true;
	else if (sysfs_streq(buf, "0"))
		msd.mpd->siop_status = false;
	else {
		pr_info("%s: Invalid argument!!", __func__);
		return size;
	}

	if (mfd->panel_power_on) {
		if (msd.mpd->acl_control(mfd->bl_level))
			mipi_samsung_disp_send_cmd(mfd,
						PANEL_ACL_CONTROL, true);
	} else
		pr_info("%s : panel is off state. updating state value.\n", __func__);

	pr_info("%s : acl_status (%d) siop_status (%d)",
			__func__, msd.mpd->acl_status, msd.mpd->siop_status);

	return size;
}

static ssize_t mipi_samsung_auto_brightness_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n",
					msd.dstat.auto_brightness);
	pr_info("%s : %d\n", __func__, msd.dstat.auto_brightness);

	return rc;
}

static ssize_t mipi_samsung_auto_brightness_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	static int first_auto_br;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(msd.msm_pdev);

	if (sysfs_streq(buf, "0"))
		msd.dstat.auto_brightness = 0;
	else if (sysfs_streq(buf, "1"))
		msd.dstat.auto_brightness = 1;
	else if (sysfs_streq(buf, "2"))
		msd.dstat.auto_brightness = 2;
	else if (sysfs_streq(buf, "3"))
		msd.dstat.auto_brightness = 3;
	else if (sysfs_streq(buf, "4"))
		msd.dstat.auto_brightness = 4;
	else if (sysfs_streq(buf, "5"))
		msd.dstat.auto_brightness = 5;
	else if (sysfs_streq(buf, "6"))
		msd.dstat.auto_brightness = HBM_MODE_ID;
	else
		pr_info("%s: Invalid argument!!", __func__);

	if (!first_auto_br) {
		pr_info("%s : skip first auto brightness store (%d) (%d)!!\n",
				__func__, msd.dstat.auto_brightness, mfd->bl_level);
		first_auto_br++;
		return size;
	}

	if (mfd->resume_state == MIPI_RESUME_STATE) {
		msd.mpd->first_bl_hbm_psre = 1;
		mipi_samsung_disp_backlight(mfd);
		pr_info("%s : %d\n",__func__,msd.dstat.auto_brightness);
	} else {
		msd.mpd->first_bl_hbm_psre = 0;
		pr_info("%s : panel is off state!!\n", __func__);
	}

	return size;
}

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
void mipi_dsi_configure_dividers(int fps);
int mipi_AMS367AV_dynamic_fps_folder(int is_folder_action, struct msm_panel_info *pinfo)
{
	if (!is_folder_action || goal_fps == 0)
		goal_fps = pinfo->mipi.frame_rate;

	current_fps = goal_fps;
	mipi_dsi_configure_dividers(current_fps);

	pr_info("%s : fps=%d, folder=%d", __func__, current_fps, is_folder_action);
	return 0;
}

static ssize_t mipi_samsung_fps_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, 20, "%d\n", current_fps);

	return rc;
}

static ssize_t mipi_samsung_fps_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;
	int level = atoi(buf);

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (mfd->panel_power_on == FALSE) {
		pr_err("%s fps set error, panel power off 1", __func__);
		return size;
	}

	/* tuning history.
	* goal_fps cannot be lower than 43, because mdp UNDERRUN - 2013.4.30
	*/
	switch( level ) {
	case 0 : goal_fps = 60;	break;
	case 1 : goal_fps = 43;	break;
	case 2 : goal_fps = 51;	break;
	default:
		pr_info("%s fps set error : invalid level %d", __func__, level);
		return size;
	}

	if (current_fps != goal_fps)
		current_fps = goal_fps;
	else
		return size;

	mutex_lock(&dsi_tx_mutex);

	if (mfd->panel_power_on == FALSE) {
		mutex_unlock(&dsi_tx_mutex);
		pr_info("%s fps set error, panel power off 2", __func__);
		return size;
	} else {
		mipi_runtime_clk_change(current_fps);
		mutex_unlock(&dsi_tx_mutex);
	}

	pr_info("%s goal_fps : %d", __func__, goal_fps);

	return size;
}

#endif

static ssize_t mipi_samsung_disp_backlight_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int rc;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(msd.msm_pdev);

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", mfd->bl_level);
	pr_info("%s : %d\n", __func__, mfd->bl_level);

	return rc;
}

static ssize_t mipi_samsung_disp_backlight_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;
	int level = atoi(buf);

	mfd = platform_get_drvdata(msd.msm_pdev);

	mfd->bl_level = level;

	if (mfd->resume_state == MIPI_RESUME_STATE) {
		mipi_samsung_disp_backlight(mfd);
		pr_info("%s : level (%d)\n",__func__,level);
	} else {
		pr_info("%s : panel is off state!!\n", __func__);
	}

	return size;
}

static struct lcd_ops mipi_samsung_disp_props = {
#ifdef WA_FOR_FACTORY_MODE
	.get_power = NULL,
	.set_power = NULL,
#else
	.get_power = mipi_samsung_disp_get_power,
	.set_power = mipi_samsung_disp_set_power,
#endif
};

#ifdef WA_FOR_FACTORY_MODE
static DEVICE_ATTR(lcd_power, S_IRUGO | S_IWUSR,
			mipi_samsung_disp_get_power,
			mipi_samsung_disp_set_power);
#endif
static DEVICE_ATTR(lcd_type, S_IRUGO, mipi_samsung_disp_lcdtype_show, NULL);
static DEVICE_ATTR(gamma_mode, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_disp_gamma_mode_show,
			mipi_samsung_disp_gamma_mode_store);
static DEVICE_ATTR(power_reduce, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_disp_acl_show,
			mipi_samsung_disp_acl_store);
static DEVICE_ATTR(auto_brightness, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_auto_brightness_show,
			mipi_samsung_auto_brightness_store);
static DEVICE_ATTR(siop_enable, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_disp_siop_show,
			mipi_samsung_disp_siop_store);
static DEVICE_ATTR(backlight, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_disp_backlight_show,
			mipi_samsung_disp_backlight_store);
#endif

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
static DEVICE_ATTR(fps_change, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_fps_show,
			mipi_samsung_fps_store);
#endif


#ifdef DDI_VIDEO_ENHANCE_TUNING
#define MAX_FILE_NAME 128
#define TUNING_FILE_PATH "/sdcard/"
#define TUNE_FIRST_SIZE 5
#define TUNE_SECOND_SIZE 108
static char tuning_file[MAX_FILE_NAME];

static char mdni_tuning1[TUNE_FIRST_SIZE];
static char mdni_tuning2[TUNE_SECOND_SIZE];

static struct dsi_cmd_desc mdni_tune_cmd[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(mdni_tuning2), mdni_tuning2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(mdni_tuning1), mdni_tuning1},
};

static char char_to_dec(char data1, char data2)
{
	char dec;

	dec = 0;

	if (data1 >= 'a') {
		data1 -= 'a';
		data1 += 10;
	} else if (data1 >= 'A') {
		data1 -= 'A';
		data1 += 10;
	} else
		data1 -= '0';

	dec = data1 << 4;

	if (data2 >= 'a') {
		data2 -= 'a';
		data2 += 10;
	} else if (data2 >= 'A') {
		data2 -= 'A';
		data2 += 10;
	} else
		data2 -= '0';

	dec |= data2;

	return dec;
}

static void sending_tune_cmd(char *src, int len)
{
	struct msm_fb_data_type *mfd;
	struct dcs_cmd_req cmdreq;

	int data_pos;
	int cmd_step;
	int cmd_pos;

	cmd_step = 0;
	cmd_pos = 0;

	for (data_pos = 0; data_pos < len;) {
		if (*(src + data_pos) == '0') {
			if (*(src + data_pos + 1) == 'x') {
				if (!cmd_step) {
					mdni_tuning1[cmd_pos] =
					char_to_dec(*(src + data_pos + 2),
							*(src + data_pos + 3));
				} else {
					mdni_tuning2[cmd_pos] =
					char_to_dec(*(src + data_pos + 2),
							*(src + data_pos + 3));
				}

				data_pos += 3;
				cmd_pos++;

				if (cmd_pos == TUNE_FIRST_SIZE && !cmd_step) {
					cmd_pos = 0;
					cmd_step = 1;
				}
			} else
				data_pos++;
		} else {
			data_pos++;
		}
	}

	printk(KERN_INFO "\n");
	for (data_pos = 0; data_pos < TUNE_FIRST_SIZE ; data_pos++)
		printk(KERN_INFO "0x%x ", mdni_tuning1[data_pos]);
	printk(KERN_INFO "\n");
	for (data_pos = 0; data_pos < TUNE_SECOND_SIZE ; data_pos++)
		printk(KERN_INFO"0x%x ", mdni_tuning2[data_pos]);
	printk(KERN_INFO "\n");

	mfd = platform_get_drvdata(msd.msm_pdev);

	mutex_lock(&dsi_tx_mutex);

	cmdreq.cmds = mdni_tune_cmd;
	cmdreq.cmds_cnt = ARRAY_SIZE(mdni_tune_cmd);
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

	mutex_unlock(&dsi_tx_mutex);
}

static void load_tuning_file(char *filename)
{
	struct file *filp;
	char *dp;
	long l;
	loff_t pos;
	int ret;
	mm_segment_t fs;

	pr_info("%s called loading file name : [%s]\n", __func__,
	       filename);

	fs = get_fs();
	set_fs(get_ds());

	filp = filp_open(filename, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		printk(KERN_ERR "%s File open failed\n", __func__);
		return;
	}

	l = filp->f_path.dentry->d_inode->i_size;
	pr_info("%s Loading File Size : %ld(bytes)", __func__, l);

	dp = kmalloc(l + 10, GFP_KERNEL);
	if (dp == NULL) {
		pr_info("Can't not alloc memory for tuning file load\n");
		filp_close(filp, current->files);
		return;
	}
	pos = 0;
	memset(dp, 0, l);

	pr_info("%s before vfs_read()\n", __func__);
	ret = vfs_read(filp, (char __user *)dp, l, &pos);
	pr_info("%s after vfs_read()\n", __func__);

	if (ret != l) {
		pr_info("vfs_read() filed ret : %d\n", ret);
		kfree(dp);
		filp_close(filp, current->files);
		return;
	}

	filp_close(filp, current->files);

	set_fs(fs);

	sending_tune_cmd(dp, l);

	kfree(dp);
}


static ssize_t tuning_show(struct device *dev,
			   struct device_attribute *attr, char *buf)
{
	int ret = 0;

	ret = snprintf(buf, MAX_FILE_NAME, "Tunned File Name : %s\n",
								tuning_file);

	return ret;
}

static ssize_t tuning_store(struct device *dev,
			    struct device_attribute *attr, const char *buf,
			    size_t size)
{
	char *pt;
	memset(tuning_file, 0, sizeof(tuning_file));
	snprintf(tuning_file, MAX_FILE_NAME, "%s%s", TUNING_FILE_PATH, buf);

	pt = tuning_file;
	while (*pt) {
		if (*pt == '\r' || *pt == '\n') {
			*pt = 0;
			break;
		}
		pt++;
	}

	pr_info("%s:%s\n", __func__, tuning_file);

	load_tuning_file(tuning_file);

	return size;
}

static DEVICE_ATTR(tuning, 0664, tuning_show, tuning_store);
#endif


static int __devinit mipi_samsung_disp_probe(struct platform_device *pdev)
{
	struct platform_device *msm_fb_added_dev;
#if defined(CONFIG_LCD_CLASS_DEVICE)
	struct lcd_device *lcd_device;
#endif
#if defined(CONFIG_BACKLIGHT_CLASS_DEVICE)
	struct backlight_device *bd = NULL;
#endif
	int ret = 0;
	int i;

	msd.dstat.acl_on = false;

	if (pdev->id == 0) {
		msd.mipi_samsung_disp_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_added_dev = msm_fb_add_device(pdev);

	mutex_init(&dsi_tx_mutex);

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	msd.msm_pdev = msm_fb_added_dev;
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
	msd.early_suspend.suspend = mipi_samsung_disp_early_suspend;
	msd.early_suspend.resume = mipi_samsung_disp_late_resume;
	msd.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&msd.early_suspend);
#endif

	msd.mpd->lcd_no = get_disp_switch();
	for (i = 0; i < LCD_LOTS; i++) {
		msd.dstat.is_elvss_loaded[i] = false;
		msd.dstat.is_smart_dim_loaded[i] = false;
	}

#if defined(CONFIG_LCD_CLASS_DEVICE)
	lcd_device = lcd_device_register("panel", &pdev->dev, NULL,
					&mipi_samsung_disp_props);

	if (IS_ERR(lcd_device)) {
		ret = PTR_ERR(lcd_device);
		printk(KERN_ERR "lcd : failed to register device\n");
		return ret;
	}

#ifdef WA_FOR_FACTORY_MODE
	sysfs_remove_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_power.attr);

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_power.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_lcd_power.attr.name);
	}
#endif

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_type.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_lcd_type.attr.name);
	}

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_gamma_mode.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_gamma_mode.attr.name);
	}

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_power_reduce.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_power_reduce.attr.name);
	}

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_siop_enable.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_siop_enable.attr.name);
	}

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_backlight.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_backlight.attr.name);
	}

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
	ret = sysfs_create_file(&lcd_device->dev.kobj,
						&dev_attr_fps_change.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_fps_change.attr.name);
	}
#endif

#if defined(CONFIG_BACKLIGHT_CLASS_DEVICE)
	bd = backlight_device_register("panel", &lcd_device->dev,
						NULL, NULL, NULL);
	if (IS_ERR(bd)) {
		ret = PTR_ERR(bd);
		pr_info("backlight : failed to register device\n");
		return ret;
	}

	ret = sysfs_create_file(&bd->dev.kobj,
					&dev_attr_auto_brightness.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_auto_brightness.attr.name);
	}
#endif
#endif
#if defined(CONFIG_MDNIE_LITE_TUNING) \
	|| defined(CONFIG_FB_MDP4_ENHANCE)
	init_mdnie_class();
#endif

#if defined(DDI_VIDEO_ENHANCE_TUNING)
		ret = sysfs_create_file(&lcd_device->dev.kobj,
				&dev_attr_tuning.attr);
		if (ret) {
			pr_info("sysfs create fail-%s\n",
					dev_attr_tuning.attr.name);
		}
#endif
	return 0;
}

static struct platform_driver this_driver = {
	.probe = mipi_samsung_disp_probe,
	.driver = {
		   .name = "mipi_ams367av_oled",
		   },
	.shutdown = mipi_samsung_disp_shutdown
};

static struct msm_fb_panel_data samsung_panel_data = {
	.on = mipi_samsung_disp_on,
	.off = mipi_samsung_disp_off,
	.set_backlight = mipi_samsung_disp_backlight,
};

static int ch_used[3];

int mipi_ams367av_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd)
{
	struct platform_device *pdev = NULL;
	int ret = 0;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_ams367av_oled",
					   (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	samsung_panel_data.panel_info = *pinfo;
	msd.mpd = mpd;
	if (!msd.mpd) {
		printk(KERN_ERR
		  "%s: get mipi_panel_data failed!\n", __func__);
		goto err_device_put;
	}
	mpd->msd = &msd;
	ret = platform_device_add_data(pdev, &samsung_panel_data,
		sizeof(samsung_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return ret;

err_device_put:
	platform_device_put(pdev);
	return ret;
}


static int __init current_boot_mode(char *mode)
{
	/*
	*	1 is recovery booting
	*	0 is normal booting
	*/

	if (strncmp(mode, "1", 1) == 0)
		recovery_boot_mode = 1;
	else
		recovery_boot_mode = 0;

	pr_debug("%s %s", __func__, recovery_boot_mode == 1 ?
						"recovery" : "normal");
	return 1;
}
__setup("androidboot.boot_recovery=", current_boot_mode);

static int __init mipi_samsung_disp_init(void)
{
	mipi_dsi_buf_alloc(&msd.samsung_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&msd.samsung_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}
module_init(mipi_samsung_disp_init);
