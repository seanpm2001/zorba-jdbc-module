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

#include "parametermetadata.h"
#include "jdbc.h"
#include <zorba/singleton_item_sequence.h>

namespace zorba
{
namespace jdbc
{


ItemSequence_t
ParameterMetadataFunction::evaluate(const ExternalFunction::Arguments_t& args,
                           const zorba::StaticContext* aStaticContext,
                           const zorba::DynamicContext* aDynamincContext) const
{
  JdbcModule::init(aStaticContext);
  Item result;

  JDBC_MODULE_TRY
    String lStatementUUID = JdbcModule::getStringArg(args, 0);

    jobject oPreparedStatement = JdbcModule::getObject(aDynamincContext, lStatementUUID, INSTANCE_MAP_PREPAREDSTATEMENTS);

    jobject oParameterMetaData = env->CallObjectMethod(oPreparedStatement, jPreparedStatement.getParameterMetaData);
    CHECK_EXCEPTION

    int columns = env->CallIntMethod(oParameterMetaData, jParameterMetadata.getParameterCount);
    CHECK_EXCEPTION

    zorba::ItemFactory* itemFactory = Zorba::getInstance(0)->getItemFactory();
    std::vector<zorba::Item> elements;

    for (int i=1; i<=columns; i++) {
        std::vector<std::pair<zorba::Item, zorba::Item> > column;

        jstring oName = (jstring) env->CallObjectMethod(oParameterMetaData, jParameterMetadata.getParameterClassName, i);
        CHECK_EXCEPTION
        const char * cName = env->GetStringUTFChars(oName, 0);
        CHECK_EXCEPTION
        String sName(cName);
        zorba::Item iName = itemFactory->createString(sName);
        std::pair<zorba::Item, zorba::Item> pName(itemFactory->createString("name"), iName);
        column.push_back(pName);
        env->ReleaseStringUTFChars(oName, cName);
        CHECK_EXCEPTION 

        jstring oType = (jstring) env->CallObjectMethod(oParameterMetaData, jParameterMetadata.getParameterTypeName, i);
        CHECK_EXCEPTION
        const char * cType = env->GetStringUTFChars(oType, 0);
        CHECK_EXCEPTION 
        String  sType(cType);
        zorba::Item iType = itemFactory->createString(sType);
        std::pair<zorba::Item, zorba::Item> pType(itemFactory->createString("type"), iType);
        column.push_back(pType);
        elements.push_back(itemFactory->createJSONObject(column));
        env->ReleaseStringUTFChars(oType, cType);
        CHECK_EXCEPTION 
    }
    std::pair<zorba::Item, zorba::Item> allColumns(itemFactory->createString("columns"), itemFactory->createJSONArray(elements));
    std::vector<std::pair<zorba::Item, zorba::Item> > vResult;
    vResult.push_back(allColumns);
    result = itemFactory->createJSONObject(vResult);

  JDBC_MODULE_CATCH
  
  return ItemSequence_t(new SingletonItemSequence(result));
}

}}; // namespace zorba, jdbc
