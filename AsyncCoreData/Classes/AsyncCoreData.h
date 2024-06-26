//
//  AsyncCoreData.h
//  ZhuShouCustomize
//
//  Created by 罗亮富 on 2019/1/9.
//  Copyright © 2019年 罗亮富. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>
#import "UniqueValueProtocol.h"
#import "AsyncHelper.h"

#ifndef lazy
#define lazy readonly
#endif

#define QUERY_ENTITY(entityName) AsyncCoreData queryEntity:entityName \


@class AsyncCoreData;


@interface AsyncCoreData : NSObject


#pragma mark- insert/update

/**
 写入数据库（插入/更新）

 插入更新规则：
 1.数据模型uniqueValue(UniqueValueProtocol协议规范)值不为nil，则判断数据表中对应的uniqueValue值是否存在，若存在则更新，不存在则插入
 2.数据模型uniqueValue若为nil，则：
    a> 若数据模型本身是从数据库中读取的，则写入的时候将覆盖原来读取时候对应的那条数据
    b> 若数据模型不是来源于数据库中，则将数据插入到数据表中
 */
+(nullable NSError *)queryEntity:(nonnull NSString *)entityName
             saveModels:(nonnull NSArray<id<UniqueValueProtocol>> *)datas;

+(void)queryEntity:(nonnull NSString *)entityName
   saveModelsAsync:(NSArray<id<UniqueValueProtocol>> *_Nonnull)datas
        completion:(void (^_Nullable)(NSError *_Nullable))block;

//慎用，高效批量创建新数据，用在一次性创建大批量数据且数据不重复的情况，此方法会忽略uniqueValue的值，所有数据不判断重复直接插入
+(nullable NSError *)queryEntity:(nonnull NSString *)entityName
               createModelsAnyway:(nonnull NSArray<id<UniqueValueProtocol>> *)datas;

+(void)queryEntity:(nonnull NSString *)entityName
    createModelsAnywayAsync:(nonnull NSArray<id<UniqueValueProtocol>> *)datas
        completion:(void (^_Nullable)(NSError *_Nullable))block;
#pragma mark- delete

/**
 删除记录
 
 删除规则：
 1.数据模型uniqueValue(UniqueValueProtocol协议规范)值不为nil，则判断数据表中对应的uniqueValue值是否存在，若存在则删除，否则忽略
 2.数据模型uniqueValue若为nil，则：
    a> 若数据模型本身是从数据库中读取的，则删除数据模型对应的数据库记录
    b> 若数据模型不是来源于数据库中，则忽略
 */
+(nullable NSError *)queryEntity:(nonnull NSString *)entityName
           deleteModels:(nonnull NSArray<id<UniqueValueProtocol>> *)models;

+(void)queryEntity:(nonnull NSString *)entityName
 deleteModelsAsync:(nonnull NSArray<id<UniqueValueProtocol>> *)models
        completion:(void (^_Nullable)(NSError *_Nullable))block;


//根据数据模型uniqueValue值删除数据库中对应记录
+(nullable NSError *)queryEntity:(nonnull NSString *)entityName
    deleteModelsWithUniquevalues:(nonnull NSArray *)modelUniquevalues;

+(void)queryEntity:(nonnull NSString *)entityName
       deleteModelsWithUniquevaluesAsync:(nonnull NSArray *)modelUniquevalues
        completion:(void (^_Nullable)(NSError *_Nullable))block;


//按设定条件删除记录
+(nullable NSError *)queryEntity:(nonnull NSString *)entityName
    deleteModelsWithPredicate:(nullable NSPredicate *)predicate;

+(void)queryEntity:(nonnull NSString *)entityName
    deleteModelsWithPredicateAsync:(nullable NSPredicate *)predicate
                        completion:(void (^_Nullable)(NSError *_Nullable))block;


//按条件批量更新
+(nullable NSError *)queryEntity:(nonnull NSString *)entityName
updateModelsWithPredicate:(nullable NSPredicate *)predicate
             withValues:(nonnull NSArray *)values
                forKeys:(nonnull NSArray <NSString *>*)keys;

+(void)queryEntity:(nonnull NSString *)entityName
updateModelsWithPredicateAsync:(nullable NSPredicate *)predicate
             withValues:(nonnull NSArray *)values
                forKeys:(nonnull NSArray <NSString *>*)keys
             completion:(void (^_Nullable)(NSError *_Nullable))block;


#pragma mark- filt out

/**
 按照条件查询数据库

 @param entityName 数据表
 @param predicate 查询条件
 @param range 读取数据范围（就像是分页一样）
 @param sortKey 排序的字段，注意这个字段是数据库表的字段，而非数据模型的字段
 @param reverse 是否按照插入顺序反序输出， YES先插入的在后面，NO后插入的在前面
 @return 返回查询到的“数据模型”数组
 */
+(nullable NSArray *)queryEntity:(nonnull NSString *)entityName
    modelsWithPredicate:(nullable NSPredicate *)predicate
                inRange:(NSRange)range
              sortByKey:(nullable NSString *)sortKey
                reverse:(BOOL)reverse;

+(void)queryEntity:(nonnull NSString *)entityName
modelsWithPredicateAsync:(nullable NSPredicate *)predicate
           inRange:(NSRange)range
         sortByKey:(nullable NSString *)sortKey
           reverse:(BOOL)reverse
        completion:(void (^_Nonnull)(NSArray *_Nullable))block;


/**
 查询指定表内指定列数据，根据分组及其它限制返回数据，返回数据格式字典数组
 例：@[@{@"name": xxx, @"country": xxx}, ...]
 @param entityName 表名
 @param keyPathes 指定列名称, 可参考NSFetchRequest.h内propertiesToFetch属性
 @param groups 指定分组, 可参考NSFetchRequest.h内propertiesToGroupBy属性
 @param predicate 谓词限制
 @param sortKeyPath 排序的字段，注意这个字段是数据库表的字段，而非数据模型的字段
 @param range 读取数据范围（就像是分页一样）
 @param reverse 是否按照插入顺序反序输出， YES先插入的在后面，NO后插入的在前面
 @return NSArray<NSDictionary *> *
 */
+(NSArray<NSDictionary *> *_Nullable)queryEntity:(nonnull NSString *)entityName
                                       keyPathes:(NSArray *_Nullable)keyPathes
                                         groupby:(NSArray *_Nullable)groups
                                   withPredicate:(NSPredicate *_Nullable)predicate
                                     sortKeyPath:(NSString *_Nullable)sortKeyPath
                                         inRange:(NSRange)range
                                         reverse:(BOOL)reverse;

#pragma mark- statitic/count

+(NSUInteger)queryEntity:(nonnull NSString *)entityName
    numberOfItemsWithPredicate:(nullable NSPredicate *)predicate;

+(void)queryEntity:(nonnull NSString *)entityName
    numberOfItemsWithPredicateAsync:(nullable NSPredicate *)predicate
        completion:(void(^_Nullable)(NSUInteger ))block;



#pragma mark- statitic/massive

//function could be @"max:" @"min:" @"count"() @"sum:"
+(nonnull NSExpressionDescription *)expressionDescriptionOfFuction:(nonnull NSString *)func
                                                        forKeyPath:(nonnull NSString *)keyPath;

+(nullable NSNumber *)queryEntity:(nonnull NSString *)entityName
        valueWithFuction:(nonnull NSString *)func
                  forKey:(nonnull NSString *)key
           withPredicate:(nullable NSPredicate *)predicate;

+(void)queryEntity:(nonnull NSString *)entityName
    valueWithFuctionAsync:(nonnull NSString *)func
            forKey:(nonnull NSString *)key
     withPredicate:(nullable NSPredicate *)predicate
        completion:(void(^_Nullable)(NSNumber * _Nullable ))block;


+(NSArray<NSDictionary *> *_Nullable)queryEntity:(NSString *_Nonnull)entityName
                           sumValuesForKeyPathes:(NSArray *_Nullable)keyPathes
                                         groupby:(NSArray <NSString *>*_Nullable)groups
                                   withPredicate:(NSPredicate *_Nullable)predicate
                                     sortKeyPath:(NSString *_Nullable)sortKeyPath
                                         inRange:(NSRange)range;

+(void)queryEntity:(NSString *_Nonnull)entityName
sumValuesForKeyPathes:(NSArray *_Nullable)keyPathes
           groupby:(NSArray <NSString *>*_Nullable)groups
     withPredicate:(NSPredicate *_Nullable)predicate
       sortKeyPath:(NSString *_Nullable)sortKeyPath
           inRange:(NSRange)range
        completion:(void (^_Nullable)(NSArray<NSDictionary *> *_Nullable))block;


+(nullable id)modelForStoreUrl:(nonnull NSURL *)storeUrl;
+(nullable id)modelForStoreID:(nonnull NSManagedObjectID *)storeID;

#pragma mark- for subclasses
+(nullable NSManagedObject *)queryEntity:(NSString *_Nonnull)entityName
                 existingDBModelForModel:(__kindof NSObject<UniqueValueProtocol> *_Nullable)model
                               inContext:(nonnull NSManagedObjectContext *)context;


+(nullable NSArray<NSManagedObject *> *)queryEntity:(nonnull NSString *)entityName
                              dbModelsWithPredicate:(NSPredicate *_Nullable)predicate
                                   inRange:(NSRange)range
                                 sortByKey:(nullable NSString *)sortKey
                                   reverse:(BOOL)reverse
                                 inContext:(nonnull NSManagedObjectContext *)context;


+(nullable NSArray<NSManagedObject *> *)dbModelsWithFetchRequest:(nullable NSFetchRequest *)frqs
                                                inRange:(NSRange)range
                                              sortByKey:(nullable NSString *)sortKey
                                                reverse:(BOOL)reverse
                                              inContext:(nonnull NSManagedObjectContext *)context;


//因为NSManagedObject是跟特定NSManagedObjectContext相关的，当在操作NSManagedObject的时候要保证它对应的context还存在，所以这个方法的调用者有责任对context的生命周期进行维护
+(nullable NSManagedObject *)DBModelForStoreID:(nonnull NSManagedObjectID *)storeID inContext:(nonnull NSManagedObjectContext *)context;

//将数据库同步到磁盘 for subclass
+(nullable NSError *)synchronizeinContext:(nonnull NSManagedObjectContext *)context;

#pragma mark- 清理缓存
//ios会在收到内存警告到时候自动清理， osx需要程序员自己在适当的时机调用该方法
//+(void)clearUnNessesaryCachedData; //因为用了NSCache做缓存，不再需要手动清理

@end


#import "AsyncCoreData+Configration.h"

@interface NSObject (AsyncCoreData)
@property (nonatomic, nullable, readonly) NSManagedObjectID *storeID;
@property (nonatomic, nullable, readonly) NSURL *StoreUrl;
@end
