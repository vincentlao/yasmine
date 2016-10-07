//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "region_pseudostate_impl.hpp"

#include <algorithm>

#include "log.hpp"
#include "region.hpp"
#include "composite_state.hpp"
#include "exception.hpp"
#include "algorithm_parameters.hpp"


namespace sxy
{


region_pseudostate_impl::region_pseudostate_impl( const std::string& _name )
	: pseudostate_impl( _name ),
		parent_()
#ifdef Y_OPTIMIZE_4_SPEED
		,ancestors_(),
		 ancestors_as_regions_()
#endif
{
#ifdef Y_OPTIMIZE_4_SPEED
	ancestors_.reserve( ANCESTORS_VECTOR_SIZE );
	ancestors_as_regions_.reserve( ANCESTORS_VECTOR_SIZE );	
#endif	
}
			

const state_machine_element* region_pseudostate_impl::get_parent() const
{
	return( get_parent_region() );
}


region* region_pseudostate_impl::get_parent_region() const
{
	return( parent_ );
}


void region_pseudostate_impl::set_parent_region( region* const _parent_region )
{
	Y_LOG( log_level::LL_SPAM, "Setting '%' as parent region of '%'.", _parent_region->get_name(), get_name() );
	parent_ = _parent_region;
}


raw_composite_states region_pseudostate_impl::get_ancestors( composite_state* const _final_ancestor ) const
{
#ifdef Y_OPTIMIZE_4_SPEED
	if( ancestors_.empty() )
	{
		collect_ancestors( ancestors_, nullptr );
	}

	if( !_final_ancestor )
	{
		return( ancestors_ );
	}
	else
	{
		const auto final_ancestor = std::find( ancestors_.begin(), ancestors_.end(), _final_ancestor );

		if( final_ancestor != ancestors_.end() )
		{
			raw_composite_states ancestors( ancestors_.begin(), final_ancestor + 1 );
			return ( ancestors );
		}
		else
		{
			raw_composite_states ancestors;
			return ( ancestors );
		}
	}
#else
	raw_composite_states ancestors;
	collect_ancestors( ancestors, _final_ancestor );
	return( ancestors );
#endif
}


raw_regions region_pseudostate_impl::get_ancestors_as_regions() const
{
#ifdef Y_OPTIMIZE_4_SPEED
	if( ancestors_as_regions_.empty() )
	{
		collect_ancestors_as_regions( ancestors_as_regions_ );
	}
	return ( ancestors_as_regions_ );	
#else
	raw_regions ancestor_regions;
	collect_ancestors_as_regions( ancestor_regions );
	return( ancestor_regions );
#endif
}


void region_pseudostate_impl::collect_ancestors( raw_composite_states& _ancestors,
	composite_state* const _final_ancestor ) const
{
	Y_LOG( log_level::LL_SPAM, "Getting parent region for '%'.", get_name() );
	const auto parent_region = get_parent_region();
	if( parent_region )
	{
		Y_LOG( log_level::LL_SPAM, "Parent region '%' found for '%'.", parent_region->get_name(), get_name() );
		auto& parent_state = parent_region->get_parent_state();
		Y_LOG( log_level::LL_SPAM, "Found parent state '%' for region '%'.",
					 parent_state.get_name(), parent_region->get_name() );
		_ancestors.push_back( &parent_state );
		Y_LOG( log_level::LL_SPAM, "Searching for ancestor(s) of '%'.", parent_state.get_name() );
		const auto ancestors_of_parent_state = parent_state.get_ancestors( _final_ancestor );
		Y_LOG( log_level::LL_SPAM, "Found % ancestor(s) of '%'.", ancestors_of_parent_state.size(),
					 parent_state.get_name() );
		_ancestors.insert( _ancestors.end(), ancestors_of_parent_state.begin(), ancestors_of_parent_state.end() );
	}
}


void region_pseudostate_impl::collect_ancestors_as_regions( raw_regions& _ancestor_regions ) const
{
	const auto parent_region = get_parent_region();
	if( parent_region )
	{
		_ancestor_regions.push_back( parent_region );
		const auto parent_regions_of_parent_region = parent_region->get_parent_state().get_ancestors_as_regions();
		_ancestor_regions.insert( _ancestor_regions.end(), parent_regions_of_parent_region.begin(),
															parent_regions_of_parent_region.end() );
	}
}


}
