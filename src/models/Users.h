/**
 *
 *  Users.h
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

		class Users
		{
		public:
			struct Cols
			{
				static const std::string _id;
				static const std::string _account;
				static const std::string _pwd;
				static const std::string _avatar;
				static const std::string _email;
				static const std::string _regisTime;
				static const std::string _updateTime;
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
			explicit Users(const drogon::orm::Row &r, const ssize_t indexOffset = 0) noexcept;

			/**
			 * @brief constructor
			 * @param pJson The json object to construct a new instance.
			 */
			explicit Users(const Json::Value &pJson) noexcept(false);

			/**
			 * @brief constructor
			 * @param pJson The json object to construct a new instance.
			 * @param pMasqueradingVector The aliases of table columns.
			 */
			Users(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false);

			Users() = default;

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
			/// Get the value of the column id, returns the default value if the column is null
			const int32_t &getValueOfId() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<int32_t> &getId() const noexcept;
			/// Set the value of the column id
			void setId(const int32_t &pId) noexcept;

			/**  For column account  */
			/// Get the value of the column account, returns the default value if the column is null
			const std::string &getValueOfAccount() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<std::string> &getAccount() const noexcept;
			/// Set the value of the column account
			void setAccount(const std::string &pAccount) noexcept;
			void setAccount(std::string &&pAccount) noexcept;
			void setAccountToNull() noexcept;

			/**  For column pwd  */
			/// Get the value of the column pwd, returns the default value if the column is null
			const std::string &getValueOfPwd() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<std::string> &getPwd() const noexcept;
			/// Set the value of the column pwd
			void setPwd(const std::string &pPwd) noexcept;
			void setPwd(std::string &&pPwd) noexcept;

			/**  For column avatar  */
			/// Get the value of the column avatar, returns the default value if the column is null
			const std::string &getValueOfAvatar() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<std::string> &getAvatar() const noexcept;
			/// Set the value of the column avatar
			void setAvatar(const std::string &pAvatar) noexcept;
			void setAvatar(std::string &&pAvatar) noexcept;
			void setAvatarToNull() noexcept;

			/**  For column email  */
			/// Get the value of the column email, returns the default value if the column is null
			const std::string &getValueOfEmail() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<std::string> &getEmail() const noexcept;
			/// Set the value of the column email
			void setEmail(const std::string &pEmail) noexcept;
			void setEmail(std::string &&pEmail) noexcept;
			void setEmailToNull() noexcept;

			/**  For column regisTime  */
			/// Get the value of the column regisTime, returns the default value if the column is null
			const ::trantor::Date &getValueOfRegistime() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<::trantor::Date> &getRegistime() const noexcept;
			/// Set the value of the column regisTime
			void setRegistime(const ::trantor::Date &pRegistime) noexcept;
			void setRegistimeToNull() noexcept;

			/**  For column updateTime  */
			/// Get the value of the column updateTime, returns the default value if the column is null
			const ::trantor::Date &getValueOfUpdatetime() const noexcept;
			/// Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
			const std::shared_ptr<::trantor::Date> &getUpdatetime() const noexcept;
			/// Set the value of the column updateTime
			void setUpdatetime(const ::trantor::Date &pUpdatetime) noexcept;
			void setUpdatetimeToNull() noexcept;

			static size_t getColumnNumber() noexcept { return 7; }
			static const std::string &getColumnName(size_t index) noexcept(false);

			Json::Value toJson() const;
			Json::Value toMasqueradedJson(const std::vector<std::string> &pMasqueradingVector) const;
			/// Relationship interfaces
		private:
			friend drogon::orm::Mapper<Users>;
#ifdef __cpp_impl_coroutine
			friend drogon::orm::CoroMapper<Users>;
#endif
			static const std::vector<std::string> &insertColumns() noexcept;
			void outputArgs(drogon::orm::internal::SqlBinder &binder) const;
			const std::vector<std::string> updateColumns() const;
			void updateArgs(drogon::orm::internal::SqlBinder &binder) const;
			/// For mysql or sqlite3
			void updateId(const uint64_t id);
			std::shared_ptr<int32_t> id_;
			std::shared_ptr<std::string> account_;
			std::shared_ptr<std::string> pwd_;
			std::shared_ptr<std::string> avatar_;
			std::shared_ptr<std::string> email_;
			std::shared_ptr<::trantor::Date> registime_;
			std::shared_ptr<::trantor::Date> updatetime_;
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
			bool dirtyFlag_[7] = {false};

		public:
			static const std::string &sqlForFindingByPrimaryKey()
			{
				static const std::string sql = "select * from " + tableName + " where id = $1";
				return sql;
			}

			static const std::string &sqlForDeletingByPrimaryKey()
			{
				static const std::string sql = "delete from " + tableName + " where id = $1";
				return sql;
			}
			std::string sqlForInserting(bool &needSelection) const
			{
				std::string sql = "insert into " + tableName + " (";
				size_t parametersCount = 0;
				needSelection = false;
				sql += "id,";
				++parametersCount;
				if (dirtyFlag_[1])
				{
					sql += "account,";
					++parametersCount;
				}
				if (dirtyFlag_[2])
				{
					sql += "pwd,";
					++parametersCount;
				}
				if (dirtyFlag_[3])
				{
					sql += "avatar,";
					++parametersCount;
				}
				if (dirtyFlag_[4])
				{
					sql += "email,";
					++parametersCount;
				}
				if (dirtyFlag_[5])
				{
					sql += "regisTime,";
					++parametersCount;
				}
				if (dirtyFlag_[6])
				{
					sql += "updateTime,";
					++parametersCount;
				}
				needSelection = true;
				if (parametersCount > 0)
				{
					sql[sql.length() - 1] = ')';
					sql += " values (";
				}
				else
					sql += ") values (";

				int placeholder = 1;
				char placeholderStr[64];
				size_t n = 0;
				sql += "default,";
				if (dirtyFlag_[1])
				{
					n = sprintf(placeholderStr, "$%d,", placeholder++);
					sql.append(placeholderStr, n);
				}
				if (dirtyFlag_[2])
				{
					n = sprintf(placeholderStr, "$%d,", placeholder++);
					sql.append(placeholderStr, n);
				}
				if (dirtyFlag_[3])
				{
					n = sprintf(placeholderStr, "$%d,", placeholder++);
					sql.append(placeholderStr, n);
				}
				if (dirtyFlag_[4])
				{
					n = sprintf(placeholderStr, "$%d,", placeholder++);
					sql.append(placeholderStr, n);
				}
				if (dirtyFlag_[5])
				{
					n = sprintf(placeholderStr, "$%d,", placeholder++);
					sql.append(placeholderStr, n);
				}
				if (dirtyFlag_[6])
				{
					n = sprintf(placeholderStr, "$%d,", placeholder++);
					sql.append(placeholderStr, n);
				}
				if (parametersCount > 0)
				{
					sql.resize(sql.length() - 1);
				}
				if (needSelection)
				{
					sql.append(") returning *");
				}
				else
				{
					sql.append(1, ')');
				}
				LOG_TRACE << sql;
				return sql;
			}
		};
	} // namespace database_test
} // namespace drogon_model