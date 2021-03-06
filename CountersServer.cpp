#include <memory>
#include <string>

#include "counters/CountersDecrementKafkaStoreConsumer.h"
#include "counters/CountersHandler.h"
#include "counters/CountersIncrementKafkaConsumer.h"
#include "pipeline/RedisPipelineBootstrap.h"
#include "platform/gcloud/GoogleCloudStorage.h"

namespace counters {

static pipeline::RedisPipelineBootstrap::Config config{
  redisHandlerFactory : [](pipeline::RedisPipelineBootstrap* bootstrap) -> std::shared_ptr<pipeline::RedisHandler> {
    return std::make_shared<CountersHandler>(bootstrap->getDatabaseManager(), bootstrap->getKafkaConsumerHelper());
  },

  kafkaConsumerFactoryMap :
      {{
           CountersIncrementKafkaConsumer::name(),
           [](const std::string& brokerList, const pipeline::KafkaConsumerConfig& consumerConfig,
              const std::string& offsetKey,
              pipeline::RedisPipelineBootstrap* bootstrap) -> std::shared_ptr<infra::kafka::AbstractConsumer> {
             return std::make_shared<CountersIncrementKafkaConsumer>(
                 brokerList, consumerConfig.topic, consumerConfig.partition, consumerConfig.groupId, offsetKey,
                 consumerConfig.lowLatency, bootstrap->getKafkaConsumerHelper());
           },
       },
       {
           CountersDecrementKafkaStoreConsumer::name(),
           [](const std::string& brokerList, const pipeline::KafkaConsumerConfig& consumerConfig,
              const std::string& offsetKey,
              pipeline::RedisPipelineBootstrap* bootstrap) -> std::shared_ptr<infra::kafka::AbstractConsumer> {
             return std::make_shared<CountersDecrementKafkaStoreConsumer>(
                 brokerList, consumerConfig.objectStoreBucketName, consumerConfig.objectStoreObjectNamePrefix,
                 consumerConfig.topic, consumerConfig.partition, consumerConfig.groupId, offsetKey,
                 consumerConfig.offsetKeySuffix, bootstrap->getKafkaConsumerHelper(),
                 std::make_shared<platform::gcloud::GoogleCloudStorage>());
           },
       }},

  databaseManagerFactory : nullptr,

  scheduledTaskQueueFactory : nullptr,

  rocksDbCfConfiguratorMap : {
      {
          pipeline::DatabaseManager::defaultColumnFamilyName(), CountersHandler::optimizeColumnFamily,
      },
  },

  rocksDbConfigurator : nullptr,

  singletonRedisHandler : false,  // in order to support transactions
};

static auto redisPipelineBootstrap = pipeline::RedisPipelineBootstrap::create(config);

}  // namespace counters
