#include "AccountService.h"

AccountService::AccountService(const std::string dburi)
{
    repository_ = new AccountRepository(dburi);
}

AccountService::~AccountService()
{
    delete repository_;
    repository_ = nullptr;
}

DatabaseResponseWrapper AccountService::handle_message(const AccountServiceRequestWrapper& wrapper)
{
	switch (wrapper.request_case()) {
		case AccountServiceRequestWrapper::kRequestAccountData: {
			DatabaseResponseWrapper response_wrapper;
			auto* account_service_Wrapper = response_wrapper.mutable_account_service_response();
			account_service_Wrapper->mutable_response_account_data()->CopyFrom(repository_->get_account_data(wrapper.request_account_data()));
			return response_wrapper;
		}
		default:
			DatabaseResponseWrapper empty_wrapper;
			std::cout << "Unknown message..." << std::endl;
			return empty_wrapper;
			break;
	}
}
