//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "behavior_exception.h"


namespace sxy
{


behavior_exception::behavior_exception( const event_sptr _event )
	: exception("Error handle exception."),
		error_event_( _event )
{	
	// Nothing to do...
}

	
behavior_exception::~behavior_exception() = default;


const event_sptr behavior_exception::get_error_event() const
{
	return( error_event_ );
}


}
