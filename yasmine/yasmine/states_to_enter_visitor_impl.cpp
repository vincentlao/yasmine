//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "states_to_enter_visitor_impl.hpp"

#include "base.hpp"
#include "simple_state.hpp"
#include "composite_state.hpp"
#include "final_state.hpp"
#include "region.hpp"
#include "deep_history.hpp"
#include "shallow_history.hpp"


namespace sxy
{


states_to_enter_visitor_impl::states_to_enter_visitor_impl( raw_states_by_nesting_level& _states_to_enter,
	raw_const_region_set& _regions_to_enter, composite_state& _LCA_of_compound_transition )
	: states_to_enter_( _states_to_enter ),
		regions_to_enter_( _regions_to_enter ),
		LCA_of_compound_transition_( _LCA_of_compound_transition )
{
	// Nothing to do...
}


void states_to_enter_visitor_impl::visit( composite_state& _composite_state )
{
	insert_states_to_enter( _composite_state );
}


void states_to_enter_visitor_impl::visit( simple_state& _simple_state )
{
	insert_states_to_enter( _simple_state );
}


void states_to_enter_visitor_impl::visit( final_state& _final_state )
{
	insert_states_to_enter( _final_state );
}


void states_to_enter_visitor_impl::visit( initial_pseudostate& _initial_pseudostate )
{
	Y_UNUSED_PARAMETER( _initial_pseudostate );
	Y_ASSERT( false, "A pseudostate cannot be entered!" );
}


void states_to_enter_visitor_impl::visit( choice& _choice )
{
	Y_UNUSED_PARAMETER( _choice );	
}


void states_to_enter_visitor_impl::visit( junction& _junction )
{
	Y_UNUSED_PARAMETER( _junction );
	Y_ASSERT( false, "A pseudostate cannot be entered!" );
}


void states_to_enter_visitor_impl::visit( join& _join )
{
	Y_UNUSED_PARAMETER( _join );
	Y_ASSERT( false, "A pseudostate cannot be entered!" );
}


void states_to_enter_visitor_impl::visit( fork& _fork )
{
	Y_UNUSED_PARAMETER( _fork );
	Y_ASSERT( false, "A pseudostate cannot be entered!" );
}


void states_to_enter_visitor_impl::visit( entry_point& _entry_point )
{
	Y_UNUSED_PARAMETER( _entry_point );
	Y_ASSERT( false, "A pseudostate cannot be entered!" );
}


void states_to_enter_visitor_impl::visit( exit_point& _exit_point )
{
	Y_UNUSED_PARAMETER( _exit_point );
	Y_ASSERT( false, "A pseudostate cannot be entered!" );
}


void states_to_enter_visitor_impl::visit( deep_history& _deep_history )
{
	auto& parent_state = _deep_history.get_parent_state();
	states_to_enter_.insert( &parent_state );
	add_last_active_child_states_to_enter( parent_state );
}


void states_to_enter_visitor_impl::visit( shallow_history& _shallow_history )
{
	auto& parent_state = _shallow_history.get_parent_state();
	states_to_enter_.insert( &parent_state );

	for( const auto & region : parent_state.get_regions() )
	{
		const auto last_active_state_of_region = region->get_last_active_state();
		if( last_active_state_of_region )
		{
			states_to_enter_.insert( last_active_state_of_region );
		}

		regions_to_enter_.insert( last_active_state_of_region->get_parent_region() );
	}
}


void states_to_enter_visitor_impl::visit( terminate_pseudostate& _terminate_pseudostate )
{
	Y_UNUSED_PARAMETER( _terminate_pseudostate );
}


void states_to_enter_visitor_impl::get_states_up_to_LCA( state& _state )
{
	auto ancestors = _state.get_ancestors( &LCA_of_compound_transition_ );

	for( auto ancestor : ancestors )
	{
		if( ancestor != &LCA_of_compound_transition_ )
		{
			states_to_enter_.insert( ancestor );
		}
	}

	states_to_enter_.insert( &_state );
}


void states_to_enter_visitor_impl::get_regions_up_to_LCA( const state& _state )
{
	auto ancestors_as_regions = _state.get_ancestors_as_regions();

	for( auto ancestor_as_region : ancestors_as_regions )
	{
		if( &ancestor_as_region->get_parent_state() != &LCA_of_compound_transition_ )
		{
			regions_to_enter_.insert( ancestor_as_region );
		}
	}
}


void states_to_enter_visitor_impl::add_last_active_child_states_to_enter( const state& _state )
{
	for( auto & region : _state.get_regions() )
	{
		const auto state = region->get_last_active_state();
		states_to_enter_.insert( state );
		regions_to_enter_.insert( state->get_parent_region() );
		add_last_active_child_states_to_enter( *state );
	}
}


void states_to_enter_visitor_impl::insert_states_to_enter( state& _state )
{
	states_to_enter_.insert( &_state );
	regions_to_enter_.insert( _state.get_parent_region() );
	get_states_up_to_LCA( _state );
	get_regions_up_to_LCA( _state );
}


}
