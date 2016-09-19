//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef EXECUTION_STATE_EXIT_STEP_C20999E7_5BFF_464B_905E_7192F6153955
#define EXECUTION_STATE_EXIT_STEP_C20999E7_5BFF_464B_905E_7192F6153955


#include "execution_step.h"


namespace sxy
{


class state;


class execution_state_exit_step final:
	public execution_step
{
public:
	explicit execution_state_exit_step( state& _state );
	virtual ~execution_state_exit_step() override;
	execution_state_exit_step( const execution_state_exit_step& ) = delete;
	execution_state_exit_step& operator=( const execution_state_exit_step& ) = delete;
	virtual bool execute_behavior( event_processing_callback* const _event_processing_callback,
		const event& _event, behavior_exceptions& _behavior_exceptions, 
		async_event_handler* const _async_event_handler ) const override;
	virtual void accept( execution_step_visitor& _visitor ) const override;
	const state& get_state() const;


private:
	state& state_;
};


}


#endif
