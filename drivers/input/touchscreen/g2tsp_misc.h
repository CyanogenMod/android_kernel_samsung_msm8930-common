/*



*/
#ifndef __G2TSP_MISC_H__
#define __G2TSP_MISC_H__


extern const u16 Refence_dac[];

//void g2tsp_firmware_load(struct g2tsp 	*ts, const unsigned char *data, size_t size);
void firmware_request_handler(const struct firmware *fw, void *context);
void g2tsp_flash_eraseall(struct g2tsp *ts, u8 cmd);
void g2tsp_dac_write(struct g2tsp *ts, u8 *data);
void PushDbgPacket(struct g2tsp *ts, u8 *src, u16 len);
int PopDbgPacket(struct g2tsp *ts, u8 *src);
void MakeI2cDebugBuffer(u8 *dst, u8 *src, u8 fcnt);
bool ReadTSPDbgData(struct g2tsp *ts, u8 type, u8 *frameBuf); 
void dbgMsgToCheese(const char *fmt, ...);
void TSPFrameCopytoBuf(u16 *dacBuf, u8* data);
void TSPFrameCopytoBuf_swap16(u16 *dacBuf, u8* data);


#endif //__G2TSP_MISC_H__

