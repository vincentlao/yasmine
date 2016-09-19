//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016 Seadex GmbH                                                                   //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/License.html.            //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef COMPOUND_TRANSITION_CONSUMER_01F0BAAD_6712_44B9_AA77_76A33352E801
#define COMPOUND_TRANSITION_CONSUMER_01F0BAAD_6712_44B9_AA77_76A33352E801


#include "transition_step_fwd.h"
#include "compound_transition_fwd.h"
#include "transition_kind.h"


namespace sxy
{


class vertex;
class region;
class composite_state;


class compound_transition_consumer
{
public:
	compound_transition_consumer() = default;
	virtual ~compound_transition_consumer() = default;
	compound_transition_consumer( const compound_transition_consumer& ) = delete;
	compound_transition_consumer& operator=( const compound_transition_consumer& ) = delete;
	virtual transition_steps& get_transition_steps() = 0;
	virtual const vertex& get_last_target() const = 0;
	virtual const compound_transitions& get_sub_compound_transitions() const = 0;
	virtual region * get_LCA_region() = 0;
	virtual composite_state * get_LCA_composite_state() = 0;
	virtual transition_kind get_transition_kind() = 0;
};


}


#endif
