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

ResponseWrapper AccountService::handle_message(const RequestWrapper& wrapper)
{
    ResponseWrapper response_wrapper;

    switch (wrapper.request_type()) {
        case RequestWrapper::REQUESTACCOUNTDATA: {
            RequestAccountData request_data;
            if (request_data.ParseFromString(wrapper.request_payload())) {
                ResponseAccountData response_data = repository_->get_account_data(request_data);

                response_wrapper.set_response_type(ResponseWrapper::RESPONSEACCOUNTDATA);
                response_wrapper.set_response_id(wrapper.request_id());
                response_wrapper.set_response_payload(response_data.SerializeAsString());

                return response_wrapper;
            }
        }
    }
}
