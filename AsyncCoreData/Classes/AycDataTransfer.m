//
//  AycDataTransfer.m
//  AsyncCoreData
//
//  Created by 罗亮富 on 2020/12/15.
//

#import "AycDataTransfer.h"
#import <CoreData/CoreData.h>

@implementation AycDataTransfer

#define RETURN_IF_ERROR if(e) { \
    if(error)\
        *error = e;\
    return;\
}

+(void)copyDataOfModel:(NSString *)modelName from:(NSURL *)srcDBFileUrl to:(NSURL *)destDBFileUrl error:(NSError **)error {
    
    NSURL *coreDataModelFileUrl = [[NSBundle mainBundle] URLForResource:modelName withExtension:@"momd"];
    NSManagedObjectModel *mobjModel = [[NSManagedObjectModel alloc] initWithContentsOfURL:coreDataModelFileUrl];
    
    NSError *e = nil;
    
    NSPersistentStoreCoordinator *srcPersistantStoreCord = [self storeCoordinateWithModel:mobjModel fromFile:srcDBFileUrl error:&e];
    NSManagedObjectContext *srcContext = [[NSManagedObjectContext alloc] initWithConcurrencyType:NSPrivateQueueConcurrencyType];
    srcContext.persistentStoreCoordinator = srcPersistantStoreCord;
    
    RETURN_IF_ERROR
    
    [srcContext performBlockAndWait:^{
        NSError *error = nil;
        NSPersistentStoreCoordinator *destPersistantStoreCord = [self storeCoordinateWithModel:mobjModel fromFile:destDBFileUrl error:&error];
        NSManagedObjectContext *destContext = [[NSManagedObjectContext alloc] initWithConcurrencyType:NSPrivateQueueConcurrencyType];
        destContext.persistentStoreCoordinator = destPersistantStoreCord;
        
//        RETURN_IF_ERROR
        if (error) {
            return;
        }
        
    #if DEBUG
        NSLog(@"Start trasfer:\nfrom:%@\nto:%@\n---",srcDBFileUrl.path,destDBFileUrl.path);
    #endif
        [destContext performBlockAndWait:^{
            
            NSError *error = nil;
            
            for(NSEntityDescription *entDes in mobjModel.entities) {
        #if DEBUG
                NSLog(@"tranfer entity:%@",entDes.name);
        #endif
                NSFetchRequest *ferq = [NSFetchRequest fetchRequestWithEntityName:entDes.name];
                NSFetchRequest *ferq2 = [NSFetchRequest fetchRequestWithEntityName:entDes.name];
                
                BOOL shouldResolveConflict = NO;
                for(NSPropertyDescription *pdes in entDes.properties) {
                    if([pdes.name isEqualToString:@"uniqueID"]) {
                        shouldResolveConflict = YES;
                        break;
                    }
                }
                
                NSArray *objs = [srcContext executeFetchRequest:ferq error:&error];;
                if (error) {
                    return;
                }
    //            RETURN_IF_ERROR
                
                for(NSManagedObject *fromObj in objs) {
                    
                    //解决唯一索引冲突 用
                    if(shouldResolveConflict) {
                        id uniqueVal = uniqueVal = [fromObj valueForKey:@"uniqueID"];;
                    
                        if(uniqueVal) {
                            ferq2.predicate = [NSPredicate predicateWithFormat:@"uniqueID = %@",uniqueVal];
                        }
                        NSUInteger c = [destContext countForFetchRequest:ferq2 error:nil];
                    
                        if(c > 0)
                            continue;
                    }
                    NSManagedObject *toObj = [[NSManagedObject alloc] initWithEntity:entDes insertIntoManagedObjectContext:destContext];
                    for(NSPropertyDescription *pdes in entDes.properties) {
                        [toObj setValue:[fromObj valueForKey:pdes.name] forKey:pdes.name];
                    }
                }
            }
            [destContext save:&error];
        }];
    }];
    
    RETURN_IF_ERROR
    
#if DEBUG
    NSLog(@"\n===========<%s> FINISHED with error:%@",__PRETTY_FUNCTION__,e);
#endif
    
}

+(NSPersistentStoreCoordinator *)storeCoordinateWithModel:(NSManagedObjectModel *)model fromFile:(NSURL *)DBFile error:(NSError **)error {
    
    NSPersistentStoreCoordinator *persistantStoreCord = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:model];
    NSDictionary *options = @{
                                NSMigratePersistentStoresAutomaticallyOption:@YES,
                                NSInferMappingModelAutomaticallyOption:@YES,
                                };
    
    [persistantStoreCord addPersistentStoreWithType:NSSQLiteStoreType configuration:nil URL:DBFile options:options error:error];
    
    return persistantStoreCord;
}


@end
