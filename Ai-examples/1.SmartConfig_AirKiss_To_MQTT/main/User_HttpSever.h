#ifndef __HTTP_SEVER_H__
#define __HTTP_SEVER_H__

void HttpSever_Init(void);

/**
 * @description: 
 * @param {type} 
 * @return: 
 */
void NetRc_save_info(char *Device_Mac);

/**
 * @description: 
 * @param {type} 
 * @return: 
 */
int NetRc_Read_info(uint8_t *data);
#endif
