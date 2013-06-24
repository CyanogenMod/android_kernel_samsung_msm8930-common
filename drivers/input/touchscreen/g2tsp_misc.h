/*



*/
#ifndef __G2TSP_MISC_H__
#define __G2TSP_MISC_H__


//void g2tsp_firmware_load(struct g2tsp 	*ts, const unsigned char *data, size_t size);
void firmware_request_handler(const struct firmware *fw, void *context);
void g2tsp_flash_eraseall(struct g2tsp *ts);



#endif //__G2TSP_MISC_H__

