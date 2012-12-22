/*
 * Copyright 2006-2012 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "metadata.h"
#include "jdbc.h"
#include <zorba/singleton_item_sequence.h>

namespace zorba
{
namespace jdbc
{


ItemSequence_t
MetadataFunction::evaluate(const ExternalFunction::Arguments_t& args,
                           const zorba::StaticContext* aStaticContext,
                           const zorba::DynamicContext* aDynamincContext) const
{
  JdbcModule::init(aStaticContext);
  Item result;

  JDBC_MODULE_TRY
    String lStatementUUID = JdbcModule::getStringArg(args, 0);

    jobject oStatement = JdbcModule::getObject(aDynamincContext, lStatementUUID, INSTANCE_MAP_STATEMENTS);

    zorba::ItemFactory* itemFactory = Zorba::getInstance(0)->getItemFactory();

    int rowsAffected = JdbcModule::env->CallIntMethod(oStatement, JdbcModule::jStatement.getUpdateCount);
    CHECK_EXCEPTION

    std::vector<std::pair<zorba::Item, zorba::Item> > vResult;
    if (rowsAffected==-1) { // NON UPDATE QUERY
      jobject oResultSet = JdbcModule::env->CallObjectMethod(oStatement, JdbcModule::jStatement.getResultSet);
      CHECK_EXCEPTION

      jobject oMetadata = JdbcModule::env->CallObjectMethod(oResultSet, JdbcModule::jResultSet.getMetaData);
      CHECK_EXCEPTION

      int columns = JdbcModule::env->CallIntMethod(oMetadata, JdbcModule::jResultSetMetadata.getColumnCount);
      CHECK_EXCEPTION

      std::vector<zorba::Item> elements;

      for (int i=1; i<=columns; i++) {
          std::vector<std::pair<zorba::Item, zorba::Item> > column;

          jstring oName = (jstring) JdbcModule::env->CallObjectMethod(oMetadata, JdbcModule::jResultSetMetadata.getColumnName, i);
          CHECK_EXCEPTION
          String sName = JdbcModule::env->GetStringUTFChars(oName, NULL);
          CHECK_EXCEPTION
          zorba::Item iName = itemFactory->createString(sName);
          std::pair<zorba::Item, zorba::Item> pName(itemFactory->createString("name"), iName);
          column.push_back(pName);

          jstring oType = (jstring) JdbcModule::env->CallObjectMethod(oMetadata, JdbcModule::jResultSetMetadata.getColumnTypeName, i);
          CHECK_EXCEPTION
          String  sType = JdbcModule::env->GetStringUTFChars(oType, NULL);
          CHECK_EXCEPTION 
          zorba::Item iType = itemFactory->createString(sType);
          std::pair<zorba::Item, zorba::Item> pType(itemFactory->createString("type"), iType);
          column.push_back(pType);
          elements.push_back(itemFactory->createJSONObject(column));
      }
      std::pair<zorba::Item, zorba::Item> allColumns(itemFactory->createString("columns"), itemFactory->createJSONArray(elements));
      vResult.push_back(allColumns);
    } else { // UPDATE QUERY
      std::pair<zorba::Item, zorba::Item> allColumns(itemFactory->createString("affectedrows"), itemFactory->createInteger(rowsAffected));
      vResult.push_back(allColumns);
    }
    result = itemFactory->createJSONObject(vResult);
  JDBC_MODULE_CATCH
  
  return ItemSequence_t(new SingletonItemSequence(result));
}

}}; // namespace zorba, jdbc
