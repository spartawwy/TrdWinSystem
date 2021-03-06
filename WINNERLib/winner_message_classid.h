#ifndef WINNER_MESSAGE_CLASSID_H_
#define WINNER_MESSAGE_CLASSID_H_

namespace WINNERSystem
{
	const char* ClassIDArray[] = 
	{
		"GenericRequest",
		"HandShake",
		"Heartbeat",
		"LogMessage",
		 
		"RaidMaster",
		"RequestAck",
		 
		"TopicRequest",
		 
		"UserRequest",
		"UserRequestAck",

        "QuotationRequest",
        "QuotationMessage"

        //"RaidStateMessage",
        //"Listing",
        //"ListingVariable",
        //"GenericParam"
	};

}

#endif // WINNER_MESSAGE_CLASSID_H_