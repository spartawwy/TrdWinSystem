#include "wzf_client_socket.h"

#include <TLib/core/tsystem_time.h>

//#include "WZELib/wze_private_qr_msg.h"
//#include "WZELib/wze_query_msg.h"
//#include "WZELib/wze_transfer_msg.h"
//#include "WZELib/wze_product.h"

using namespace TSystem;
//using namespace WZFSystem;

WZFClientSocket::WZFClientSocket(ServerClientAppBase& app)
	: app_(app)
	, pconn_(nullptr)
{
}

void WZFClientSocket::Connection(const std::shared_ptr<TSystem::communication::Connection>& pconn)
{
	assert( pconn );
	pconn_ = pconn;
}
