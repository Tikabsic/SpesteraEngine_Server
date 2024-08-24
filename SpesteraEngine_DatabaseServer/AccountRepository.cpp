#include "AccountRepository.h"

ResponseAccountData AccountRepository::get_account_data(const RequestAccountData& data)
{
    mysqlx::Table accounts = db_->getSchema("spestera_db").getTable(table_name_);
    
    mysqlx::RowResult result = accounts.select("AccountName", "Password", "AccountId")
        .where("AccountName = :AccountName")
        .bind("AccountName", data.account_name())
        .execute();

    mysqlx::Row row = result.fetchOne();

    if (!row.isNull()){
        ResponseAccountData response_data;
        response_data.set_account_name(row[0].get<std::string>());
        response_data.set_account_password(row[1].get<std::string>());
        response_data.set_account_id(row[2].get<std::string>());
        response_data.set_account_password_to_verify(data.account_password_to_verify());
        return response_data;
    }
}
