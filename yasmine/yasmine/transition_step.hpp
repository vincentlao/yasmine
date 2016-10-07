//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TRANSITION_STEP_6023D41F_1644_441E_8A49_E2BE433910AF
#define TRANSITION_STEP_6023D41F_1644_441E_8A49_E2BE433910AF


#include "compatibility.hpp"
#include "vertex_fwd.hpp"
#include "transition_fwd.hpp"
#include "transition_step_fwd.hpp"


namespace sxy
{


class entry_point;
class exit_point;
class event;


class transition_step
{
public:
	transition_step() = default;
	virtual ~transition_step() noexcept = default;
	transition_step( const transition_step& ) = delete;
	transition_step& operator=( const transition_step& ) = delete;
	virtual const raw_transitions& get_transitions() const = 0;
	virtual const vertex& get_unique_source() const = 0;
	virtual const vertex& get_unique_target() const = 0;
	virtual void execute_transition_behaviors( const event& _event ) const = 0;
	virtual const exit_point * get_exit_point() const = 0;
	virtual const entry_point * get_entry_point() const = 0;
	virtual const raw_const_vertices get_target_vertices() = 0;
};


}


#endif
