
#include <stdlib.h>
#include <string.h>
#include "sc_types.h"
#include "Prefix.h"
#include "PrefixRequired.h"
/*! \file Implementation of the state machine 'prefix'
*/

/* prototypes of all internal functions */
static sc_boolean prefix_check_main_region_APAGADO_tr0_tr0(const Prefix* handle);
static sc_boolean prefix_check_main_region_APAGADO_lr1_lr1(const Prefix* handle);
static sc_boolean prefix_check_main_region_ENCENDIDO_tr0_tr0(const Prefix* handle);
static sc_boolean prefix_check_main_region_ENCENDIDO_lr1_lr1(const Prefix* handle);
static void prefix_effect_main_region_APAGADO_tr0(Prefix* handle);
static void prefix_effect_main_region_APAGADO_lr1_lr1(Prefix* handle);
static void prefix_effect_main_region_ENCENDIDO_tr0(Prefix* handle);
static void prefix_effect_main_region_ENCENDIDO_lr1_lr1(Prefix* handle);
static void prefix_enact_main_region_APAGADO(Prefix* handle);
static void prefix_enact_main_region_ENCENDIDO(Prefix* handle);
static void prefix_enseq_main_region_APAGADO_default(Prefix* handle);
static void prefix_enseq_main_region_ENCENDIDO_default(Prefix* handle);
static void prefix_enseq_main_region_default(Prefix* handle);
static void prefix_exseq_main_region_APAGADO(Prefix* handle);
static void prefix_exseq_main_region_ENCENDIDO(Prefix* handle);
static void prefix_exseq_main_region(Prefix* handle);
static void prefix_react_main_region_APAGADO(Prefix* handle);
static void prefix_react_main_region_ENCENDIDO(Prefix* handle);
static void prefix_react_main_region__entry_Default(Prefix* handle);
static void prefix_clearInEvents(Prefix* handle);
static void prefix_clearOutEvents(Prefix* handle);

const sc_integer PREFIX_PREFIXIFACE_LED3 = 5;
const sc_boolean PREFIX_PREFIXIFACE_LED_ON = bool_false;
const sc_boolean PREFIX_PREFIXIFACE_LED_OFF = bool_true;
const sc_integer PREFIX_PREFIXINTERNAL_CI250MS = 250;
const sc_integer PREFIX_PREFIXINTERNAL_CI500MS = 500;

void prefix_init(Prefix* handle)
{
	sc_integer i;

	for (i = 0; i < PREFIX_MAX_ORTHOGONAL_STATES; ++i)
	{
		handle->stateConfVector[i] = Prefix_last_state;
	}
	
	
	handle->stateConfVectorPosition = 0;

	prefix_clearInEvents(handle);
	prefix_clearOutEvents(handle);

	/* Default init sequence for statechart prefix */
	handle->internal.viTitilar = 0;

}

void prefix_enter(Prefix* handle)
{
	/* Default enter sequence for statechart prefix */
	prefix_enseq_main_region_default(handle);
}

void prefix_exit(Prefix* handle)
{
	/* Default exit sequence for statechart prefix */
	prefix_exseq_main_region(handle);
}

sc_boolean prefix_isActive(const Prefix* handle)
{
	sc_boolean result;
	if (handle->stateConfVector[0] != Prefix_last_state)
	{
		result =  bool_true;
	}
	else
	{
		result = bool_false;
	}
	return result;
}

/* 
 * Always returns 'false' since this state machine can never become final.
 */
sc_boolean prefix_isFinal(const Prefix* handle)
{
   return bool_false;
}

static void prefix_clearInEvents(Prefix* handle)
{
	handle->iface.evTick_raised = bool_false;
}

static void prefix_clearOutEvents(Prefix* handle)
{
}

void prefix_runCycle(Prefix* handle)
{
	
	prefix_clearOutEvents(handle);
	
	for (handle->stateConfVectorPosition = 0;
		handle->stateConfVectorPosition < PREFIX_MAX_ORTHOGONAL_STATES;
		handle->stateConfVectorPosition++)
		{
			
		switch (handle->stateConfVector[handle->stateConfVectorPosition])
		{
		case Prefix_main_region_APAGADO :
		{
			prefix_react_main_region_APAGADO(handle);
			break;
		}
		case Prefix_main_region_ENCENDIDO :
		{
			prefix_react_main_region_ENCENDIDO(handle);
			break;
		}
		default:
			break;
		}
	}
	
	prefix_clearInEvents(handle);
}


sc_boolean prefix_isStateActive(const Prefix* handle, PrefixStates state)
{
	sc_boolean result = bool_false;
	switch (state)
	{
		case Prefix_main_region_APAGADO :
			result = (sc_boolean) (handle->stateConfVector[0] == Prefix_main_region_APAGADO
			);
			break;
		case Prefix_main_region_ENCENDIDO :
			result = (sc_boolean) (handle->stateConfVector[0] == Prefix_main_region_ENCENDIDO
			);
			break;
		default:
			result = bool_false;
			break;
	}
	return result;
}

void prefixIface_raise_evTick(Prefix* handle)
{
	handle->iface.evTick_raised = bool_true;
}


const sc_integer prefixIface_get_lED3(const Prefix* handle)
{
	return PREFIX_PREFIXIFACE_LED3;
}
const sc_boolean prefixIface_get_lED_ON(const Prefix* handle)
{
	return PREFIX_PREFIXIFACE_LED_ON;
}
const sc_boolean prefixIface_get_lED_OFF(const Prefix* handle)
{
	return PREFIX_PREFIXIFACE_LED_OFF;
}

/* implementations of all internal functions */

static sc_boolean prefix_check_main_region_APAGADO_tr0_tr0(const Prefix* handle)
{
	return ((handle->iface.evTick_raised) && (handle->internal.viTitilar == 0)) ? bool_true : bool_false;
}

static sc_boolean prefix_check_main_region_APAGADO_lr1_lr1(const Prefix* handle)
{
	return ((handle->iface.evTick_raised) && (handle->internal.viTitilar != 0)) ? bool_true : bool_false;
}

static sc_boolean prefix_check_main_region_ENCENDIDO_tr0_tr0(const Prefix* handle)
{
	return ((handle->iface.evTick_raised) && (handle->internal.viTitilar == 0)) ? bool_true : bool_false;
}

static sc_boolean prefix_check_main_region_ENCENDIDO_lr1_lr1(const Prefix* handle)
{
	return ((handle->iface.evTick_raised) && (handle->internal.viTitilar != 0)) ? bool_true : bool_false;
}

static void prefix_effect_main_region_APAGADO_tr0(Prefix* handle)
{
	prefix_exseq_main_region_APAGADO(handle);
	prefix_enseq_main_region_ENCENDIDO_default(handle);
}

static void prefix_effect_main_region_APAGADO_lr1_lr1(Prefix* handle)
{
	handle->internal.viTitilar -= 1;
}

static void prefix_effect_main_region_ENCENDIDO_tr0(Prefix* handle)
{
	prefix_exseq_main_region_ENCENDIDO(handle);
	prefix_enseq_main_region_APAGADO_default(handle);
}

static void prefix_effect_main_region_ENCENDIDO_lr1_lr1(Prefix* handle)
{
	handle->internal.viTitilar -= 1;
}

/* Entry action for state 'APAGADO'. */
static void prefix_enact_main_region_APAGADO(Prefix* handle)
{
	/* Entry action for state 'APAGADO'. */
	prefixIface_opLED(handle, PREFIX_PREFIXIFACE_LED3, PREFIX_PREFIXIFACE_LED_OFF);
	handle->internal.viTitilar = PREFIX_PREFIXINTERNAL_CI250MS;
}

/* Entry action for state 'ENCENDIDO'. */
static void prefix_enact_main_region_ENCENDIDO(Prefix* handle)
{
	/* Entry action for state 'ENCENDIDO'. */
	prefixIface_opLED(handle, PREFIX_PREFIXIFACE_LED3, PREFIX_PREFIXIFACE_LED_ON);
	handle->internal.viTitilar = PREFIX_PREFIXINTERNAL_CI500MS;
}

/* 'default' enter sequence for state APAGADO */
static void prefix_enseq_main_region_APAGADO_default(Prefix* handle)
{
	/* 'default' enter sequence for state APAGADO */
	prefix_enact_main_region_APAGADO(handle);
	handle->stateConfVector[0] = Prefix_main_region_APAGADO;
	handle->stateConfVectorPosition = 0;
}

/* 'default' enter sequence for state ENCENDIDO */
static void prefix_enseq_main_region_ENCENDIDO_default(Prefix* handle)
{
	/* 'default' enter sequence for state ENCENDIDO */
	prefix_enact_main_region_ENCENDIDO(handle);
	handle->stateConfVector[0] = Prefix_main_region_ENCENDIDO;
	handle->stateConfVectorPosition = 0;
}

/* 'default' enter sequence for region main region */
static void prefix_enseq_main_region_default(Prefix* handle)
{
	/* 'default' enter sequence for region main region */
	prefix_react_main_region__entry_Default(handle);
}

/* Default exit sequence for state APAGADO */
static void prefix_exseq_main_region_APAGADO(Prefix* handle)
{
	/* Default exit sequence for state APAGADO */
	handle->stateConfVector[0] = Prefix_last_state;
	handle->stateConfVectorPosition = 0;
}

/* Default exit sequence for state ENCENDIDO */
static void prefix_exseq_main_region_ENCENDIDO(Prefix* handle)
{
	/* Default exit sequence for state ENCENDIDO */
	handle->stateConfVector[0] = Prefix_last_state;
	handle->stateConfVectorPosition = 0;
}

/* Default exit sequence for region main region */
static void prefix_exseq_main_region(Prefix* handle)
{
	/* Default exit sequence for region main region */
	/* Handle exit of all possible states (of prefix.main_region) at position 0... */
	switch(handle->stateConfVector[ 0 ])
	{
		case Prefix_main_region_APAGADO :
		{
			prefix_exseq_main_region_APAGADO(handle);
			break;
		}
		case Prefix_main_region_ENCENDIDO :
		{
			prefix_exseq_main_region_ENCENDIDO(handle);
			break;
		}
		default: break;
	}
}

/* The reactions of state APAGADO. */
static void prefix_react_main_region_APAGADO(Prefix* handle)
{
	/* The reactions of state APAGADO. */
	if (prefix_check_main_region_APAGADO_tr0_tr0(handle) == bool_true)
	{ 
		prefix_effect_main_region_APAGADO_tr0(handle);
	}  else
	{
		if (prefix_check_main_region_APAGADO_lr1_lr1(handle) == bool_true)
		{ 
			prefix_effect_main_region_APAGADO_lr1_lr1(handle);
		} 
	}
}

/* The reactions of state ENCENDIDO. */
static void prefix_react_main_region_ENCENDIDO(Prefix* handle)
{
	/* The reactions of state ENCENDIDO. */
	if (prefix_check_main_region_ENCENDIDO_tr0_tr0(handle) == bool_true)
	{ 
		prefix_effect_main_region_ENCENDIDO_tr0(handle);
	}  else
	{
		if (prefix_check_main_region_ENCENDIDO_lr1_lr1(handle) == bool_true)
		{ 
			prefix_effect_main_region_ENCENDIDO_lr1_lr1(handle);
		} 
	}
}

/* Default react sequence for initial entry  */
static void prefix_react_main_region__entry_Default(Prefix* handle)
{
	/* Default react sequence for initial entry  */
	prefix_enseq_main_region_APAGADO_default(handle);
}


