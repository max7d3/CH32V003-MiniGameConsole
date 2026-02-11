#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#define BTN_DEBOUNCE_TIME 10

typedef enum
{
    BTN_ACTION,
    BTN_UP,
    BTN_DOWN,
    BTN_RIGHT,
    BTN_LEFT,
    BTN_COUNT

}buttons_names_t;

typedef enum
{
    BTN_STATE_IDLE,
	BTN_STATE_DEBOUNCE,
    BTN_STATE_PRESSED,

}buttons_states_t;

typedef struct
{
	buttons_states_t state;
	uint32_t last_tick;

}button_status_t;

void BTN_Init(void);
void BTN_Task(void);
uint8_t BTN_IsPressed(buttons_names_t button);
// Not debounced but without any lag
uint8_t BTN_IsPressedRAW(buttons_names_t button);

#endif /* _BUTTONS_H_ */