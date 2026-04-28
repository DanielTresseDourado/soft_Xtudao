#include "Micro_III_Functions.h"


void ST_Init(type_ST *pST, uint32_t time_lapse)
{
	pST->initial_time = HAL_GetTick();
	pST->elapsed_time = 0;
	pST->delay_time = time_lapse;
}
type_bool ST(type_ST *pST)
{
	pST->elapsed_time = HAL_GetTick() - pST->initial_time;
	if (pST->elapsed_time >= pST->delay_time)
	{
		return True;
	}
	else return False;
}
void ST_Lapse(type_ST *pST)
{
	pST->initial_time = pST->initial_time + pST->delay_time;
	pST->elapsed_time = 0;
}

void PWM_Init(type_PWM *pPWM,
	GPIO_TypeDef* GPIO_Port,
	uint16_t GPIO_Pin, 
	uint32_t Period,
	float Duty)
{
	pPWM->T = Period;
	if (Duty <= 0.0f)
		pPWM->duty_cycle = 0;
	else if (Duty >= 1.0f)
		pPWM->duty_cycle = 1.0;
	else
		pPWM->duty_cycle = Duty;
	pPWM->t_act = pPWM->T * pPWM->duty_cycle;
	pPWM->t_ina = pPWM->T - pPWM->t_act;
	
	pPWM->Port = GPIO_Port;
	pPWM->Pin = GPIO_Pin;
	
	pPWM->T_shadow = pPWM->T;
	pPWM->duty_cicle_shadow = pPWM->duty_cycle;
	pPWM->shadow = Inactive;
	
	pPWM->state = Active;
	ST_Init(&pPWM->timer, pPWM->t_act);
	HAL_GPIO_WritePin(pPWM->Port, pPWM->Pin, GPIO_PIN_SET);
}

void PWM_Run(type_PWM *pPWM)
{
	if (ST(&pPWM->timer))
	{
		ST_Lapse(&pPWM->timer);
		if (pPWM->state == Active)
		{
			pPWM->state = Inactive;
			HAL_GPIO_WritePin(pPWM->Port, pPWM->Pin, GPIO_PIN_RESET);
			pPWM->timer.delay_time = pPWM->t_ina;
		}
		else
		{
			pPWM->state = Active;
			if (pPWM->shadow)
			{
				pPWM->duty_cycle = pPWM->duty_cicle_shadow;
				pPWM->T = pPWM->T_shadow;
				pPWM->t_act = pPWM->T * pPWM->duty_cycle;
				pPWM->t_ina = pPWM->T - pPWM->t_act;
			}
			HAL_GPIO_WritePin(pPWM->Port, pPWM->Pin, GPIO_PIN_SET);
			pPWM->timer.delay_time = pPWM->t_act;

		}
	}
}

void PWM_Update(type_PWM *pPWM,
	uint32_t Period, 
	float Duty,
	type_bool_state shadow)
{
#if (0)
	pPWM->T = Period;
	if (Duty <= 0.0f)
		pPWM->duty_cycle = 0;
	else if (Duty >= 1.0f)
		pPWM->duty_cycle = 1.0;
	else
		pPWM->duty_cycle = Duty;
	pPWM->t_act = pPWM->T * pPWM->duty_cycle;
	pPWM->t_ina = pPWM->T - pPWM->t_act;
	
	pPWM->T_shadow = pPWM->T;
	pPWM->duty_cicle_shadow = pPWM->duty_cycle;
#endif
	
	pPWM->shadow = shadow;
	pPWM->T_shadow = Period;
	if (Duty <= 0.0f)
		pPWM->duty_cicle_shadow = 0;
	else if (Duty >= 1.0f)
		pPWM->duty_cicle_shadow = 1.0;
	else
		pPWM->duty_cicle_shadow = Duty;	
	if (!shadow)
	{
		pPWM->duty_cycle = pPWM->duty_cicle_shadow;
		pPWM->T = pPWM->T_shadow;
		pPWM->t_act = pPWM->T * pPWM->duty_cycle;
		pPWM->t_ina = pPWM->T - pPWM->t_act;
	}
}


void Button_Init(type_button *btn, GPIO_TypeDef *Port, uint16_t Pin, uint32_t debounce_time, type_transition_edge Edge)
{
	btn->atu = Active; // Começa ativo
	btn->ant = Active;
    
	btn->Port = Port;
	btn->Pin = Pin;
    
	btn->Edge = Edge;
	btn->transition_state = Detecting;
	btn->debounce_time = debounce_time;
}


type_transition_state Button_Debounce(type_button *btn)
{
	btn->atu = (type_bool_state)HAL_GPIO_ReadPin(btn->Port, btn->Pin);

	switch (btn->transition_state)
	{
	case Detecting:
		// Verifica se houve a transição baseada na borda escolhida
		// Rising: ant=Inactive (0) e atu=Active (1)
		// Falling: ant=Active (1) e atu=Inactive (0)
		if ((btn->Edge == Rising && btn->atu == Active && btn->ant == Inactive) ||
		    (btn->Edge == Falling && btn->atu == Inactive && btn->ant == Active))
		{
			ST_Init(&btn->timer_db, btn->debounce_time); 
			btn->transition_state = Possible_transition;
		}
		break;

	case Possible_transition:
		if (ST(&btn->timer_db))
		{
			// Se (Rising E Ativo) OU (Falling E Inativo)
			if ((btn->Edge == Rising  && btn->atu == Active) ||
			    (btn->Edge == Falling && btn->atu == Inactive))
			{
				btn->transition_state = Detected;
			}
			else
			{
				btn->transition_state = Detecting;
			}
		}
		break;

	case Detected:
		// O Main lê esse estado e nós voltamos a detectar no próximo ciclo
		btn->transition_state = Detecting;
		break;

	default:
		btn->transition_state = Detecting;
		break;
	}

	btn->ant = btn->atu;
	return btn->transition_state;
}
