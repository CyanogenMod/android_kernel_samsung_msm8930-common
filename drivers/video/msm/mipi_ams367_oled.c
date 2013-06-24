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
#include "mipi_ams367_oled.h"
#include "mdp4.h"

static struct mipi_samsung_driver_data msd;
static unsigned int recovery_boot_mode;
boolean is_acl_on;
#define WA_FOR_FACTORY_MODE
#define READ_MTP_ONCE

struct dcs_cmd_req cmdreq;

int is_ams367_connected = 1;

#ifndef DEBUG_MIPI
#define DEBUG_MIPI 1
#define DEBUG_THIS()	pr_err("(mipi) %s : %d called\n", __func__, __LINE__)
#define DEBUG_IN()	pr_err("(mipi) + %s : %d called\n", __func__, __LINE__)
#define DEBUG_OUT()	pr_err("(mipi) - %s : %d called\n", __func__, __LINE__)
#define DEBUG_STR(X, ...)	pr_err("(mipi) %s : "X"\n", __func__, ## __VA_ARGS__)

static char debug_str[256];
static int debug_str_pos;
#define DEBUG_STR_CLEAR() do { debug_str[0] = 0; debug_str_pos = 0; } while (0)
#define DEBUG_STR_ADD(a)	strcpy(debug_str + strlen(debug_str), a)
#define DEBUG_STR_ADD_PRINTF(a, ...)	snprintf(debug_str + strlen(debug_str), sizeof(debug_str) - strlen(debug_str) - 1, a, ## __VA_ARGS__)
#define DEBUG_STR_SHOW()	pr_err("(mipi) %s : %s\n", __func__, debug_str)

#ifndef TRUE
#define TRUE (1 == 1)
#define FALSE (!TRUE)
#endif
#endif /* #ifndef DEBUG_MIPI */

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
static int current_fps;
static int goal_fps = 0;
#endif

static uint32 mipi_samsung_manufacture_id(struct msm_fb_data_type *mfd)
{
	static char manufacture_id1[2] = { 0xDA, 0x00 };	/* DTYPE_DCS_READ */
	static char manufacture_id2[2] = { 0xDB, 0x00 };	/* DTYPE_DCS_READ */
	static char manufacture_id3[2] = { 0xDC, 0x00 };	/* DTYPE_DCS_READ */
	static struct dsi_cmd_desc samsung_manufacture_id_cmd[] = {
		{DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id1), manufacture_id1}
		,
		{DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id2), manufacture_id2}
		,
		{DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id3), manufacture_id3}
	};
	struct dsi_buf *rp, *tp;
	struct dsi_cmd_desc *cmd;

	char ids[ARRAY_SIZE(samsung_manufacture_id_cmd)];
	uint32 id;

	int i;

	tp = &msd.samsung_tx_buf;
	rp = &msd.samsung_rx_buf;
	id = 0;
	for (i = 0; i < ARRAY_SIZE(samsung_manufacture_id_cmd); i++) {
		mipi_dsi_buf_init(rp);
		mipi_dsi_buf_init(tp);
		cmd = &(samsung_manufacture_id_cmd[i]);
		mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);
		ids[i] = rp->data[0];
		id |= ids[i];
		id <<= 8;
	}
	pr_info("%s: manufacture_id=%x\n", __func__, id);

#ifdef FACTORY_TEST
	if (id == 0x00) {
		pr_info("Lcd is not connected\n");
		is_ams367_connected = 0;
	}
#endif
	return id;
}

static void read_reg(char src_reg, int src_length, char *dest_buffer, const int is_use_mutex, struct msm_fb_data_type *p_mfd)
{
	const int one_read_size = 4;
	const int loop_limit = 16;
	/* first byte = read-register */
	static char read_reg[2] = { 0xFF, 0x00 };
	static struct dsi_cmd_desc s6e8aa0_read_reg_cmd = {
		DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(read_reg), read_reg
	};
	/* first byte is size of Register */
	static char packet_size[] = { 0x04, 0 };
	static struct dsi_cmd_desc s6e8aa0_packet_size_cmd = {
		DTYPE_MAX_PKTSIZE, 1, 0, 0, 0, sizeof(packet_size), packet_size
	};

	/* second byte is Read-position */
	static char reg_read_pos[] = { 0xB0, 0x00 };
	static struct dsi_cmd_desc s6e8aa0_read_pos_cmd = {
		DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(reg_read_pos),
		reg_read_pos
	};

	int read_pos;
	int readed_size;
	int show_cnt;

	int i, j;
	char show_buffer[256];
	int show_buffer_pos = 0;

	read_reg[0] = src_reg;

	show_buffer_pos += snprintf(show_buffer, sizeof(show_buffer), "(mipi)read_reg : %X[%d] : ", src_reg, src_length);

	read_pos = 0;
	readed_size = 0;

	packet_size[0] = (char)src_length;
	mipi_dsi_buf_init(&msd.samsung_tx_buf);
	mipi_dsi_cmds_tx(&msd.samsung_tx_buf, &(s6e8aa0_packet_size_cmd), 1);

	show_cnt = 0;
	for (j = 0; j < loop_limit; j++) {
		reg_read_pos[1] = read_pos;
		if (mipi_dsi_cmds_tx(&msd.samsung_tx_buf, &(s6e8aa0_read_pos_cmd), 1) < 1) {
			show_buffer_pos += snprintf(show_buffer + show_buffer_pos, sizeof(show_buffer), "Tx command FAILED");
			break;
		}
		mipi_dsi_buf_init(&msd.samsung_tx_buf);
		mipi_dsi_buf_init(&msd.samsung_rx_buf);
		readed_size = mipi_dsi_cmds_rx(p_mfd, &msd.samsung_tx_buf, &msd.samsung_rx_buf, &s6e8aa0_read_reg_cmd, one_read_size);
		for (i = 0; i < readed_size; i++, show_cnt++) {
			show_buffer_pos += snprintf(show_buffer + show_buffer_pos, sizeof(show_buffer), "%02x ", msd.samsung_rx_buf.data[i]);
			if (dest_buffer != NULL && show_cnt < src_length)
				dest_buffer[show_cnt] = msd.samsung_rx_buf.data[i];
		}
		show_buffer_pos += snprintf(show_buffer + show_buffer_pos, sizeof(show_buffer), ".");
		read_pos += readed_size;
		if (read_pos > src_length)
			break;
	}

	if (j == loop_limit)
		show_buffer_pos += snprintf(show_buffer + show_buffer_pos, sizeof(show_buffer), "Overrun");

	pr_info("%s\n", show_buffer);
}

static int mipi_ams367_send_cmd(struct msm_fb_data_type *mfd, enum mipi_samsung_cmd_list cmd, unsigned char lock)
{
	struct dsi_cmd_desc *cmd_desc;
	int cmd_size = 0;
	int i;

	/*      wake_lock(&idle_wake_lock); */
	/*temp */

	if (lock)
		mutex_lock(&mfd->dma->ov_mutex);

	switch (cmd) {
	case PANEL_READY_TO_ON:
		DEBUG_STR("PANEL_READY_TO_ON:");
		cmd_desc = msd.mpd->ready_to_on.cmd;
		cmd_size = msd.mpd->ready_to_on.size;
		break;
	case PANEL_READY_TO_OFF:
		DEBUG_STR("PANEL_READY_TO_OFF:");
		cmd_desc = msd.mpd->ready_to_off.cmd;
		cmd_size = msd.mpd->ready_to_off.size;
		break;
	case PANEL_ON:
		DEBUG_STR("PANEL_ON:");
		cmd_desc = msd.mpd->on.cmd;
		cmd_size = msd.mpd->on.size;
		break;
	case PANEL_OFF:
		DEBUG_STR("PANEL_OFF:");
		cmd_desc = msd.mpd->off.cmd;
		cmd_size = msd.mpd->off.size;
		break;
	case PANEL_LATE_ON:
		DEBUG_STR("PANEL_LATE_ON:");
		cmd_desc = msd.mpd->late_on.cmd;
		cmd_size = msd.mpd->late_on.size;
		break;
	case PANEL_EARLY_OFF:
#if 0				/* this code called after LCD-Power-off. */
		DEBUG_STR("PANEL_EARLY_OFF:");
		cmd_desc = msd.mpd->early_off.cmd;
		cmd_size = msd.mpd->early_off.size;
#else
		DEBUG_STR("PANEL_EARLY_OFF: ignored");
#endif
		break;
	case PANEL_GAMMA_UPDATE:
		DEBUG_STR("PANEL_GAMMA_UPDATE:");
		cmd_desc = msd.mpd->gamma_update.cmd;
		cmd_size = msd.mpd->gamma_update.size;
		break;
	case MTP_READ_ENABLE:
		DEBUG_STR("MTP_READ_ENABLE:");
		cmd_desc = msd.mpd->mtp_read_enable.cmd;
		cmd_size = msd.mpd->mtp_read_enable.size;
		break;
#ifdef USE_ACL
	case PANEL_ACL_ON:
		DEBUG_STR("PANEL_ACL_ON:");
		cmd_desc = msd.mpd->acl_on.cmd;
		cmd_size = msd.mpd->acl_on.size;
		msd.mpd->ldi_acl_stat = true;
		break;
	case PANEL_ACL_OFF:
		DEBUG_STR("PANEL_ACL_OFF:");
		cmd_desc = msd.mpd->acl_off.cmd;
		cmd_size = msd.mpd->acl_off.size;
		msd.mpd->ldi_acl_stat = false;
		break;
	case PANEL_ACL_UPDATE:
		DEBUG_STR("PANEL_ACL_UPDATE:");
		cmd_desc = msd.mpd->acl_update.cmd;
		cmd_size = msd.mpd->acl_update.size;
		break;
#endif
#ifdef USE_ELVSS
	case PANEL_ELVSS_UPDATE:
		DEBUG_STR("PANEL_ELVSS_UPDATE:");
		msd.mpd->set_elvss(mfd->bl_level);
		cmd_desc = msd.mpd->elvss_update.cmd;
		cmd_size = msd.mpd->elvss_update.size;
		break;
#endif
#if defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT_PANEL)
	case PANEL_BRIGHT_CTRL:
		DEBUG_STR("PANEL_BRIGHT_CTRL:");
		cmd_desc = msd.mpd->combined_ctrl.cmd;
		cmd_size = msd.mpd->combined_ctrl.size;
		break;
#endif
	default:
		goto unknown_command;
		;
	}

	if (!cmd_size)
		goto unknown_command;

	mipi_dsi_mdp_busy_wait();
	/* Added to resolved cmd loss during dimming factory test */
	mdelay(1);

	for (i = 0; i < cmd_size; i++) {
		switch (cmd_desc[i].dlen) {
		case 0:
			DEBUG_STR("length=0?? [%d]", i);
			break;
		case 1:
/*                              if( cmd_desc[i].dtype != DTYPE_DCS_WRITE ) DEBUG_STR( "check1 %dth", i ); */
			cmd_desc[i].dtype = DTYPE_DCS_WRITE;
			break;
		case 2:
/*                              if( cmd_desc[i].dtype != DTYPE_DCS_WRITE1 ) DEBUG_STR( "check2 %dth", i ); */
			cmd_desc[i].dtype = DTYPE_DCS_WRITE1;
			break;
		default:
/*                              if( cmd_desc[i].dtype != DTYPE_DCS_LWRITE ) DEBUG_STR( "checkL %dth", i ); */
			cmd_desc[i].dtype = DTYPE_DCS_LWRITE;
			break;
		}

	}

	if (mfd->panel_info.type == MIPI_CMD_PANEL) {
		cmdreq.cmds = cmd_desc;
		cmdreq.cmds_cnt = cmd_size;
		cmdreq.flags = CMD_REQ_COMMIT;
		cmdreq.rlen = 0;
		cmdreq.cb = NULL;
		mipi_dsi_cmdlist_put(&cmdreq);
	} else
		mipi_dsi_cmds_tx(&msd.samsung_tx_buf, cmd_desc, cmd_size);

	if (lock)
		mutex_unlock(&mfd->dma->ov_mutex);

	/*      wake_unlock(&idle_wake_lock); */
	/*temp */

	return 0;

unknown_command:
	if (lock)
		mutex_unlock(&mfd->dma->ov_mutex);

	/*      wake_unlock(&idle_wake_lock); */
	/*temp */

	return 0;
}

static unsigned char first_on;

static int find_mtp(struct msm_fb_data_type *mfd, char *mtp)
{
	char first_mtp[MTP_DATA_SIZE_EA8868];
	char second_mtp[MTP_DATA_SIZE_EA8868];
	int mtp_size = MTP_DATA_SIZE_EA8868;
	int correct_mtp;

	pr_info("mpt read\n");
	read_reg(MTP_REGISTER, mtp_size, first_mtp, FALSE, mfd);
	read_reg(MTP_REGISTER, mtp_size, second_mtp, FALSE, mfd);

	if (memcmp(first_mtp, second_mtp, mtp_size) != 0) {
		char third_mtp[MTP_DATA_SIZE_EA8868];
		pr_info("mpt read, one more\n");
		read_reg(MTP_REGISTER, mtp_size, third_mtp, FALSE, mfd);

		if (memcmp(first_mtp, third_mtp, mtp_size) == 0) {
			pr_info("MTP data is used from first read mtp");
			memcpy(mtp, first_mtp, mtp_size);
			correct_mtp = 1;
		} else if (memcmp(second_mtp, third_mtp, mtp_size) == 0) {
			pr_info("MTP data is used from second read mtp");
			memcpy(mtp, second_mtp, mtp_size);
			correct_mtp = 2;
		} else {
			pr_info("MTP data is used 0 read mtp");
			memset(mtp, 0, mtp_size);
			correct_mtp = 0;
		}
	} else {
		pr_info("MTP data is used from first read mtp");
		memcpy(mtp, first_mtp, mtp_size);
		correct_mtp = 1;
	}

	return correct_mtp;
}

static void mipi_samsung_disp_backlight_lock(struct msm_fb_data_type *mfd, int lock);
static int mipi_samsung_disp_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	static int boot_on;

	int is_registed_lcd;
	int access_lcd_no;

	int mtp_cnt;
	unsigned int temp_manufacture_id;
	unsigned char temp_mtp[MTP_DATA_SIZE + 16];

	int i;

	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	mipi = &mfd->panel_info.mipi;

		mipi_ams367_send_cmd(mfd, MTP_READ_ENABLE, false);

		temp_manufacture_id = mipi_samsung_manufacture_id(mfd);

		mtp_cnt = find_mtp(mfd, temp_mtp);
		DEBUG_STR_CLEAR();
		DEBUG_STR_ADD_PRINTF(" MTP :");
		for (i = 0; i < MTP_DATA_SIZE_EA8868; i++) {
			DEBUG_STR_ADD_PRINTF("%02x ", temp_mtp[i]);
			if (i > 0 && i % 4 == 0)
				DEBUG_STR_ADD(".");
		}
		if (mtp_cnt >= MTP_RETRY_MAX)
			DEBUG_STR_ADD(": FAILED");
		DEBUG_STR_SHOW();

	is_registed_lcd = false;
	access_lcd_no = 0;
	msd.dstat.gamma_mode = GAMMA_2_2;
	for (i = 0; i < LCD_LOTS; i++) {
		if (!msd.dstat.is_smart_dim_loaded[i])
			continue;

		access_lcd_no = i;
		if (temp_manufacture_id == msd.mpd->manufacture_id[i] && !memcmp(temp_mtp, &(msd.mpd->smart_ea8868[i].MTP), MTP_DATA_SIZE_EA8868)) {
			is_registed_lcd = true;
			msd.dstat.gamma_mode = GAMMA_SMART;
			break;
		}
	}

	if (is_registed_lcd) {
		DEBUG_STR("alredy registed LCD-%x", access_lcd_no);
	} else {		/* not registed */
		for (i = 0; i < LCD_LOTS; i++)
			if (!msd.dstat.is_smart_dim_loaded[i])
				break;

		if (i >= LCD_LOTS) {
			DEBUG_STR("not-registed LCD was not found.");
		} else {
			msd.mpd->manufacture_id[i] = temp_manufacture_id;
			memcpy(&(msd.mpd->smart_ea8868[i].MTP), temp_mtp, MTP_DATA_SIZE_EA8868);
			smart_dimming_init(&(msd.mpd->smart_ea8868[i]));
			msd.dstat.is_smart_dim_loaded[i] = true;
			msd.dstat.gamma_mode = GAMMA_SMART;
			access_lcd_no = i;
			DEBUG_STR("Registered LCD-%d\n", access_lcd_no);

			if (boot_on == 0) {
				if (recovery_boot_mode == 0)
					boot_on = 1;
				else {
/*
					msd.mpd->smart_ea8868[i].brightness_level = get_gamma_lux();
					reset_gamma_level();
*/
				}

			}
		}

	}
	msd.mpd->lcd_no = access_lcd_no;

	if (unlikely(first_on)) {
		first_on = false;
		return 0;
	}

	mipi_ams367_send_cmd(mfd, PANEL_READY_TO_ON, false);
	if (mipi->mode == DSI_VIDEO_MODE)
		mipi_ams367_send_cmd(mfd, PANEL_ON, false);
#if !defined(CONFIG_HAS_EARLYSUSPEND)
	mipi_ams367_send_cmd(mfd, PANEL_LATE_ON, false);
#endif

	mfd->resume_state = MIPI_RESUME_STATE;

	mipi_samsung_disp_backlight_lock( mfd, false );

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

	mfd->resume_state = MIPI_SUSPEND_STATE;

	mipi_ams367_send_cmd(mfd, PANEL_READY_TO_OFF, false);
	mipi_ams367_send_cmd(mfd, PANEL_OFF, false);

#ifdef USE_ACL
	msd.mpd->ldi_acl_stat = false;
#endif
	return 0;
}

static void mipi_samsung_disp_shutdown(struct platform_device *pdev)
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

static void mipi_samsung_disp_backlight_lock(struct msm_fb_data_type *mfd, int lock)
{
	mfd->backlight_ctrl_ongoing = FALSE;

	pr_info("mipi_samsung_disp_backlight %d\n", mfd->bl_level);

	/* mpd->setgamma = set_gamma_level */
	if (!msd.mpd->set_gamma || !mfd->panel_power_on || mfd->resume_state == MIPI_SUSPEND_STATE)
		goto end;
	if (msd.mpd->set_gamma(mfd->bl_level, msd.dstat.gamma_mode) < 0)
		goto end;

	pr_info("mipi_samsung_disp_backlight %d\n", mfd->bl_level);
	mipi_ams367_send_cmd(mfd, PANEL_GAMMA_UPDATE, lock);

#ifdef USE_ELVSS
	if (msd.mpd->set_elvss)
		mipi_ams367_send_cmd(mfd, PANEL_ELVSS_UPDATE, lock);
#endif

#ifdef USE_ACL
	if (msd.mpd->set_acl && msd.dstat.acl_on && msd.mpd->set_acl(mfd->bl_level))
		mipi_ams367_send_cmd(mfd, PANEL_ACL_OFF, lock);
	if (msd.mpd->set_acl && msd.dstat.acl_on && !msd.mpd->set_acl(mfd->bl_level)) {
		mipi_ams367_send_cmd(mfd, PANEL_ACL_ON, lock);
		mipi_ams367_send_cmd(mfd, PANEL_ACL_UPDATE, lock);
	}
#endif

end:
	return;
}

static void mipi_samsung_disp_backlight(struct msm_fb_data_type *mfd)
{
	mipi_samsung_disp_backlight_lock(mfd, true);
}

#if defined(CONFIG_HAS_EARLYSUSPEND)
static void mipi_samsung_disp_early_suspend(struct early_suspend *h)
{
	struct msm_fb_data_type *mfd;
	pr_info("%s", __func__);

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd)) {
		pr_info("%s NO PDEV.\n", __func__);
		return;
	}
	if (unlikely(mfd->key != MFD_KEY)) {
		pr_info("%s MFD_KEY is not matched.\n", __func__);
		return;
	}

	mipi_ams367_send_cmd(mfd, PANEL_EARLY_OFF, true);
	mfd->resume_state = MIPI_SUSPEND_STATE;
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
#if  defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT)
	reset_gamma_level();
	mfd->resume_state = MIPI_RESUME_STATE;
	mipi_samsung_disp_backlight(mfd);
#endif
	pr_info("%s", __func__);
}
#endif

#if defined(CONFIG_LCD_CLASS_DEVICE)

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

#ifdef WA_FOR_FACTORY_MODE
static ssize_t mipi_samsung_disp_get_power(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct msm_fb_data_type *mfd;
	int rc;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", mfd->panel_power_on);
	pr_info("mipi_samsung_disp_get_power(%d)\n", mfd->panel_power_on);

	return rc;
}

static ssize_t mipi_samsung_disp_set_power(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
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
		mipi_ams367_send_cmd(mfd, PANEL_LATE_ON, true);
		mipi_samsung_disp_backlight(mfd);
	} else {
		mfd->fbi->fbops->fb_blank(FB_BLANK_POWERDOWN, mfd->fbi);
	}

	pr_info("mipi_samsung_disp_set_power\n");

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
		mipi_ams367_send_cmd(mfd, PANEL_LATE_ON, true);
		mipi_samsung_disp_backlight(mfd);
	} else {
		mfd->fbi->fbops->fb_blank(FB_BLANK_POWERDOWN, mfd->fbi);
	}

	pr_info("mipi_samsung_disp_set_power\n");
	return 0;
}
#endif

static ssize_t mipi_samsung_disp_lcdtype_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	char temp[20];

	snprintf(temp, strnlen(msd.mpd->panel_name, 20) + 1, msd.mpd->panel_name);
	strlcat(buf, temp, 20);
	return strnlen(buf, 20);
}

static ssize_t mipi_samsung_disp_gamma_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", msd.dstat.gamma_mode);
	pr_info("gamma_mode: %d\n", *buf);

	return rc;
}

static ssize_t mipi_samsung_disp_gamma_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	if (sysfs_streq(buf, "1") && !msd.dstat.gamma_mode) {
		/* 1.9 gamma */
		msd.dstat.gamma_mode = GAMMA_1_9;
	} else if (sysfs_streq(buf, "0") && msd.dstat.gamma_mode) {
		/* 2.2 gamma */
		msd.dstat.gamma_mode = GAMMA_2_2;
	} else {
		pr_info("%s: Invalid argument!!", __func__);
	}

	return size;
}

static ssize_t mipi_samsung_disp_acl_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", msd.dstat.acl_on);
	pr_info("acl status: %d\n", *buf);

	return rc;
}

static ssize_t mipi_samsung_disp_acl_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (!mfd->panel_power_on) {
		pr_info("%s: panel is off state. updating state value.\n", __func__);
		if (sysfs_streq(buf, "1") && !msd.dstat.acl_on) {
			msd.dstat.acl_on = true;
			is_acl_on = true;
		} else if (sysfs_streq(buf, "0") && msd.dstat.acl_on) {
			msd.dstat.acl_on = false;
			is_acl_on = false;
		} else
			pr_info("%s: Invalid argument!!", __func__);
	} else {
		if (sysfs_streq(buf, "1") && !msd.dstat.acl_on) {
			if (msd.mpd->set_acl  && msd.mpd->set_acl(mfd->bl_level))
				mipi_ams367_send_cmd(mfd, PANEL_ACL_OFF, true);
			else {
				mipi_ams367_send_cmd(mfd, PANEL_ACL_ON, true);
				mipi_ams367_send_cmd(mfd, PANEL_ACL_UPDATE, true);
			}
			msd.dstat.acl_on = true;
			is_acl_on = true;
		} else if (sysfs_streq(buf, "0") && msd.dstat.acl_on) {
			mipi_ams367_send_cmd(mfd, PANEL_ACL_OFF, true);
			msd.dstat.acl_on = false;
			is_acl_on = false;
		} else {
			pr_info("%s: Invalid argument!!", __func__);
		}
	}

	return size;
}

static ssize_t mipi_samsung_auto_brightness_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", msd.dstat.auto_brightness);
	pr_info("auot_brightness: %d\n", *buf);

	return rc;
}

static ssize_t mipi_samsung_auto_brightness_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
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
		msd.dstat.auto_brightness = 6;
	else
		pr_info("%s: Invalid argument!!", __func__);

	return size;
}

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
void mipi_dsi_configure_dividers(int fps);
int mipi_AMS367_dynamic_fps_folder(int is_folder_action, struct msm_panel_info *pinfo)
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

	if (level == 0)
		goal_fps = 60;
	else if (level == 1)
		goal_fps = 51; /*42; 42fps -> gray screen occured */
	else if (level == 2)
		goal_fps = 51;
	else {
		pr_info("%s fps set error : invalid level %d", __func__, level);
		return size;
	}

	if (current_fps != goal_fps)
		current_fps = goal_fps;
	else
		return size;

	mutex_lock(&mfd->dma->ov_mutex);

	if (mfd->panel_power_on == FALSE) {
		mutex_unlock(&mfd->dma->ov_mutex);
		pr_info("%s fps set error, panel power off 2", __func__);
		return size;
	} else {
		mipi_runtime_clk_change(current_fps);
		mutex_unlock(&mfd->dma->ov_mutex);
	}

	pr_info("%s goal_fps : %d", __func__, goal_fps);

	return size;
}

#endif

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
static DEVICE_ATTR(lcd_power, S_IRUGO | S_IWUSR, mipi_samsung_disp_get_power, mipi_samsung_disp_set_power);
#endif
static DEVICE_ATTR(lcd_type, S_IRUGO, mipi_samsung_disp_lcdtype_show, NULL);
static DEVICE_ATTR(gamma_mode, S_IRUGO | S_IWUSR | S_IWGRP, mipi_samsung_disp_gamma_mode_show, mipi_samsung_disp_gamma_mode_store);
static DEVICE_ATTR(power_reduce, S_IRUGO | S_IWUSR | S_IWGRP, mipi_samsung_disp_acl_show, mipi_samsung_disp_acl_store);
static DEVICE_ATTR(auto_brightness, S_IRUGO | S_IWUSR | S_IWGRP, mipi_samsung_auto_brightness_show, mipi_samsung_auto_brightness_store);

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
static DEVICE_ATTR(fps_change, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_samsung_fps_show,
			mipi_samsung_fps_store);
#endif

#endif

#ifdef READ_REGISTER_ESD
#define ID_05H_IDLE 0x0
#define ID_E5H_IDLE 0x80
#define ID_0AH_IDLE 0x9c
static char error_id1[2] = {
	0x05, 0x00
};				/* DTYPE_DCS_READ */

static char error_id2[2] = {
	0xE5, 0x00
};				/* DTYPE_DCS_READ */

static char error_id3[2] = {
	0x0A, 0x00
};				/* DTYPE_DCS_READ */

static struct dsi_cmd_desc error_id1_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 0, sizeof(error_id1), error_id1
};

static struct dsi_cmd_desc error_id2_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 0, sizeof(error_id2), error_id2
};

static struct dsi_cmd_desc error_id3_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 0, sizeof(error_id3), error_id3
};

static char error_buf[3];
static void read_error_register(struct msm_fb_data_type *mfd)
{

	struct dsi_buf *rp, *tp;
	struct dsi_cmd_desc *cmd;

	mipi_ams367_send_cmd(mfd, MTP_READ_ENABLE, false);
	/*      wake_lock(&idle_wake_lock); */
	/*temp */
	mutex_lock(&mfd->dma->ov_mutex);

	mdp4_dsi_cmd_dma_busy_wait(mfd);
	mdp4_dsi_blt_dmap_busy_wait(mfd);
	mipi_dsi_mdp_busy_wait();

	tp = &msd.samsung_tx_buf;
	rp = &msd.samsung_rx_buf;

	cmd = &error_id2_cmd;
	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);
	error_buf[1] = *rp->data;

	cmd = &error_id3_cmd;
	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);
	error_buf[2] = *rp->data;

	mutex_unlock(&mfd->dma->ov_mutex);
	/*      wake_unlock(&idle_wake_lock); */
	/*temp */
}

static void esd_test_work_func(struct work_struct *work)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (unlikely(!mfd)) {
		pr_info("%s NO PDEV.\n", __func__);
		return;
	}

	pr_debug("%s start", __func__);
	read_error_register(mfd);
	pr_debug("%s end E5H=0x%x 0AH=%x\n", __func__, error_buf[1], error_buf[2]);

	if ((ID_E5H_IDLE ^ error_buf[1]) || (ID_0AH_IDLE ^ error_buf[2])) {

		pr_info("%s: E5H=%x 0AH=%x\n", __func__, error_buf[1], error_buf[2]);

		esd_execute();
	}

/*	if (mfd->resume_state != MIPI_SUSPEND_STATE)//temp
		queue_delayed_work(msd.mpd->esd_workqueue,
				&(msd.mpd->esd_work), ESD_INTERVAL * HZ); */

}
#endif

static int mipi_samsung_disp_probe(struct platform_device *pdev)
{
	int ret = 0;
	int i;
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	struct platform_device *msm_fb_added_dev;
#endif
#if defined(CONFIG_LCD_CLASS_DEVICE)
	struct lcd_device *lcd_device;
#endif
#if defined(CONFIG_BACKLIGHT_CLASS_DEVICE)
	struct backlight_device *bd;
#endif

	msd.dstat.acl_on = false;
	is_acl_on = false;
	if (pdev->id == 0) {
		msd.mipi_samsung_disp_pdata = pdev->dev.platform_data;
		first_on = false;
		ret = 0;
		return ret;
	}
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	msm_fb_added_dev = msm_fb_add_device(pdev);
	msd.msm_pdev = msm_fb_added_dev;
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
	msd.early_suspend.suspend = mipi_samsung_disp_early_suspend;
	msd.early_suspend.resume = mipi_samsung_disp_late_resume;
	msd.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&msd.early_suspend);

#endif
	msd.mpd->lcd_no = 0;
	for (i = 0; i < LCD_LOTS; i++) {
		msd.dstat.is_elvss_loaded[i] = false;
		msd.dstat.is_smart_dim_loaded[i] = false;
	}

#if defined(CONFIG_LCD_CLASS_DEVICE)
	lcd_device = lcd_device_register("panel", &pdev->dev, NULL, &mipi_samsung_disp_props);

	if (IS_ERR(lcd_device)) {
		ret = PTR_ERR(lcd_device);
		pr_err("lcd : failed to register device\n");
		return ret;
	}
#ifdef WA_FOR_FACTORY_MODE
	sysfs_remove_file(&lcd_device->dev.kobj, &dev_attr_lcd_power.attr);
	ret = sysfs_create_file(&lcd_device->dev.kobj, &dev_attr_lcd_power.attr);
	if (ret)
		pr_info("sysfs create fail-%s\n", dev_attr_lcd_power.attr.name);
#endif
	ret = sysfs_create_file(&lcd_device->dev.kobj, &dev_attr_lcd_type.attr);
	if (ret)
		pr_info("sysfs create fail-%s\n", dev_attr_lcd_type.attr.name);
	ret = sysfs_create_file(&lcd_device->dev.kobj, &dev_attr_gamma_mode.attr);
	if (ret)
		pr_info("sysfs create fail-%s\n", dev_attr_gamma_mode.attr.name);
	ret = sysfs_create_file(&lcd_device->dev.kobj, &dev_attr_power_reduce.attr);
	if (ret)
		pr_info("sysfs create fail-%s\n", dev_attr_power_reduce.attr.name);

#if defined(CONFIG_RUNTIME_MIPI_CLK_CHANGE)
	ret = sysfs_create_file(&lcd_device->dev.kobj,
						&dev_attr_fps_change.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_fps_change.attr.name);
	}
#endif

#if defined(CONFIG_BACKLIGHT_CLASS_DEVICE)
	bd = backlight_device_register("panel", &lcd_device->dev, NULL, NULL, NULL);
	if (IS_ERR(bd)) {
		ret = PTR_ERR(bd);
		pr_info("backlight : failed to register device\n");
		return ret;
	}

	ret = sysfs_create_file(&bd->dev.kobj, &dev_attr_auto_brightness.attr);
	if (ret)
		pr_info("sysfs create fail-%s\n", dev_attr_auto_brightness.attr.name);
#endif
#endif
	return ret;
}

static struct platform_driver this_driver = {
	.probe = mipi_samsung_disp_probe,
	.driver = {
		   .name = "mipi_ams367_oled",
		   },
	.shutdown = mipi_samsung_disp_shutdown
};

static struct msm_fb_panel_data samsung_panel_data = {
	.on = mipi_samsung_disp_on,
	.off = mipi_samsung_disp_off,
	.set_backlight = mipi_samsung_disp_backlight,
};

static int ch_used[3];
int ams367_mipi_samsung_device_register(struct msm_panel_info *pinfo, u32 channel, u32 panel, struct mipi_panel_data *mpd)
{
	struct platform_device *pdev = NULL;
	int ret = 0;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	pdev = platform_device_alloc("mipi_ams367_oled", (panel << 8) | channel);
	if (!pdev)
		return -ENOMEM;

	samsung_panel_data.panel_info = *pinfo;
	msd.mpd = mpd;
	if (!msd.mpd) {
		pr_err("%s: get mipi_panel_data failed!\n", __func__);
		goto err_device_put;
	}
	mpd->msd = &msd;
	ret = platform_device_add_data(pdev, &samsung_panel_data, sizeof(samsung_panel_data));
	if (ret) {
		pr_err("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_err("%s: platform_device_register failed!\n", __func__);
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
	 *      1 is recovery booting
	 *      0 is normal booting
	 */

	if (strncmp(mode, "1", 1) == 0)
		recovery_boot_mode = 1;
	else
		recovery_boot_mode = 0;

	pr_debug("%s %s", __func__, recovery_boot_mode == 1 ? "recovery" : "normal");
	return 1;
}

__setup("androidboot.boot_recovery=", current_boot_mode);
static int __init mipi_samsung_disp_init(void)
{
	mipi_dsi_buf_alloc(&msd.samsung_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&msd.samsung_rx_buf, DSI_BUF_SIZE);

/*	wake_lock_init(&idle_wake_lock,
	WAKE_LOCK_IDLE, "MIPI idle lock");*/
	/*temp */
	return platform_driver_register(&this_driver);
}

module_init(mipi_samsung_disp_init);
