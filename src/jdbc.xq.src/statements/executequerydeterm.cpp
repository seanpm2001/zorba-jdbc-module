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

#include "executequerydeterm.h"
#include "jdbc.h"
#include "jsonitemsequence.h"

namespace zorba
{
namespace jdbc
{


ItemSequence_t
ExecuteQueryDeterministicFunction::evaluate(const ExternalFunction::Arguments_t& args,
                           const zorba::StaticContext* aStaticContext,
                           const zorba::DynamicContext* aDynamincContext) const
{
  JdbcModule::init(aStaticContext);
  jobject result=NULL;

  JDBC_MODULE_TRY
    String lConnectionUUID = JdbcModule::getStringArg(args, 0);
    String lQuery = JdbcModule::getStringArg(args, 1);

    jobject oConnection = JdbcModule::getObject(aDynamincContext, lConnectionUUID, INSTANCE_MAP_CONNECTIONS);

    jobject oStatement = JdbcModule::env->CallObjectMethod(oConnection, JdbcModule::jConnection.createStatement);
    CHECK_EXCEPTION
    jstring query =  JdbcModule::env->NewStringUTF(lQuery.c_str());
    CHECK_EXCEPTION
    result = JdbcModule::env->CallObjectMethod(oStatement, JdbcModule::jStatement.executeQuery, query);
    CHECK_EXCEPTION

  JDBC_MODULE_CATCH
  
  return ItemSequence_t(new JSONItemSequence(result));
}

}}; // namespace zorba, jdbc
