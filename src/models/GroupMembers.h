/**
 *
 *  GroupMembers.h
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#pragma once
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <drogon/orm/SqlBinder.h>
#include <drogon/orm/Mapper.h>
#ifdef __cpp_impl_coroutine
#include <drogon/orm/CoroMapper.h>
#endif
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <json/json.h>
#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <iostream>

namespace drogon
{
namespace orm
{
class DbClient;
using DbClientPtr = std::shared_ptr<DbClient>;
}
}
namespace drogon_model
{
namespace database_test
{
class ChatGroups;
class Users;

class GroupMembers
{
  public:
    struct Cols
    {
        static const std::string _id;
        static const std::string _createdAt;
        static const std::string _updatedAt;
        static const std::string _groupId;
        static const std::string _userId;
    };

    const static int primaryKeyNumber;
    const static std::string tableName;
    const static bool hasPrimaryKey;
    const static std::string primaryKeyName;
    using PrimaryKeyType = int32_t;
    const PrimaryKeyType &getPrimaryKey() const;

    /**
     * @brief constructor
     * @param r One row of records in the SQL query result.
     * @param indexOffset Set the offset to -1 to access all columns by column names,
     * otherwise access all columns by offsets.
     * @note If the SQL is not a style of 'select * from table_name ...' (select all
     * columns by an asterisk), please set the offset to -1.
     */
    explicit GroupMembers(const drogon::orm::Row &r, const ssize_t indexOffset = 0) noexcept;

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     */
    explicit GroupMembers(const Json::Value &pJson) noexcept(false);

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     * @param pMasqueradingVector The aliases of table columns.
     */
    GroupMembers(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false);

    GroupMembers() = default;

    void updateByJson(const Json::Value &pJson) noexcept(false);
    void updateByMasqueradedJson(const Json::Value &pJson,
                                 const std::vector<std::string> &pMasqueradingVector) noexcept(false);
    static bool validateJsonForCreation(const Json::Value &pJson, std::string &err);
    static bool validateMasqueradedJsonForCreation(const Json::Value &,
                                                const std::vector<std::string> &pMasqueradingVector,
                                                    std::string &err);
    static bool validateJsonForUpdate(const Json::Value &pJson, std::string &err);
    static bool validateMasqueradedJsonForUpdate(const Json::Value &,
                                          const std::vector<std::string> &pMasqueradingVector,
                                          std::string &err);
    static bool validJsonOfField(size_t index,
                          const std::string &fieldName,
                          const Json::Value &pJson,
                          std::string &err,
                          bool isForCreation);

    /**  For column id  */
    ///Get the value of the column id, returns the default value if the column is null
    const int32_t &getValueOfId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getId() const noexcept;
    ///Set the value of the column id
    void setId(const int32_t &pId) noexcept;

    /**  For column createdAt  */
    ///Get the value of the column createdAt, returns the default value if the column is null
    const ::trantor::Date &getValueOfCreatedat() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<::trantor::Date> &getCreatedat() const noexcept;
    ///Set the value of the column createdAt
    void setCreatedat(const ::trantor::Date &pCreatedat) noexcept;
    void setCreatedatToNull() noexcept;

    /**  For column updatedAt  */
    ///Get the value of the column updatedAt, returns the default value if the column is null
    const ::trantor::Date &getValueOfUpdatedat() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<::trantor::Date> &getUpdatedat() const noexcept;
    ///Set the value of the column updatedAt
    void setUpdatedat(const ::trantor::Date &pUpdatedat) noexcept;
    void setUpdatedatToNull() noexcept;

    /**  For column groupId  */
    ///Get the value of the column groupId, returns the default value if the column is null
    const int32_t &getValueOfGroupid() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getGroupid() const noexcept;
    ///Set the value of the column groupId
    void setGroupid(const int32_t &pGroupid) noexcept;

    /**  For column userId  */
    ///Get the value of the column userId, returns the default value if the column is null
    const int32_t &getValueOfUserid() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getUserid() const noexcept;
    ///Set the value of the column userId
    void setUserid(const int32_t &pUserid) noexcept;


    static size_t getColumnNumber() noexcept {  return 5;  }
    static const std::string &getColumnName(size_t index) noexcept(false);

    Json::Value toJson() const;
    Json::Value toMasqueradedJson(const std::vector<std::string> &pMasqueradingVector) const;
    /// Relationship interfaces
    void getUser(const drogon::orm::DbClientPtr &clientPtr,
                 const std::function<void(Users)> &rcb,
                 const drogon::orm::ExceptionCallback &ecb) const;
    void getGroup(const drogon::orm::DbClientPtr &clientPtr,
                  const std::function<void(ChatGroups)> &rcb,
                  const drogon::orm::ExceptionCallback &ecb) const;
  private:
    friend drogon::orm::Mapper<GroupMembers>;
#ifdef __cpp_impl_coroutine
    friend drogon::orm::CoroMapper<GroupMembers>;
#endif
    static const std::vector<std::string> &insertColumns() noexcept;
    void outputArgs(drogon::orm::internal::SqlBinder &binder) const;
    const std::vector<std::string> updateColumns() const;
    void updateArgs(drogon::orm::internal::SqlBinder &binder) const;
    ///For mysql or sqlite3
    void updateId(const uint64_t id);
    std::shared_ptr<int32_t> id_;
    std::shared_ptr<::trantor::Date> createdat_;
    std::shared_ptr<::trantor::Date> updatedat_;
    std::shared_ptr<int32_t> groupid_;
    std::shared_ptr<int32_t> userid_;
    struct MetaData
    {
        const std::string colName_;
        const std::string colType_;
        const std::string colDatabaseType_;
        const ssize_t colLength_;
        const bool isAutoVal_;
        const bool isPrimaryKey_;
        const bool notNull_;
    };
    static const std::vector<MetaData> metaData_;
    bool dirtyFlag_[5]={ false };
  public:
    static const std::string &sqlForFindingByPrimaryKey()
    {
        static const std::string sql="select * from " + tableName + " where id = ?";
        return sql;
    }

    static const std::string &sqlForDeletingByPrimaryKey()
    {
        static const std::string sql="delete from " + tableName + " where id = ?";
        return sql;
    }
    std::string sqlForInserting(bool &needSelection) const
    {
        std::string sql="insert into " + tableName + " (";
        size_t parametersCount = 0;
        needSelection = false;
            sql += "id,";
            ++parametersCount;
        if(dirtyFlag_[1])
        {
            sql += "createdAt,";
            ++parametersCount;
        }
        if(dirtyFlag_[2])
        {
            sql += "updatedAt,";
            ++parametersCount;
        }
        if(dirtyFlag_[3])
        {
            sql += "groupId,";
            ++parametersCount;
        }
        if(dirtyFlag_[4])
        {
            sql += "userId,";
            ++parametersCount;
        }
        needSelection=true;
        if(parametersCount > 0)
        {
            sql[sql.length()-1]=')';
            sql += " values (";
        }
        else
            sql += ") values (";

        sql +="default,";
        if(dirtyFlag_[1])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[2])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[3])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[4])
        {
            sql.append("?,");

        }
        if(parametersCount > 0)
        {
            sql.resize(sql.length() - 1);
        }
        sql.append(1, ')');
        LOG_TRACE << sql;
        return sql;
    }
};
} // namespace database_test
} // namespace drogon_model
