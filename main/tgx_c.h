#ifdef __cplusplus
extern "C" {
#endif

void tgx_setup();
void tgx_next();

/**
 * 获取显存指针
 */
uint16_t * getFrame();

#ifdef __cplusplus
}
#endif
