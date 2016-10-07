//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "state_machine.hpp"

#include <memory>

#include "make_unique.hpp"
#include "base.hpp"
#include "log_and_throw.hpp"
#include "event_processing_callback.hpp"
#include "composite_state_impl.hpp"
#include "event_impl.hpp"
#include "transition_controller.hpp"
#include "uri.hpp"
#include "region.hpp"
#include "behavior_impl.hpp"
#include "constraint_impl.hpp"
#include "transition_impl.hpp"
#include "exception.hpp"
#include "async_simple_state_impl.hpp"


namespace sxy
{


state_machine::state_machine( const std::string& _name,	event_processing_callback* const _event_processing_callback )
	: name_( _name ),
		event_processing_callback_( _event_processing_callback ),
		root_state_( sxy::make_unique< composite_state_impl >( _name ) ),
		transitions_(),
		deferred_events_(),
		state_machine_is_running_( false )
#ifdef Y_PROFILER
		, processed_events_(0)
#endif

{
	Y_LOG( sxy::log_level::LL_TRACE, "Creating state_machine '%'.", _name );
	if( event_processing_callback_ )
	{
		event_processing_callback_->add_state_machine_introspection( *this );
	}
	Y_LOG( sxy::log_level::LL_TRACE, "Created state_machine '%'.", _name );
}


state_machine::~state_machine() noexcept
{
#ifdef Y_PROFILER
	Y_LOG( sxy::log_level::LL_TRACE, "events fired by '%': %.", name_, processed_events_ );
#endif
	
	Y_LOG( sxy::log_level::LL_TRACE, "Destroying state_machine '%'.", name_ );

	Y_ASSERT( !state_machine_is_running_, "State machine is still running!" );

	Y_LOG( sxy::log_level::LL_TRACE, "Destroyed state_machine '%'.", name_ );
}


composite_state& state_machine::get_root_state() const
{
	Y_ASSERT( root_state_, "No root state!" );
	return( *root_state_ );
}


#ifdef Y_PROFILER	
// cppcheck-suppress unusedFunction
uint32_t state_machine::get_number_of_processed_events() const
{
	return( processed_events_ );
}
#endif


// cppcheck-suppress unusedFunction
transition& state_machine::add_transition( transition_uptr _transition )
{
	auto& transition = *_transition;
	transitions_.push_back( std::move( _transition ) );
	return( transition );
}


transition& state_machine::add_transition( const event_id _event_id, vertex& _source, vertex& _target, 
	const sxy::transition_kind _kind, const constraint_function& _guard, const behavior_function& _behavior )
{
	auto l_transition =
		sxy::make_unique< sxy::transition_impl >( _event_id, _source, _target, _kind,
			( _guard ? ( constraint_impl::create( _guard ) ) : nullptr ),
			( _behavior ? ( behavior_impl::create_behavior( _behavior ) ) : nullptr ) );
	auto& transition = *l_transition;
	transitions_.push_back( std::move( l_transition ) );
	return( transition );
}


transition& state_machine::add_transition( const event_ids _event_ids, vertex& _source, vertex& _target, 
	const sxy::transition_kind _kind, const constraint_function& _guard, const behavior_function& _behavior )
{																																																						 
	auto l_transition =
		sxy::make_unique< sxy::transition_impl >( _event_ids, _source, _target, _kind,
		( _guard ? ( constraint_impl::create( _guard ) ) : nullptr ),
																							( _behavior ? ( behavior_impl::create_behavior( _behavior ) ) : nullptr ) );
	auto& transition = *l_transition;
	transitions_.push_back( std::move( l_transition ) );
	return( transition );
}


transition& state_machine::add_transition( const event_id _event_id, vertex& _source, vertex& _target, 
	const constraint_function& _guard, const sxy::transition_kind _kind )
{
	auto l_transition =
		sxy::make_unique< sxy::transition_impl >( _event_id, _source, _target, _kind,
		( _guard ? ( constraint_impl::create( _guard ) ) : nullptr ), nullptr );
	auto& transition = *l_transition;
	transitions_.push_back( std::move( l_transition ) );
	return( transition );
}


transition& state_machine::add_transition( const event_id _event_id, vertex& _source, vertex& _target, 
	const constraint_function& _guard, const behavior_function& _behavior, const sxy::transition_kind _kind )
{
	auto l_transition =
		sxy::make_unique< sxy::transition_impl >( _event_id, _source, _target, _kind,
		( _guard ? ( constraint_impl::create( _guard ) ) : nullptr ),
		( _behavior ? ( behavior_impl::create_behavior( _behavior ) ) : nullptr ) );
	auto& transition = *l_transition;
	transitions_.push_back( std::move( l_transition ) );
	return( transition );
}


transition& state_machine::add_transition( const event_id _event_id, vertex& _source, vertex& _target, 
	const behavior_function& _behavior, const sxy::transition_kind _kind )
{		
	auto l_transition =
		sxy::make_unique< sxy::transition_impl >( _event_id, _source, _target, _kind,
		nullptr, ( _behavior ? ( behavior_impl::create_behavior( _behavior ) ) : nullptr ) );
	auto& transition = *l_transition;
	transitions_.push_back( std::move( l_transition ) );
	return( transition );
}


bool state_machine::fire_event( const event_sptr& _event )
{		
	Y_LOG( log_level::LL_INFO, "Firing & processing event '%' (%) with priority '%'.", _event->get_name(), 
		_event->get_id(), static_cast<int>(_event->get_priority()) );
	const auto terminate_pseudostate_has_been_reached = process_event( _event, nullptr );
	Y_LOG( log_level::LL_INFO, "Event '%' (%) has been fired & processed.", _event->get_name(), _event->get_id() );
	return( !terminate_pseudostate_has_been_reached );
}


bool state_machine::check( state_machine_defects& _defects ) const
{

	auto check_ok = root_state_->check( _defects );
	if( check_ok )
	{
		for( const auto & transition : transitions_ )
		{
			if( !transition->check( _defects ) )
			{
				check_ok = false;
				break;
			}
		}
	}

	return( check_ok );
}


bool state_machine::start_state_machine()
{
	Y_LOG( log_level::LL_INFO, "Starting state machine '%'.", name_ );

	const auto foo = start_state_machine( nullptr );

	Y_LOG( log_level::LL_INFO, "Started state machine '%'.", name_ );


	return( foo );
}


void state_machine::stop_state_machine()
{		 
	Y_LOG( log_level::LL_INFO, "Stopping state machine '%'.", name_ );

	Y_ASSERT( state_machine_is_running_, "State machine is not running!" );
	stop_all_async_states( *root_state_ );
	state_machine_is_running_ = false;

	Y_LOG( log_level::LL_INFO, "Stopped state machine '%'.", name_ );
}

std::string state_machine::get_name() const 
{
	return( name_ );
}


bool state_machine::start_state_machine( async_event_handler* const _async_event_handler )
{
	Y_ASSERT( root_state_, "No root state!" );
	auto state_machine_started = false;
	state_machine_is_running_ = true;
	try
	{
		if( event_processing_callback_ )
		{
			event_processing_callback_->before_event( COMPLETION_EVENT );
		}

		transition_controller transition_controller;
		state_machine_started = transition_controller.start_state_machine( *root_state_, event_processing_callback_,
			_async_event_handler );
		if( event_processing_callback_ )
		{
			event_processing_callback_->after_event( COMPLETION_EVENT );
		}

		if( !state_machine_started )
		{
			Y_LOG( sxy::log_level::LL_INFO, "Terminate pseudostate reached. The state machine is stopping." );
			state_machine_is_running_ = false;
		}
	}
	catch( const std::exception& exception )
	{
		Y_LOG( sxy::log_level::LL_FATAL, "State machine cannot start: %.", exception.what() );
		state_machine_is_running_ = false;
		throw;
	}
	catch( ... )
	{
		Y_LOG( sxy::log_level::LL_FATAL, "State machine can not start: Unknown exception occurred." );
		state_machine_is_running_ = false;
		throw;
	}
	return( state_machine_started );
}


bool state_machine::process_event( const event_sptr& _event, async_event_handler* const _async_event_handler )
{
#ifdef Y_PROFILER
	++processed_events_;
#endif

	Y_LOG( log_level::LL_INFO, "'%' is processing event '%' (%) with priority '%'.", get_name(), _event->get_name(), _event->get_id(), static_cast<int>(_event->get_priority()) );

	Y_ASSERT( state_machine_is_running_, "State machine is not running!" );
	auto terminate_pseudostate_has_been_reached = true;
	Y_LOG( log_level::LL_TRACE, "Starting processing of event '%' (%) with priority '%'.", _event->get_name(), _event->get_id(),
		static_cast<int>(_event->get_priority()) );
	try
	{
		if( event_processing_callback_ )
		{
			event_processing_callback_->before_event( _event->get_id(), _event->get_priority() );
		}
		
		transition_controller transition_controller;
		auto event_is_deferred = false;
		terminate_pseudostate_has_been_reached = transition_controller.process_event( *_event, *root_state_,
			event_processing_callback_, event_is_deferred, _async_event_handler );
		Y_LOG( log_level::LL_TRACE, "Event '%' (%) has been processed.", _event->get_name(), _event->get_id() );
		if( event_processing_callback_ )
		{
			event_processing_callback_->after_event( _event->get_id() );
		}

		if( terminate_pseudostate_has_been_reached )
		{
			Y_LOG( sxy::log_level::LL_INFO, "Terminate pseudostate has been reached! The state machine '%' is stopping.", get_name() );
			state_machine_is_running_ = false;
		}
		else
		{
			if( event_is_deferred )
			{
				add_deferred_event( _event );
			}
			else
			{
				terminate_pseudostate_has_been_reached = process_deferred_events( _async_event_handler );
			}
		}

		if( terminate_pseudostate_has_been_reached )
		{
			Y_LOG( sxy::log_level::LL_INFO, "Terminate pseudostate has been reached! The state machine '%' is stopping.", get_name() );
			state_machine_is_running_ = false;
		}
	}
	catch( const std::exception& exception )
	{
		Y_LOG( sxy::log_level::LL_FATAL, "std::exception occurred during event processing in state machine '%': %", get_name(), exception.what() );
		state_machine_is_running_ = false;
		throw;
	}
	catch( ... )
	{
		Y_LOG( sxy::log_level::LL_FATAL, "Unknown exception occurred in state machine '%'!", get_name() );
		state_machine_is_running_ = false;
		throw;
	}

	Y_LOG( log_level::LL_INFO, "'%' processed event '%' (%).", get_name(), _event->get_name(), _event->get_id());

	return( terminate_pseudostate_has_been_reached );
}


// cppcheck-suppress unusedFunction
const events& state_machine::get_deferred_events() const
{
	return( deferred_events_ );
}


// cppcheck-suppress unusedFunction
raw_const_states state_machine::get_active_state_configuration() const
{
	raw_const_states active_state_configuration = {};	
	const auto& root = get_root_state();
	check_regions_for_active_states( active_state_configuration, root );
	return( active_state_configuration );
}


void state_machine::get_active_states_from_region( raw_const_states& _active_state_configuration,
	const region& _region ) const
{
	const auto active_state = _region.get_active_state();
	if( active_state )
	{
		_active_state_configuration.push_back( active_state );
		check_regions_for_active_states( _active_state_configuration, *active_state );
	}
}


void state_machine::check_regions_for_active_states( raw_const_states& _active_state_configuration,
	const state& _state ) const
{
	for( const auto & region : _state.get_regions() )
	{
		get_active_states_from_region( _active_state_configuration, *region );
	}
}


void state_machine::add_deferred_event( const event_sptr& _event_id )
{
	deferred_events_.push_back( _event_id );
}


bool state_machine::process_deferred_events( async_event_handler* const _async_event_handler )
{
	const auto deferred_events = deferred_events_;
	deferred_events_.clear();
	auto terminate_pseudostate_has_been_reached = false;
	transition_controller transition_controller;

	for( const auto deferred_event : deferred_events )
	{
		if( event_processing_callback_ )
		{
			event_processing_callback_->before_event( deferred_event->get_id(), deferred_event->get_priority() );
		}

		auto event_is_deferred = false;
		terminate_pseudostate_has_been_reached = transition_controller.process_event( *deferred_event, *root_state_,
			event_processing_callback_, event_is_deferred, _async_event_handler );
		if( event_processing_callback_ )
		{
			event_processing_callback_->after_event( deferred_event->get_id() );
		}

		if( terminate_pseudostate_has_been_reached )
		{
			Y_LOG( sxy::log_level::LL_INFO, "Terminate pseudostate has been reached! The state machine is stopping." );
			break;
		}
		else
		{
			if( event_is_deferred )
			{
				deferred_events_.push_back( deferred_event );
			}
		}
	}

	return( terminate_pseudostate_has_been_reached );
}


void state_machine::stop_all_async_states( state& _state )
{		
	for( auto& region : _state.get_regions())
	{
		stop_all_async_states_from_region( region );
	}
}


void state_machine::stop_all_async_states_from_region( region_uptr& _region)
{
	auto active_state = _region->get_active_state();
	if( active_state )
	{
		auto async_state = dynamic_cast< async_simple_state_impl* >( active_state );
		if( async_state )
		{
			async_state->stop_do_behavior();
		}
		else
		{
			stop_all_async_states( *active_state );
		}
	}
}


}
