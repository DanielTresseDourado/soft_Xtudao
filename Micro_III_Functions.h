#pragma once
#ifndef Micro_III_Functions_H
#define Micro_III_Functions_H

#include "main.h"

typedef enum
{
	off = 0,
	on  = 1
}type_on_off;

typedef enum
{
	False = 0,
	True  = 1
}type_bool;

typedef struct
{
	uint32_t initial_time;
	uint32_t elapsed_time;
	uint32_t delay_time;
}type_ST; // tipo soft timer

typedef enum
{
	Inactive = 0,
	Active   = 1
}type_bool_state;

typedef struct
{
	type_bool_state state;
	uint32_t T;
	uint32_t t_act;
	uint32_t t_ina;
	float duty_cycle;
	GPIO_TypeDef *Port;
	uint16_t Pin;
	type_ST timer;
	uint32_t T_shadow;
	float duty_cicle_shadow;
	type_bool_state shadow;

}type_PWM; // tipo PWM

typedef enum
{
	Detecting           = 0,
	Possible_transition = 1,
	Detected            = 2
} type_transition_state;

typedef enum
{
	Falling,
	Rising
}
type_transition_edge;

typedef struct
{
	type_bool_state atu;
	type_bool_state ant;
	GPIO_TypeDef *Port;
	uint16_t Pin;
	type_transition_state transition_state;
	type_transition_edge Edge;
	
	type_ST timer_db;
	uint32_t debounce_time;

} type_button;

#endif