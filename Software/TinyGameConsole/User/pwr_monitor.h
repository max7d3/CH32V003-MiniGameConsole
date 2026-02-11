#ifndef USER_POWER_MONITOR_H_
#define USER_POWER_MONITOR_H_

void PWR_MONITOR_Init(void);
void PWR_MONITOR_Loop(void);

uint8_t PWR_Monitor_IsPowerLow(void);

#endif /* USER_POWER_MONITOR_H_ */